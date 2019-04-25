#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_cpp.hxx"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Chat.h"
#include "ChatSupport.h"
#include "ChatPlugin.h"
#include "ChatApplication.h"

class ChatObjectReaderListener : public DDSDataReaderListener
{

  public:

    virtual void on_data_available(DDSDataReader *reader);
};


template <typename T>
void take_and_print(typename T::DataReader* reader)
{
    ReturnCode_t retcode;
    SampleInfo sample_info;
    T *sample = NULL;

    sample = T::TypeSupport::create_data();
    if (sample == NULL)
    {
        printf("Failed sample initialize\n");
        return;
    }

    retcode = reader->take_next_sample(*sample, sample_info);
    while (retcode == RETCODE_OK)
    {
        if (sample_info.valid_data)
        {
            printf("\nValid sample received\n");
            /* TODO read sample attributes here */
            printf("id = %s,   content=%s\n", sample->id, sample->content); /*>>><<<*/
        }
        else
        {
            printf("\nSample received\n\tINVALID DATA\n");
        }
        retcode = reader->take_next_sample(*sample, sample_info);
    }

    T::TypeSupport::delete_data(sample);
}

void
ChatObjectReaderListener::on_data_available(DataReader * reader)
{
    My::ChatObjectDataReader *hw_reader = My::ChatObjectDataReader::narrow(reader);
    take_and_print<My::ChatObject>(hw_reader);
}

int
subscriber_main_w_args(DDS_Long domain_id, char *udp_intf, char *peer,
                       DDS_Long sleep_time, DDS_Long count)
{
    DDSDataReader *datareader = NULL;
    ChatObjectReaderListener *listener = new ChatObjectReaderListener();
    DDS_ReturnCode_t retcode;
    Application *application = NULL;
    RT_Registry_T *registry = NULL;

    application = new Application();
    if (application == NULL)
    {
        printf("failed Application new\n");
        goto done;
    }

    retcode = application->initialize(My::name::CHAT_SUB_IF, sleep_time, count);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed Application initialize\n");
        goto done;
    }

    datareader = application->participant->lookup_datareader_by_name(My::name::CHAT_READER);
    if (datareader == NULL)
    {
        printf("datareader not found %s\n", My::name::CHAT_READER);
        goto done;
    }

    retcode = datareader->set_listener(listener,
                                       DDS_DATA_AVAILABLE_STATUS |
                                       DDS_SUBSCRIPTION_MATCHED_STATUS);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to set listener\n");
        goto done;
    }

    retcode = application->enable();
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to enable application\n");
        goto done;
    }

    if (application->count != 0)
    {
        printf("Running for %d seconds, press Ctrl-C to exit\n",
               application->count);
        OSAPI_Thread_sleep(application->count * 1000);
    }
    else
    {
        int sleep_loop_count =  (24 * 60 * 60) / 2000;
        int sleep_loop_left = (24 * 60 * 60) % 2000;

        printf("Running for 24 hours, press Ctrl-C to exit\n");

        while (sleep_loop_count)
        {
            OSAPI_Thread_sleep(2000  * 1000); /* sleep is in ms */
            --sleep_loop_count;
        }

        OSAPI_Thread_sleep(sleep_loop_left * 1000);
    }

  done:

    if (application != NULL)
    {
        delete application;
    }

    if (listener != NULL)
    {
        delete listener;
    }

    return 0;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__))
int
main(int argc, char **argv)
{
    DDS_Long i = 0;
    DDS_Long domain_id = 0;
    char *peer = NULL;
    char *udp_intf = NULL;
    DDS_Long sleep_time = 1000;
    DDS_Long count = 0;

    for (i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-domain"))
        {
            ++i;
            if (i == argc)
            {
                printf("-domain <domain_id>\n");
                return -1;
            }
            domain_id = strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-udp_intf"))
        {
            ++i;
            if (i == argc)
            {
                printf("-udp_intf <interface>\n");
                return -1;
            }
            udp_intf = argv[i];
        }
        else if (!strcmp(argv[i], "-peer"))
        {
            ++i;
            if (i == argc)
            {
                printf("-peer <address>\n");
                return -1;
            }
            peer = argv[i];
        }
        else if (!strcmp(argv[i], "-sleep"))
        {
            ++i;
            if (i == argc)
            {
                printf("-sleep_time <sleep_time>\n");
                return -1;
            }
            sleep_time = strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-count"))
        {
            ++i;
            if (i == argc)
            {
                printf("-count <count>\n");
                return -1;
            }
            count = strtol(argv[i], NULL, 0);
        }
        else if (!strcmp(argv[i], "-h"))
        {
            Application::help(argv[0]);
            return 0;
        }
        else
        {
            printf("unknown option: %s\n", argv[i]);
            return -1;
        }
    }

    return subscriber_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#elif defined(RTI_VXWORKS)
int
subscriber_main(void)
{
    /* Explicitly configure args below */
    DDS_Long domain_id = 44;
    char *peer = "10.10.65.103";
    char *udp_intf = NULL;
    DDS_Long sleep_time = 1000;
    DDS_Long count = 0;

    return subscriber_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#endif


