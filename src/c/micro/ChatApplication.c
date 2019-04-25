
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"
#include "disc_dpde/disc_dpde_discovery_plugin.h"
#include "ChatApplication.h"
#include "ChatPlugin.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"
#include "netio/netio_udp.h"

void
Application_help(char *appname)
{
    printf("%s [options]\n", appname);
    printf("options:\n");
    printf("-h                 - This text\n");
    printf("-domain <id>       - DomainId (default: 0)\n");
    printf("-udp_intf <intf>   - udp interface (no default)\n");
    printf("-peer <address>    - peer address (no default)\n");
    printf("-count <count>     - count (default -1)\n");
    printf("-sleep <ms>        - sleep between sends (default 1s)\n");
    printf("\n");
}

struct Application *
Application_create(const char *local_participant_name,
const char *remote_participant_name,
DDS_Long domain_id, char *udp_intf, char *peer,
DDS_Long sleep_time, DDS_Long count)
{
    DDS_ReturnCode_t retcode;
    DDS_DomainParticipantFactory *factory = NULL;
    struct DDS_DomainParticipantFactoryQos dpf_qos =
    DDS_DomainParticipantFactoryQos_INITIALIZER;
    struct DDS_DomainParticipantQos dp_qos =
    DDS_DomainParticipantQos_INITIALIZER;
    DDS_Boolean success = DDS_BOOLEAN_FALSE;
    struct Application *application = NULL;
    RT_Registry_T *registry = NULL;
    struct UDP_InterfaceFactoryProperty *udp_property = NULL;

    struct DPDE_DiscoveryPluginProperty discovery_plugin_properties =
    DPDE_DiscoveryPluginProperty_INITIALIZER;

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

    registry =
    DDS_DomainParticipantFactory_get_registry
    (DDS_DomainParticipantFactory_get_instance());

    if (!RT_Registry_register(registry, DDSHST_WRITER_DEFAULT_HISTORY_NAME,
    WHSM_HistoryFactory_get_interface(), NULL, NULL))
    {
        printf("failed to register wh\n");
        goto done;
    }

    if (!RT_Registry_register(registry, DDSHST_READER_DEFAULT_HISTORY_NAME,
    RHSM_HistoryFactory_get_interface(), NULL, NULL))
    {
        printf("failed to register rh\n");
        goto done;
    }

    /* Configure UDP transport's allowed interfaces */
    if (!RT_Registry_unregister(registry, NETIO_DEFAULT_UDP_NAME, NULL, NULL))
    {
        printf("failed to unregister udp\n");
        goto done;
    }

    udp_property = (struct UDP_InterfaceFactoryProperty *)
    malloc(sizeof(struct UDP_InterfaceFactoryProperty));
    if (udp_property == NULL)
    {
        printf("failed to allocate udp properties\n");
        goto done;
    }
    *udp_property = UDP_INTERFACE_FACTORY_PROPERTY_DEFAULT;

    /* For additional allowed interface(s), increase maximum and length, and
    set interface below:
    */
    if (!REDA_StringSeq_set_maximum(&udp_property->allow_interface,2))
    {
        printf("failed to set allow_interface maximum\n");
        goto done;
    }
    if (!REDA_StringSeq_set_length(&udp_property->allow_interface,2))
    {
        printf("failed to set allow_interface length\n");
        goto done;
    }

    /* loopback interface */
    #if defined(RTI_DARWIN)
    *REDA_StringSeq_get_reference(&udp_property->allow_interface,0) = 
    DDS_String_dup("lo0");
    #elif defined (RTI_LINUX)
    *REDA_StringSeq_get_reference(&udp_property->allow_interface,0) = 
    DDS_String_dup("lo");
    #elif defined (RTI_VXWORKS)
    *REDA_StringSeq_get_reference(&udp_property->allow_interface,0) = 
    DDS_String_dup("lo0");
    #elif defined(RTI_WIN32)
    *REDA_StringSeq_get_reference(&udp_property->allow_interface,0) = 
    DDS_String_dup("Loopback Pseudo-Interface 1");
    #else
    *REDA_StringSeq_get_reference(&udp_property->allow_interface,0) = 
    DDS_String_dup("lo");
    #endif

    if (udp_intf != NULL)
    { /* use interface supplied on command line */
        *REDA_StringSeq_get_reference(&udp_property->allow_interface,1) = 
        DDS_String_dup(udp_intf);
    } 
    else                /* use hardcoded interface */
    {
        #if defined(RTI_DARWIN)
        *REDA_StringSeq_get_reference(&udp_property->allow_interface,1) = 
        DDS_String_dup("en1");
        #elif defined (RTI_LINUX)
        *REDA_StringSeq_get_reference(&udp_property->allow_interface,1) = 
        DDS_String_dup("eth0");
        #elif defined (RTI_VXWORKS)
        *REDA_StringSeq_get_reference(&udp_property->allow_interface,1) = 
        DDS_String_dup("geisc0");
        #elif defined(RTI_WIN32)
        *REDA_StringSeq_get_reference(&udp_property->allow_interface,1) = 
        DDS_String_dup("Local Area Connection");
        #else
        *REDA_StringSeq_get_reference(&udp_property->allow_interface,1) = 
        DDS_String_dup("ce0");
        #endif
    }

    if (!RT_Registry_register(registry, NETIO_DEFAULT_UDP_NAME,
    UDP_InterfaceFactory_get_interface(),
    (struct RT_ComponentFactoryProperty*)udp_property, NULL))
    {
        printf("failed to register udp\n");
        goto done;
    }

    DDS_DomainParticipantFactory_get_qos(factory, &dpf_qos);
    dpf_qos.entity_factory.autoenable_created_entities = DDS_BOOLEAN_FALSE;
    DDS_DomainParticipantFactory_set_qos(factory, &dpf_qos);

    if (peer == NULL)
    {
        peer = "127.0.0.1"; /* default to loopback */
    }

    if (!RT_Registry_register(registry,
    "dpde",
    DPDE_DiscoveryFactory_get_interface(),
    &discovery_plugin_properties._parent, 
    NULL))
    {
        printf("failed to register dpde\n");
        goto done;
    }

    if (!RT_ComponentFactoryId_set_name(&dp_qos.discovery.discovery.name,"dpde"))
    {
        printf("failed to set discovery plugin name\n");
        goto done;
    }
    if (!DDS_StringSeq_set_maximum(&dp_qos.discovery.initial_peers,1))
    {
        printf("failed to set initial peers maximum\n");
        goto done;
    }
    if (!DDS_StringSeq_set_length(&dp_qos.discovery.initial_peers,1))
    {
        printf("failed to set initial peers length\n");
        goto done;
    }
    *DDS_StringSeq_get_reference(&dp_qos.discovery.initial_peers,0) = DDS_String_dup(peer);

    /* if there are more remote or local endpoints, you need to increase these limits */
    dp_qos.resource_limits.max_destination_ports = 32;
    dp_qos.resource_limits.max_receive_ports = 32;
    dp_qos.resource_limits.local_topic_allocation = 1;
    dp_qos.resource_limits.local_type_allocation = 1;
    dp_qos.resource_limits.local_reader_allocation = 1;
    dp_qos.resource_limits.local_writer_allocation = 1;
    dp_qos.resource_limits.remote_participant_allocation = 8 + 2 /* Admin Console */;
    dp_qos.resource_limits.remote_reader_allocation = 8 + 24 /* Admin Console */;
    dp_qos.resource_limits.remote_writer_allocation = 8 + 24 /* Admin Console */;
    dp_qos.resource_limits.matching_writer_reader_pair_allocation = 16 + 48 /* Admin Console */;

    /* >>> Set Participant Name <<< */
    snprintf(dp_qos.participant_name.name, DDS_ENTITYNAME_QOS_NAME_MAX,
    		"Rajive : %s (micro C)", local_participant_name);

    application->participant =
    DDS_DomainParticipantFactory_create_participant(factory, domain_id,
    &dp_qos, NULL,
    DDS_STATUS_MASK_NONE);

    if (application->participant == NULL)
    {
        printf("failed to create participant\n");
        goto done;
    }

    sprintf(application->type_name, "My::ChatObject");
    retcode = DDS_DomainParticipant_register_type(application->participant,
    application->type_name,
    My_ChatObjectTypePlugin_get());
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to register type: %s\n", "test_type");
        goto done;
    }

    sprintf(application->topic_name, My_name_CHAT_TOPIC);
    application->topic =
    DDS_DomainParticipant_create_topic(application->participant,
    application->topic_name,
    application->type_name,
    &DDS_TOPIC_QOS_DEFAULT, NULL,
    DDS_STATUS_MASK_NONE);

    if (application->topic == NULL)
    {
        printf("topic == NULL\n");
        goto done;
    }

    success = DDS_BOOLEAN_TRUE;

    done:
    DDS_DomainParticipantQos_finalize(&dp_qos);

    if (!success)
    {
        if (udp_property != NULL)
        {
            UDP_InterfaceFactoryProperty_finalize(udp_property);
            free(udp_property);
        }
        free(application);
        application = NULL;
    }

    return application;
}

