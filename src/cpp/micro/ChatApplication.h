
#ifndef Application_h
#define Application_h

#ifndef rti_me_cpp_hxx
#include "rti_me_cpp.hxx"
#endif

using namespace DDS;

/*e \dref_Example_Config_UseReliableQos
* Define USE_NON_VOLATILE_QOS to use reliable
* and transient local policies on the DataReader;
* otherwise, use best-effort reliability
* by default.
*/

#define USE_NON_VOLATILE_QOS

class Application
{
  public:
    DomainParticipant *participant; 
    Topic *topic; 
    char topic_name[255];
    char type_name[255];
    Long sleep_time;
    Long count;

    static void help(char *appname);

    ReturnCode_t initialize(const char *local_participant_name,
    const char *remote_participant_name,
    Long domain_id, 
    char *udp_intf, 
    char *peer, 
    Long sleep_time, 
    Long count);

    ReturnCode_t enable();

    Application();
    ~Application();
};

#endif
