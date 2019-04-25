
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef rti_me_cpp_hxx
#include "rti_me_cpp.hxx"
#endif

#include "Chat.h"
#include "ChatSupport.h"
#include "ChatPlugin.h"
#include "ChatApplication.h"

using namespace DDS;

class ChatObjectReaderListener : public DataReaderListener
{

  public:

    virtual void on_data_available(DataReader *reader);
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
subscriber_main_w_args(Long domain_id, char *udp_intf, char *peer,
Long sleep_time, Long count)
{
    Subscriber *subscriber = NULL;
    DataReader *datareader = NULL;
    ChatObjectReaderListener *listener  = new ChatObjectReaderListener();
    DataReaderQos dr_qos;
    ReturnCode_t retcode;
    Application *application = NULL;

    application = new Application();
    if (application == NULL)
    {
        printf("failed Application new\n");
        goto done;
    }

    retcode = application->initialize("subscriber", 
    "publisher", 
    domain_id,
    udp_intf, 
    peer, 
    sleep_time, 
    count);
    if (retcode != RETCODE_OK)
    {
        printf("failed Application initialize\n");
        goto done;
    }

    subscriber = application->participant->create_subscriber(
        SUBSCRIBER_QOS_DEFAULT,NULL,
        STATUS_MASK_NONE);
    if (subscriber == NULL)
    {
        printf("subscriber == NULL\n");
        goto done;
    }

    retcode = subscriber->get_default_datareader_qos(dr_qos);
    if (retcode != RETCODE_OK)
    {
        printf("failed get_default_datareader_qos\n");
        goto done;
    }

    dr_qos.resource_limits.max_samples = 32;
    dr_qos.resource_limits.max_instances = 1;
    dr_qos.resource_limits.max_samples_per_instance = 32;
    /* if there are more remote writers, you need to increase these limits */
    dr_qos.reader_resource_limits.max_remote_writers = 10;
    dr_qos.reader_resource_limits.max_remote_writers_per_instance = 10;
    dr_qos.history.depth = 32;

    /* Non Volatile QoS */

	#ifdef USE_NON_VOLATILE_QOS
	dr_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
	dr_qos.durability.kind  = DDS_TRANSIENT_LOCAL_DURABILITY_QOS; /*>>><<<*/
	#else
    dr_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
    #endif

    /* Only DATA_AVAILABLE_STATUS supported currently */
    datareader = subscriber->create_datareader(
        application->topic,
        dr_qos,
        listener,
        DATA_AVAILABLE_STATUS);

    if (datareader == NULL)
    {
        printf("datareader == NULL\n");
        goto done;
    }

    retcode = application->enable();
    if (retcode != RETCODE_OK)
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

    //DataReaderQos is automatically disposed by destructor

    return 0;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__))
int
main(int argc, char **argv)
{
    Long i = 0;
    Long domain_id = 0;
    char *peer = NULL;
    char *udp_intf = NULL;
    Long sleep_time = 1000;
    Long count = 0;

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
    Long domain_id = 44;
    char *peer = "10.10.65.103";
    char *udp_intf = NULL;
    Long sleep_time = 1000;
    Long count = 0;

    return subscriber_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#endif
