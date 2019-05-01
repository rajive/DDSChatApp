
#ifndef Application_h
#define Application_h

#include "rti_me_cpp.hxx"

using namespace DDS;

class Application {
public:
    DDSDomainParticipant *participant;
    DDS_Long sleep_time;
    DDS_Long count;

    static void help(const char *appname);

    DDS_ReturnCode_t initialize(const char *participant_name,
                                DDS_Long sleep_time,
                                DDS_Long count);

    DDS_ReturnCode_t enable();

    Application();
    ~Application();
};

#endif
