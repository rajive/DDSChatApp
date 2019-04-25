#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"

#include "Chat.h"
#include "ChatSupport.h"
#include "ChatPlugin.h"
#include "ChatApplication.h"

/* See Wire Protocol Specification on http://www.omg.org/spec/DDSI-RTPS/2.2/
 * for more details about CDR encapsulation.
 */

/*ci \brief Unsigned Long type size */
#define UNSIGNED_LONG_SIZE  4

void
ChatSubscriber_on_data_available(void *listener_data,
                                       DDS_DataReader * reader)
{
    My_ChatObjectDataReader *hw_reader = My_ChatObjectDataReader_narrow(reader);
    DDS_ReturnCode_t retcode;
    struct DDS_SampleInfo *sample_info = NULL;
    My_ChatObject *sample = NULL;

    struct DDS_SampleInfoSeq info_seq = 
        DDS_SEQUENCE_INITIALIZER;
    struct My_ChatObjectSeq sample_seq =
        DDS_SEQUENCE_INITIALIZER;

    const DDS_Long TAKE_MAX_SAMPLES = 32;
    DDS_Long i;

    retcode = My_ChatObjectDataReader_take(hw_reader,
       &sample_seq, &info_seq, TAKE_MAX_SAMPLES,
       DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);

    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to take data, retcode(%d)\n", retcode);
        goto done;
    }

    /* Print each valid sample taken */
    for (i = 0; i < My_ChatObjectSeq_get_length(&sample_seq); ++i)
    {
        sample_info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

        if (sample_info->valid_data)
        {
            sample = My_ChatObjectSeq_get_reference(&sample_seq, i);
            printf("\nSample received\n\tid: %s\n\tcontent: %s\n",
                   sample->id, sample->content);
        }
        else
        {
            printf("\nSample received\n\tINVALID DATA\n");
        }
    }

    My_ChatObjectDataReader_return_loan(hw_reader, &sample_seq, &info_seq);

done:
    My_ChatObjectSeq_finalize(&sample_seq);
    DDS_SampleInfoSeq_finalize(&info_seq);
}

void
ChatSubscriber_on_subscription_matched(void *listener_data,
                                             DDS_DataReader * reader,
                                             const struct
                                             DDS_SubscriptionMatchedStatus
                                             *status)
{
    if (status->current_count_change > 0)
    {
        printf("Matched a publisher\n");
    }
    else if (status->current_count_change < 0)
    {
        printf("Unmatched a publisher\n");
    }
}

/*i
 * \brief Helper function to filter an My_ChatObject sample
 *
 * \param[in]  sample       A My_ChatObject data sample to filter
 * \param[out] drop_sample  Out parameter determining whether the sample
 *                          should be filtered out or not.
 */
void 
ChatSubscriber_filter_sample(My_ChatObject *sample,
                             DDS_Boolean *drop_sample)
{
    /* Example filter: drop samples where contents tarts with 'X' */
    *drop_sample = (sample->content[0] == 'X') ? DDS_BOOLEAN_TRUE : DDS_BOOLEAN_FALSE;
}

/*i
 * \brief Helper function to deserialize an unsigned long
 *
 * \param[in]  src_buffer      Pointer to CDR stream buffer
 * \param[in]  need_byte_swap  Indicates whether it is needed to swap bytes
 * \param[out] instance        Deserialized unsigned long
 */
void
ChatSubscriber_deserialize_unsigned_long(char **src_buffer,
                                               RTI_BOOL need_byte_swap,
                                               DDS_UnsignedLong *instance)
{
    RTI_INT32 i;
    if (need_byte_swap)
    {
        for (i = 3; i >= 0; --i)
        {
            *((RTI_INT8*)instance + i) = *((*src_buffer)++);
        }
    }
    else
    {
        *instance = *(RTI_UINT32*)(*src_buffer);
        (*src_buffer) += CDR_LONG_SIZE;
    }
}

/*i
 * \brief Implementation of \ref DDS_DataReaderListener::on_before_sample_deserialize
 */
