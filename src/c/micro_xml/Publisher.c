#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"

#include "MyServiceSupport.h"

#include "Databus.h"
#include "App.h"

struct OutputContext {
    My_Type_Chat_Obj *Chat_Obj_sample;
    My_Type_Chat_ObjDataWriter *Chat_Obj_writer;
};

void My_Topic_Chat_output(void* context, long count) {

    struct OutputContext *output_context = (struct OutputContext*)context;
    DDS_ReturnCode_t retcode = DDS_RETCODE_OK;

    snprintf(output_context->Chat_Obj_sample->content,
                        My_Type_Chat_CONTENT_STR_SIZE,
                        "XML Micro C Hello World %ld", count);
    printf("%s %s\n", output_context->Chat_Obj_sample->id, output_context->Chat_Obj_sample->content);

    retcode = My_Type_Chat_ObjDataWriter_write(output_context->Chat_Obj_writer,
                                               output_context->Chat_Obj_sample, &DDS_HANDLE_NIL);
    switch (retcode) {
    case DDS_RETCODE_OK: break;
    default: fprintf(stderr, "failed to write\n"); break;
    };
}

void
My_Publisher_on_publication_matched(
        void *listener_data,
        DDS_DataWriter * writer,
        const struct DDS_PublicationMatchedStatus *status) {
    if (status->current_count_change > 0) {
        printf("Matched a subscriber\n");
    }
    else if (status->current_count_change < 0) {
        printf("Unmatched a subscriber\n");
    }
}

int
publisher_main_w_args(long sleep_time, long count) {
    struct Databus *databus = NULL;
    DDS_DataWriter *datawriter = NULL;

    My_Type_Chat_Obj *Chat_Obj_sample = NULL;
    My_Type_Chat_ObjDataWriter *Chat_Obj_writer = NULL;

    struct DDS_DataWriterListener dw_listener = DDS_DataWriterListener_INITIALIZER;
    dw_listener.on_publication_matched        = My_Publisher_on_publication_matched;


    assert((databus = Databus_create(My_If_PUB))
            != NULL
    );
    assert((datawriter = DDS_DomainParticipant_lookup_datawriter_by_name(
            databus->participant,
            My_Topic_Chat_WRITER))
            != NULL
    );
    assert((Chat_Obj_writer = My_Type_Chat_ObjDataWriter_narrow(datawriter))
            != NULL
    );
    assert(DDS_DataWriter_set_listener(datawriter, &dw_listener,
            DDS_PUBLICATION_MATCHED_STATUS)
            == DDS_RETCODE_OK
    );
    assert(Databus_enable(databus)
            == DDS_RETCODE_OK
    );

    assert((Chat_Obj_sample = My_Type_Chat_Obj_create())
            != NULL
    );
    strncpy(Chat_Obj_sample->id, "Rajive (xml micro C)", My_Type_Chat_ID_STR_SIZE);


    struct OutputContext output_context = {Chat_Obj_sample, Chat_Obj_writer};
    App_loop(sleep_time, count, My_Topic_Chat_output, &output_context);

done:
    Databus_delete(databus);

    if (Chat_Obj_sample != NULL) {
        My_Type_Chat_Obj_delete(Chat_Obj_sample);
    }

    return 0;
}

int
main(int argc, char **argv) {
    long sleep_time = 1000;
    long count = 0;
    App_arguments(argc, argv, &sleep_time, &count);
    return publisher_main_w_args(sleep_time, count);
}
