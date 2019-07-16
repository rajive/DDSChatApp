#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_cpp.hxx"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Chat.h"
#include "ChatSupport.h"
#include "ChatApplication.h"

class ChatObjectDataWriterListener : public DDSDataWriterListener
{
public:
    ChatObjectDataWriterListener() : DDSDataWriterListener() { }
    virtual ~ChatObjectDataWriterListener() { }

    void on_publication_matched	(DDSDataWriter *writer,
    							 const DDS_PublicationMatchedStatus& status) {
    	if (status.current_count_change > 0)
    	{
    		printf("Matched a subscriber\n");
    	}
    	else if (status.current_count_change < 0)
    	{
    		printf("Unmatched a subscriber\n");
    	}
    }
};

int
publisher_main_w_args(DDS_Long domain_id, char *udp_intf, char *peer,
                      DDS_Long sleep_time, DDS_Long count)
{
    Application *application = NULL;

    DDSDataWriter *datawriter = NULL;
    My::ChatObjectDataWriter *ChatObject_writer = NULL;
    DDS_ReturnCode_t retcode;
    My::ChatObject *sample = NULL;
    DDS_Long i;
    DDSDataWriterListener *dw_listener = new ChatObjectDataWriterListener();


    application = new Application();
    if (application == NULL)
    {
        printf("failed Application new\n");
        goto done;
    }

    retcode = application->initialize(My::If::PUB, sleep_time, count);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed Application initialize\n");
        goto done;
    }

    datawriter = application->participant->lookup_datawriter_by_name(My::Topic::Chat::WRITER);
    if (datawriter == NULL)
    {
        printf("datawriter not found: %s\n", My::Topic::Chat::WRITER);
        goto done;
    }

    retcode = datawriter->set_listener(dw_listener, DDS_PUBLICATION_MATCHED_STATUS);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to set writer listener\n");
        goto done;
    }

    retcode = application->enable();
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to enable application\n");
        goto done;
    }

    ChatObject_writer = My::ChatObjectDataWriter::narrow(datawriter);
    if (ChatObject_writer == NULL)
    {
        printf("failed datawriter narrow\n");
        goto done;
    }

    sample = My::ChatObjectTypeSupport::create_data();
    if (sample == NULL)
    {
        printf("failed My::ChatObject_create\n");
        return -1;
    }

	snprintf(sample->id, My::MAX_SIZE, "Rajive (xml micro C++)");
    for (i = 0;
        (application->count > 0 && i < application->count) ||
        (application->count == 0);
        ++i)
    {
    	snprintf(sample->content, My::MAX_SIZE, "XML Micro C++ Hello World %d", i);
        printf("%s %s\n", sample->id, sample->content);

        retcode = ChatObject_writer->write(*sample, DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK)
        {
            printf("Failed to write to sample\n");
        }

        OSAPI_Thread_sleep(application->sleep_time);
    }

  done:

    if (application != NULL)
    {
        delete application;
    }

    if (dw_listener != NULL)
    {
        delete dw_listener;
    }

    if (sample != NULL)
    {
        My::ChatObjectTypeSupport::delete_data(sample);
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

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#elif defined(RTI_VXWORKS)
int
publisher_main(void)
{
    /* Explicitly configure args below */
    DDS_Long i = 0;
    DDS_Long domain_id = 44;
    char *peer = "10.10.65.104";
    char *udp_intf = NULL;
    DDS_Long sleep_time = 1000;
    DDS_Long count = 0;

    return publisher_main_w_args(domain_id, udp_intf, peer, sleep_time, count);
}
#endif
