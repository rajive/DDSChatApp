#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"

#include "MyServiceSupport.h"

#include "Databus.h"
#include "App.h"
#include "Filters.h"


void
My_Topic_Chat_input(
        void *listener_data,
        DDS_DataReader * reader) {

    My_Type_Chat_ObjDataReader *ChatObject_reader = My_Type_Chat_ObjDataReader_narrow(reader);
    DDS_ReturnCode_t retcode;

    struct DDS_SampleInfoSeq info_seq =  DDS_SEQUENCE_INITIALIZER;
    struct My_Type_Chat_ObjSeq sample_seq = DDS_SEQUENCE_INITIALIZER;


    retcode = My_Type_Chat_ObjDataReader_take(ChatObject_reader,
            &sample_seq, &info_seq, DDS_LENGTH_UNLIMITED,
            DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);
    if (retcode != DDS_RETCODE_OK) {
        printf("failed to take data, retcode(%d)\n", retcode);
        goto done;
    }

    /* Print each valid sample taken */
    for (int i = 0; i < My_Type_Chat_ObjSeq_get_length(&sample_seq); ++i) {
        struct DDS_SampleInfo* sample_info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

        if (sample_info->valid_data) {
            My_Type_Chat_Obj* sample = My_Type_Chat_ObjSeq_get_reference(&sample_seq, i);
            printf("\nSample received (xml micro C)\n\tid: %s\n\tcontent: %s\n",
                    sample->id, sample->content);
        }
        else {
            printf("\nSample received (xml micro C)\n\tINVALID DATA\n");
        }
    }

    My_Type_Chat_ObjDataReader_return_loan(ChatObject_reader, &sample_seq, &info_seq);

done:
    My_Type_Chat_ObjSeq_finalize(&sample_seq);
    DDS_SampleInfoSeq_finalize(&info_seq);
}

void
My_Subscriber_on_subscription_matched(
        void *listener_data,
        DDS_DataReader * reader,
        const struct DDS_SubscriptionMatchedStatus *status) {
    if (status->current_count_change > 0) {
        printf("Matched a publisher\n");
    }
    else if (status->current_count_change < 0) {
        printf("Unmatched a publisher\n");
    }
}

int
subscriber_main_w_args(long sleep_time, long count) {
    struct Databus *databus = NULL;
    DDS_DataReader *datareader = NULL;
    DDS_ReturnCode_t retcode = DDS_RETCODE_OK;

    struct DDS_DataReaderListener dr_listener   = DDS_DataReaderListener_INITIALIZER;
    dr_listener.on_subscription_matched         = My_Subscriber_on_subscription_matched;
    dr_listener.on_data_available               = My_Topic_Chat_input;
#ifdef USE_SAMPLE_FILTER /* choose one callback to enable */
#ifdef FILTER_ON_DESERIALIZE
    dr_listener.on_before_sample_deserialize    = My_Type_Chat_Obj_on_before_sample_deserialize;
#else
    dr_listener.on_before_sample_commit         = My_Type_Chat_Obj_on_before_sample_commit;
#endif
#endif


    assert((databus = Databus_create(My_If_SUB))
            != NULL
    );
    assert((datareader = DDS_DomainParticipant_lookup_datareader_by_name(
            databus->participant,
            My_Topic_Chat_READER))
            != NULL
    );
    assert(DDS_DataReader_set_listener(datareader, &dr_listener,
            DDS_DATA_AVAILABLE_STATUS |
            DDS_SUBSCRIPTION_MATCHED_STATUS)
            == DDS_RETCODE_OK
    );
    assert(Databus_enable(databus)
            == DDS_RETCODE_OK
    );

    App_sleep(count);

done:
    Databus_delete(databus);
    return 0;
}

int
main(int argc, char **argv) {
    long sleep_time = 1000;
    long count = 0;
    App_arguments(argc, argv, &sleep_time, &count);
    return subscriber_main_w_args(sleep_time, count);
}
