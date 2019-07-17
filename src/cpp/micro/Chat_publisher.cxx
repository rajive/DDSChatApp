
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef rti_me_cpp_hxx
#include "rti_me_cpp.hxx"
#endif

#include "Chat.h"
#include "ChatSupport.h"
#include "ChatApplication.h"

using namespace DDS;

class ChatObjectDataWriterListener : public DataWriterListener
{
  public:
    ChatObjectDataWriterListener() : DataWriterListener() { }
    ~ChatObjectDataWriterListener() { }
};

int
publisher_main_w_args(Long domain_id, char *udp_intf, char *peer,
Long sleep_time, Long count)
{
    Application *application = NULL;

    Publisher *publisher = NULL;
    DataWriter *datawriter = NULL;
    My::Type::Chat::ObjDataWriter *hw_writer = NULL;
    DataWriterQos dw_qos;
    ReturnCode_t retcode;
    My::Type::Chat::Obj *sample = NULL;
    Long i;
    DataWriterListener *dw_listener = NULL;

    application = new Application();
    if (application == NULL)
    {
        printf("failed Application new\n");
        goto done;
    }

    retcode = application->initialize("publisher", 
    "subscriber", 
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

    publisher = application->participant->create_publisher(
        PUBLISHER_QOS_DEFAULT,NULL,STATUS_MASK_NONE);
    if (publisher == NULL)
    {
        printf("publisher == NULL\n");
        goto done;
    }

    retcode = publisher->get_default_datawriter_qos(dw_qos);
    if (retcode != RETCODE_OK)
    {
        printf("failed get_default_datawriter_qos\n");
        goto done;
    }

	#ifdef USE_NON_VOLATILE_QOS
	dw_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;
	dw_qos.durability.kind  = DDS_TRANSIENT_LOCAL_DURABILITY_QOS; /*>>><<<*/
	#else
    dw_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
    #endif
    dw_qos.resource_limits.max_samples = 32;
    dw_qos.resource_limits.max_samples_per_instance = 32;
    dw_qos.resource_limits.max_instances = 1;
    dw_qos.history.depth = 32;

    datawriter = publisher->create_datawriter(application->topic,
    dw_qos,NULL,STATUS_MASK_NONE);
    if (datawriter == NULL)
    {
        printf("datawriter == NULL\n");
        goto done;
    }

    retcode = application->enable();
    if (retcode != RETCODE_OK)
    {
        printf("failed to enable application\n");
        goto done;
    }

    hw_writer = My::Type::Chat::ObjDataWriter::narrow(datawriter);
    if (hw_writer == NULL)
    {
        printf("failed datawriter narrow\n");
        goto done;
    }

    sample = My::Type::Chat::ObjTypeSupport::create_data();
    if (sample == NULL)
    {
        printf("failed My::Type::Chat::Obj_create\n");
        return -1;
    }

	sprintf(sample->id, "Rajive (micro C++)"); /*>>><<<*/
    for (i = 0;
    (application->count > 0 && i < application->count) ||
    (application->count == 0);
    ++i)
    {
        /* TODO set sample attributes here */
    	sprintf(sample->content, "Micro C++ Hello %d", i); /*>>><<<*/
        printf("%s %s\n", sample->id, sample->content);

        retcode = hw_writer->write(*sample, HANDLE_NIL);
        if (retcode != RETCODE_OK)
        {
            printf("Failed to write sample\n");
        } 
        else
        {
            printf("Written sample %d\n",(i+1));
        } 

        OSAPI_Thread_sleep(application->sleep_time);
    }

    done:

    if (application != NULL)
    {
        delete application;
    }
    if (sample != NULL)
    {
        My::Type::Chat::ObjTypeSupport::delete_data(sample);
    } 
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

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#elif defined(RTI_VXWORKS)
int
publisher_main(void)
{
    /* Explicitly configure args below */
    Long i = 0;
    Long domain_id = 44;
    char *peer = "10.10.65.104";
    char *udp_intf = NULL;
    Long sleep_time = 1000;
    Long count = 0;

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#endif
