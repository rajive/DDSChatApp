#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Chat.h"
#include "ChatSupport.h"
#include "ChatApplication.h"

void
ChatPublisher_on_publication_matched(void *listener_data,
                                           DDS_DataWriter * writer,
                                           const struct
                                           DDS_PublicationMatchedStatus *status)
{
    if (status->current_count_change > 0)
    {
        printf("Matched a subscriber\n");
    }
    else if (status->current_count_change < 0)
    {
        printf("Unmatched a subscriber\n");
    }
}

int
publisher_main_w_args(DDS_Long sleep_time, DDS_Long count)
{
    DDS_DataWriter *datawriter;
    My_ChatObjectDataWriter *ChatObject_writer;
    DDS_ReturnCode_t retcode;
    My_ChatObject *sample = NULL;
    struct Application *application = NULL;
    DDS_Long i;
    struct DDS_DataWriterListener dw_listener =
        DDS_DataWriterListener_INITIALIZER;

    sample = My_ChatObject_create();
    if (sample == NULL)
    {
        printf("failed My_ChatObject_create\n");
        return 0;
    }

    application = Application_create(My_name_CHAT_PUB_IF, sleep_time, count);

    if (application == NULL)
    {
        printf("failed Application create\n");
        goto done;
    }

    datawriter = DDS_DomainParticipant_lookup_datawriter_by_name(
                     application->participant,
					 My_name_CHAT_WRITER);
    if (datawriter == NULL)
    {
        printf("datawriter not found: %s\n", My_name_CHAT_WRITER);
        goto done;
    }

    dw_listener.on_publication_matched =
        ChatPublisher_on_publication_matched;

    retcode = DDS_DataWriter_set_listener(datawriter, &dw_listener,
                                          DDS_PUBLICATION_MATCHED_STATUS);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to set writer listener\n");
        goto done;
    }

    retcode = Application_enable(application);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to enable application\n");
        goto done;
    }

    ChatObject_writer = My_ChatObjectDataWriter_narrow(datawriter);

    strncpy(sample->id, "Rajive (xml micro C)", My_MAX_SIZE);
    for (i = 0; (application->count > 0 && i < application->count) ||
         (application->count == 0); ++i)
    {
    	snprintf(sample->content, My_MAX_SIZE, "XML Micro C Hello World %d", i);
        printf("%s %s\n", sample->id, sample->content);

        retcode = My_ChatObjectDataWriter_write(ChatObject_writer, sample, &DDS_HANDLE_NIL);
        if (retcode != DDS_RETCODE_OK)
        {
            printf("Failed to write to sample\n");
        }

        OSAPI_Thread_sleep(application->sleep_time);
    }

  done:

    Application_delete(application);

    if (sample != NULL)
    {
        My_ChatObject_delete(sample);
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
        if (!strcmp(argv[i], "-sleep"))
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
            Application_help(argv[0]);
            return 0;
        }
        else
        {
            printf("unknown option: %s\n", argv[i]);
            return -1;
        }
    }

    return publisher_main_w_args(sleep_time, count);
}
#elif defined(RTI_VXWORKS)
int
publisher_main(void)
{
    /* Explicitly configure args below */
    DDS_Long sleep_time = 1000;
    DDS_Long count = 0;

    return publisher_main_w_args(sleep_time, count);
}
#endif
