/******************************************************************************
#   Copyright 2019 Rajive Joshi, Real-Time Innovations Inc.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#*****************************************************************************/

#ifndef Databus_h
#define Databus_h

#undef NDEBUG
#include <assert.h>

struct Databus {
    DDS_DomainParticipant *participant;
};

extern struct Databus*
Databus_create(const char *participant_name);

extern void
Databus_delete(struct Databus *databus);

extern DDS_ReturnCode_t
Databus_enable(struct Databus *databus);

#endif /* Databus_h */