DDS_ReturnCode_t
Application_enable(struct Application * application)
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
    struct UDP_InterfaceFactoryProperty *udp_property = NULL;

    retcode = DDS_DomainParticipant_delete_contained_entities(application->participant);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to delete conteined entities (retcode=%d)\n",retcode);
    }

    retcode =
    DDS_DomainParticipantFactory_delete_participant
    (DDS_DomainParticipantFactory_get_instance(), application->participant);
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to delete participant: %d\n", retcode);
        return;
    }

    registry = DDS_DomainParticipantFactory_get_registry
    (DDS_DomainParticipantFactory_get_instance());

    if (!RT_Registry_unregister(registry, NETIO_DEFAULT_UDP_NAME,
    (struct RT_ComponentFactoryProperty**)&udp_property, 
    NULL))
    {
        printf("failed to unregister udp\n");
        return;
    }
    if (udp_property != NULL)
    {
        UDP_InterfaceFactoryProperty_finalize(udp_property);
        free(udp_property);
        udp_property = NULL;
    }
    if (!RT_Registry_unregister(registry, "dpde", NULL, NULL))
    {
        printf("failed to unregister dpde\n");
        return;
    }
    if (!RT_Registry_unregister(registry, DDSHST_READER_DEFAULT_HISTORY_NAME, NULL, NULL))
    {
        printf("failed to unregister rh\n");
        return;
    }
    if (!RT_Registry_unregister(registry, DDSHST_WRITER_DEFAULT_HISTORY_NAME, NULL, NULL))
    {
        printf("failed to unregister wh\n");
        return;
    }

    free(application);

    retcode = DDS_DomainParticipantFactory_finalize_instance();
    if (retcode != DDS_RETCODE_OK)
    {
        printf("failed to finalize instance %d\n", retcode);
        return;
    }
}
