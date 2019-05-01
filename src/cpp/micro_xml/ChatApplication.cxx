#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_cpp.hxx"

#include "ChatApplication.h"
#include "ChatPlugin.h"

#include "app_gen/app_gen.h"
#include "app_gen/app_gen_plugin.h"
#include "dds_c/dds_c_profile.h"
#include "dds_c/dds_c_profile_plugin.h"

#include "ChatAppgen.h"

void
Application::help(const char *appname)
{
    printf("%s [options]\n", appname);
    printf("options:\n");
    printf("-h                 - This text\n");
    printf("-count <count>     - count (default -1)\n");
    printf("-sleep <ms>        - sleep between sends (default 1s)\n");
    printf("\n");
}

DDS_ReturnCode_t
Application::initialize(const char *participant_name,
                        DDS_Long sleep_time,
                        DDS_Long count)
{
    DDSDomainParticipantFactory *factory = NULL;
    DDS_Boolean success = DDS_BOOLEAN_FALSE;
    RTRegistry *registry = NULL;
    struct APPGEN_FactoryProperty model_xml = APPGEN_FactoryProperty_INITIALIZER;

    /* Uncomment to increase verbosity level:
       OSAPI_Log_set_verbosity(OSAPI_LOG_VERBOSITY_WARNING);
    */

    this->sleep_time = sleep_time;
    this->count = count;

    factory = DDSDomainParticipantFactory::get_instance();

    registry = factory->get_registry();

    /* This pointer must be valid as long as the Micro
     * Application Generation plugin is registered
     */
    model_xml._model = APPGEN_get_library_seq();
    if (!registry->register_component(
                              PROFILE_DEFAULT_APPGEN_NAME,
                              APPGEN_Factory_get_interface(),
                              &model_xml._parent, NULL))
    {
        printf("failed to register Application Generation\n");
        goto done;
    }

    this->participant = factory->create_participant_from_config(participant_name);
    if (this->participant == NULL)
    {
        printf("failed to create participant: %s\n", participant_name);
        goto done;
    }

    success = DDS_BOOLEAN_TRUE;

  done:

    return (success ? DDS_RETCODE_OK : DDS_RETCODE_ERROR);
}

DDS_ReturnCode_t
Application::enable()
{
    DDS_ReturnCode_t retcode;

    retcode = this->participant->enable();
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to enable entity\n");
    }

    return retcode;
}

Application::Application()
{
    this->participant = NULL;
    this->sleep_time = 1000;
    this->count = 0;
}


Application::~Application()
{
    DDS_ReturnCode_t retcode;
    RTRegistry *registry = NULL;
    DDSDomainParticipantFactory *factory = NULL;

    factory = DDSDomainParticipantFactory::get_instance();
    registry = factory->get_registry();

    if (this->participant != NULL)
    {
        retcode = participant->delete_contained_entities();
        if (retcode != DDS_RETCODE_OK)
        {
            printf("failed to delete contained entities\n");
        }

        retcode = factory->delete_participant(this->participant);
        if (retcode != DDS_RETCODE_OK)
        {
            printf("failed to delete participant\n");
            return;
        }
    }

    if (!registry->unregister(PROFILE_DEFAULT_APPGEN_NAME, NULL, NULL))
    {
        printf("failed to unregister application generation factory\n");
        return;
    }

    retcode = DDSTheParticipantFactory->finalize_instance();
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to finalize instance (retcode=%d)\n",retcode);
        return;
    }
}
