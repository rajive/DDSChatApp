#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"
#include "rtps/rtps_rtps.h"
#include "dds_c/dds_c_infrastructure.h"
#include "dds_c/dds_c_type.h"
#include "disc_dpde/disc_dpde_discovery_plugin.h"
#include "ChatApplication.h"
#include "ChatPlugin.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"
#include "netio/netio_udp.h"

#include "app_gen/app_gen.h"
#include "app_gen/app_gen_plugin.h"
#include "dds_c/dds_c_profile.h"
#include "dds_c/dds_c_profile_plugin.h"

#include "ChatAppgen.h"

void
Application_help(char *appname)
{
    printf("%s [options]\n", appname);
    printf("options:\n");
    printf("-h                 - This text\n");
    printf("-count <count>     - count (default -1)\n");
    printf("-sleep <ms>        - sleep between sends (default 1s)\n");
    printf("\n");
}

struct Application *
Application_create(const char *participant_name,
                   DDS_Long sleep_time, DDS_Long count)
{
    DDS_DomainParticipantFactory *factory = NULL;
    DDS_Boolean success = DDS_BOOLEAN_FALSE;
    struct Application *application = NULL;
    RT_Registry_T *registry = NULL;
    struct APPGEN_FactoryProperty model_xml = APPGEN_FactoryProperty_INITIALIZER;

    /* Uncomment to increase verbosity level:
       OSAPI_Log_set_verbosity(OSAPI_LOG_VERBOSITY_WARNING);
     */

    application = (struct Application *)malloc(sizeof(struct Application));
    if (application == NULL)
    {
        printf("failed to allocate application\n");
        goto done;
    }

    application->sleep_time = sleep_time;
    application->count = count;

    factory = DDS_DomainParticipantFactory_get_instance();

    registry = DDS_DomainParticipantFactory_get_registry(factory);

    /* This pointer must be valid as long as the Micro
     * Application Generation plugin is registered
     */
    model_xml._model = APPGEN_get_library_seq();
    if (!APPGEN_Factory_register(registry, &model_xml))
    {
        printf("failed to register Application Generation\n");
        goto done;
    }

    application->participant =
        DDS_DomainParticipantFactory_create_participant_from_config(factory,
                                                             participant_name);

    if (application->participant == NULL)
    {
        printf("failed to create participant: %s\n", participant_name);
        goto done;
    }

    success = DDS_BOOLEAN_TRUE;

  done:

    if (!success)
    {
        if (application != NULL)
        {
            free(application);
        }
        application = NULL;
    }

    return application;
}

DDS_ReturnCode_t
Application_enable(struct Application *application)
{
    DDS_Entity *entity;
    DDS_ReturnCode_t retcode;

    entity = DDS_DomainParticipant_as_entity(application->participant);

    retcode = DDS_Entity_enable(entity);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to enable entity\n");
    }

    return retcode;
}

void
Application_delete(struct Application *application)
{
    DDS_ReturnCode_t retcode;
    RT_Registry_T *registry = NULL;
    DDS_DomainParticipantFactory *factory = NULL;

    if (application == NULL) { return; }

    factory = DDS_DomainParticipantFactory_get_instance();
    registry = DDS_DomainParticipantFactory_get_registry(factory);

    retcode = DDS_DomainParticipant_delete_contained_entities(
                  application->participant);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to delete contained entities: %d\n",retcode);
    }

    retcode = DDS_DomainParticipantFactory_delete_participant(
                  factory, application->participant);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to delete participant: %d\n", retcode);
        return;
    }

    if (!APPGEN_Factory_unregister(registry, NULL))
    {
        printf("failed to unregister Application Generation\n");
    }

    free(application);

    retcode = DDS_DomainParticipantFactory_finalize_instance();
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to finalize domain participant factory: %d\n", retcode);
        return;
    }
}
