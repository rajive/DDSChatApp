#******************************************************************************
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
#******************************************************************************
cmake_minimum_required(VERSION 3.14)

project(ChatApp)

set(IDL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../res/types)

# IDL
SET(IDL_SOURCE_C 
    ${IDL_ROOT_DIR}/v2/Chat.idl)

#------------------------------------------------------------------------------

message("RTIMEHOME = ${RTIMEHOME}")
message("RTIME_TARGET_NAME = ${RTIME_TARGET_NAME}")

add_definitions()
link_directories()
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_EXTENSION cxx)

set(RTIDDSGEN ${RTIMEHOME}/rtiddsgen/scripts/rtiddsgen)

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

set(IDL_GEN_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/gen)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/objs/${RTIME_TARGET_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/objs/${RTIME_TARGET_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY 
    ${CMAKE_CURRENT_SOURCE_DIR}/objs/${RTIME_TARGET_NAME})

if (NOT EXISTS gen)
    file(MAKE_DIRECTORY gen)
endif()

set(RTI_LIB_PREFIX rti_me)
set(RTI_LIB_SUFFIX)

if (NOT (DEFINED RTI_SHARED_LIB))
    if (${CMAKE_BUILD_TYPE} MATCHES "[Rr]elease")
        set(RTI_LIB_SUFFIX "z")
        set(RTI_DEBUG_BUILD FALSE)
        add_definitions(-DNDEBUG)
    else()
        set(RTI_LIB_SUFFIX "zd")
        set(RTI_DEBUG_BUILD TRUE)
    endif()
else()
    if (${CMAKE_BUILD_TYPE} MATCHES "[Rr]elease")
        set(RTI_LIB_SUFFIX "")
        set(RTI_DEBUG_BUILD FALSE)
        add_definitions(-DNDEBUG)
    else()
        set(RTI_LIB_SUFFIX "d")
        set(RTI_DEBUG_BUILD TRUE)
    endif()
endif()

set(MICRO_C_LIBS rti_me_cpp${RTI_LIB_SUFFIX} 
                 rti_me_rhsm${RTI_LIB_SUFFIX} 
                 rti_me_whsm${RTI_LIB_SUFFIX} 
                 rti_me_discdpse${RTI_LIB_SUFFIX} 
                 rti_me_discdpde${RTI_LIB_SUFFIX} 
                 rti_me${RTI_LIB_SUFFIX}
)
message("MICRO_C_LIBS = ${MICRO_C_LIBS}")

include_directories(gen/ ${RTIMEHOME}/include ${RTIMEHOME}/include/rti_me)
link_directories(${RTIMEHOME}/lib/${RTIME_TARGET_NAME})

foreach(idl ${IDL_SOURCE_C})
    get_filename_component(basename ${idl} NAME_WLE)
    
    set(idl_gen_c      ${IDL_GEN_ROOT}/${basename}.${SOURCE_EXTENSION}
                       ${IDL_GEN_ROOT}/${basename}Plugin.${SOURCE_EXTENSION}
                       ${IDL_GEN_ROOT}/${basename}Support.${SOURCE_EXTENSION})
    list(APPEND IDL_GEN_C ${idl_gen_c})

    set(idl_gen_h      ${IDL_GEN_ROOT}/${basename}.h
                       ${IDL_GEN_ROOT}/${basename}Plugin.h
                       ${IDL_GEN_ROOT}/${basename}Support.h)
    list(APPEND IDL_GEN_H ${idl_gen_h})
                              
    add_custom_command(OUTPUT ${idl_gen_c} ${idl_gen_h} 
                       COMMAND ${RTIDDSGEN} -verbosity 1 -language C++ 
                               -namespace -micro 
                               -update typefiles 
                               -d ${IDL_GEN_ROOT} ${idl}
                       DEPENDS ${idl}
                       COMMENT "Regeneration type support plugin from ${idl}"
    )
    
    get_filename_component(idl_dir ${idl} DIRECTORY)
    add_custom_target(${basename}_idl ALL
                       COMMAND ${RTIDDSGEN} -verbosity 1 -convertToXml 
                                            -d ${idl_dir} ${idl}
                       COMMAND mv ${idl_dir}/${basename}.xml 
                                  ${idl_dir}/${basename}_idl.xml
                       DEPENDS ${idl}
                       COMMENT "Conversion to XML representation from ${idl}"
    )
endforeach()

#------------------------------------------------------------------------------
# Targets

set(COMMON_C_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/ChatApplication.${SOURCE_EXTENSION} 
) 

add_executable(Chat_publisher
               ${CMAKE_CURRENT_SOURCE_DIR}/Chat_publisher.${SOURCE_EXTENSION}
               ${COMMON_C_SOURCES}
               ${IDL_GEN_C} ${IDL_GEN_H})

add_executable(Chat_subscriber 
               ${CMAKE_CURRENT_SOURCE_DIR}/Chat_subscriber.${SOURCE_EXTENSION}
               ${COMMON_C_SOURCES}
               ${IDL_GEN_C} ${IDL_GEN_H})

target_link_libraries(Chat_publisher  ${MICRO_C_LIBS} ${PLATFORM_LIBS})
target_link_libraries(Chat_subscriber ${MICRO_C_LIBS} ${PLATFORM_LIBS})

#------------------------------------------------------------------------------
