#include <stdio.h>

#include "rti_me_c.h"
#include "app_gen/app_gen_plugin.h"
#include "dds_c/dds_c_profile.h"

#include "Databus.h"


struct Databus *
Databus_create(const char *participant_name) {

    struct Databus *databus = NULL;
    DDS_DomainParticipantFactory *factory = NULL;
    RT_Registry_T *registry = NULL;
    DDS_DomainParticipant *participant = NULL;

    struct APPGEN_FactoryProperty model_xml = APPGEN_FactoryProperty_INITIALIZER;
    model_xml._model = APPGEN_get_library_seq();

    assert((factory = DDS_DomainParticipantFactory_get_instance())
            != NULL
    );
    assert((registry = DDS_DomainParticipantFactory_get_registry(factory))
            != NULL
    );
    assert(APPGEN_Factory_register(registry, &model_xml)); /* register the MAG/Appgen code */
    assert((participant =
            DDS_DomainParticipantFactory_create_participant_from_config(factory,participant_name))
            != NULL
    );
    assert((databus = (struct Databus *)malloc(sizeof(struct Databus)))
            != NULL
    );

    if (databus != NULL) {
        databus->participant = participant;
    }
    return databus;
}

void
Databus_delete(struct Databus *databus) {

    DDS_ReturnCode_t retcode;
    RT_Registry_T *registry = NULL;
    DDS_DomainParticipantFactory *factory = NULL;

    if (databus == NULL || databus->participant == NULL) { return; }

    assert(DDS_DomainParticipant_delete_contained_entities(
            databus->participant)
            == DDS_RETCODE_OK
    );

    assert((factory = DDS_DomainParticipantFactory_get_instance())
            != NULL
    );
    assert(DDS_DomainParticipantFactory_delete_participant(factory, databus->participant)
            == DDS_RETCODE_OK
    );
    databus->participant = NULL;
    free(databus);


    assert((registry = DDS_DomainParticipantFactory_get_registry(factory))
            != NULL
    );
    assert(APPGEN_Factory_unregister(registry, NULL));

    assert(DDS_DomainParticipantFactory_finalize_instance()
            == DDS_RETCODE_OK
    );
}


DDS_ReturnCode_t
Databus_enable(struct Databus *databus) {

    DDS_Entity *entity = DDS_DomainParticipant_as_entity(databus->participant);
    DDS_ReturnCode_t retcode = DDS_Entity_enable(entity);

    switch (retcode) {
    case DDS_RETCODE_OK: break;
    default: fprintf(stderr, "failed to enable participant\n"); break;
    };

    return retcode;
}

