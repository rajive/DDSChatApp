#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_cpp.hxx"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "MyService.h"
#include "MyServiceSupport.h"
#include "MyServicePlugin.h"
#include "ChatApplication.h"

class ChatObjectDataReaderListener : public DDSDataReaderListener
{
public:
    ChatObjectDataReaderListener() : DDSDataReaderListener() { }
    virtual ~ChatObjectDataReaderListener() { }

	virtual void on_subscription_matched(DDSDataReader *reader,
										 const DDS_SubscriptionMatchedStatus& status);

    virtual void on_data_available(DDSDataReader *reader);
};

void
ChatObjectDataReaderListener::on_subscription_matched(DDSDataReader *reader,
		 	 	 	 	 	 	 	 	 const DDS_SubscriptionMatchedStatus& status)
{
    if (status.current_count_change > 0)
    {
        printf("Matched a publisher\n");
    }
    else if (status.current_count_change < 0)
    {
        printf("Unmatched a publisher\n");
    }
}

void
ChatObjectDataReaderListener::on_data_available(DataReader * reader)
{
    My::Type::Chat::ObjDataReader *ChatObject_reader = My::Type::Chat::ObjDataReader::narrow(reader);
    My::Type::Chat::ObjSeq sample_seq;
    SampleInfoSeq info_seq;
    ReturnCode_t retcode;

    retcode = ChatObject_reader->take(sample_seq, info_seq, LENGTH_UNLIMITED,
    								ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
    if (retcode == RETCODE_NO_DATA) {
        printf("no data (xml micro C++) %d\n", retcode);
        return;
    } else if (retcode != RETCODE_OK) {
        printf("take error (xml micro C++) %d\n", retcode);
        return;
    }

    for (int i = 0; i < sample_seq.length(); ++i) {
        if (info_seq[i].valid_data) {
            printf("\nSample received (xml micro C++)\n\tid: %s\n\tcontent: %s\n",
            		sample_seq[i].id, sample_seq[i].content);
        }
        else {
            printf("\nSample received (xml micro C++)\n\tINVALID DATA\n");
        }
    }

    retcode = ChatObject_reader->return_loan(sample_seq, info_seq);
    if (retcode != RETCODE_OK) {
        printf("return loan error (xml micro C++) %d\n", retcode);
    }
}

int
subscriber_main_w_args(DDS_Long domain_id, char *udp_intf, char *peer,
                       DDS_Long sleep_time, DDS_Long count)
{
    DDSDataReader *datareader = NULL;
    ChatObjectDataReaderListener *listener = new ChatObjectDataReaderListener();
    DDS_ReturnCode_t retcode;
    Application *application = NULL;
    RT_Registry_T *registry = NULL;

    application = new Application();
    if (application == NULL)
    {
        printf("failed Application new\n");
        goto done;
    }

    retcode = application->initialize(My::If::SUB, sleep_time, count);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed Application initialize\n");
        goto done;
    }

    datareader = application->participant->lookup_datareader_by_name(My::Topic::Chat::READER);
    if (datareader == NULL)
    {
        printf("datareader not found %s\n", My::Topic::Chat::READER);
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