DDS_Boolean
ChatSubscriber_on_before_sample_deserialize(
        void *listener_data,
        DDS_DataReader *reader,
        struct NDDS_Type_Plugin *plugin,
        struct CDR_Stream_t *stream,
        DDS_Boolean *dropped)
{
    DDS_Boolean result = DDS_BOOLEAN_FALSE;
    DDS_Long id = 0;
    RTI_BOOL need_byte_swap = DDS_BOOLEAN_FALSE;
    char *src_buffer = NULL;

    need_byte_swap = CDR_Stream_is_byte_swapped(stream);
    src_buffer = CDR_Stream_get_current_position_ptr(stream);

    /* deserialize only field 'id', instead of the whole sample */

    if (!CDR_Stream_check_size(stream, UNSIGNED_LONG_SIZE))
    {
        printf("Failed to deserialize id. The stream is too short, missing data\n");
        goto done;
    }

    /* Note primitive types must be aligned to their length in the CDR stream.
     * For example, a long must start on a 4-byte boundary. The boundaries are
     * counted from the start of the CDR stream.
     * As the sample 'id' is the first data in the stream it is already aligned.
     * Position 0 (beginning of the stream) is aligned to 4 (size of long).
     */
    ChatSubscriber_deserialize_unsigned_long(&src_buffer,
                                                   need_byte_swap,
                                                   (DDS_UnsignedLong*)&id);

    /* filter the sample */
    *dropped = (id % 2 == 0) ? DDS_BOOLEAN_TRUE : DDS_BOOLEAN_FALSE;

    if (*dropped)
    {
        printf("\nSample filtered, before deserialize...\n\tDROPPED - id: %d\n",
               id);
    }

    result = DDS_BOOLEAN_TRUE;
done:
    return result;
}

/*i
 * \brief Implementation of \ref DDS_DataReaderListener::on_before_sample_commit
 */
DDS_Boolean
ChatSubscriber_on_before_sample_commit(
        void *listener_data,
        DDS_DataReader *reader,
        const void *const sample,
        const struct DDS_SampleInfo *const sample_info,
        DDS_Boolean *dropped)
{
    My_ChatObject *hw_sample = (My_ChatObject *)sample;

    ChatSubscriber_filter_sample(hw_sample, dropped);

    if (*dropped)
    {
        printf("\nSample filtered, before commit...\n\tDROPPED - id: %s, content %s\n",
               hw_sample->id, hw_sample->content);
    }

    return DDS_BOOLEAN_TRUE;
}

int
subscriber_main_w_args(DDS_Long sleep_time, DDS_Long count)
{
    DDS_DataReader *datareader;
    DDS_ReturnCode_t retcode;
    struct DDS_DataReaderListener dr_listener =
        DDS_DataReaderListener_INITIALIZER;
    struct Application *application;

    application = Application_create(My_name_CHAT_SUB_IF, sleep_time, count);

    if (application == NULL)
    {
        printf("failed Application create\n");
        goto done;
    }


    datareader = DDS_DomainParticipant_lookup_datareader_by_name(
                     application->participant,
					 My_name_CHAT_READER);
    if (datareader == NULL)
    {
        printf("datareader not found %s\n", My_name_CHAT_READER);
        goto done;
    }

#ifdef USE_SAMPLE_FILTER
    /* choose one callback to enable */
#ifdef FILTER_ON_DESERIALIZE
    dr_listener.on_before_sample_deserialize =
        ChatSubscriber_on_before_sample_deserialize;
#else
    dr_listener.on_before_sample_commit = 
        ChatSubscriber_on_before_sample_commit;
#endif

#endif

    dr_listener.on_data_available = ChatSubscriber_on_data_available;
    dr_listener.on_subscription_matched =
        ChatSubscriber_on_subscription_matched;

    retcode = DDS_DataReader_set_listener(datareader, &dr_listener,
                                          DDS_DATA_AVAILABLE_STATUS |
                                          DDS_SUBSCRIPTION_MATCHED_STATUS);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to set reader listener\n");
        goto done;
    }

    retcode = Application_enable(application);
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

    Application_delete(application);

    return 0;
}

#if !(defined(RTI_VXWORKS) && !defined(__RTP__))
int
main(int argc, char **argv)
{
    DDS_Long i = 0;
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

    return subscriber_main_w_args(sleep_time, count);
}
#elif defined(RTI_VXWORKS)
int
subscriber_main(void)
{
    /* Explicitly configure args below */
    DDS_Long sleep_time = 1000;
    DDS_Long count = 0;

    return subscriber_main_w_args(sleep_time, count);
}
#endif
