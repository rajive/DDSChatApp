
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)

message(STATUS  RTIMEHOME = $ENV{RTIMEHOME})

ADD_DEFINITIONS()
LINK_DIRECTORIES()
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
PROJECT(Chat CXX)
SET(SOURCE_EXTENSION cxx)

SET(RTIDDSGEN $ENV{RTIMEHOME}/rtiddsgen/scripts/rtiddsgen)
SET_PROPERTY(GLOBAL PROPERTY RULE_MESSAGES OFF)

SET(IDL_SOURCE_C Chat.idl)    

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/objs/${RTIME_TARGET_NAME})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/objs/${RTIME_TARGET_NAME})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/objs/${RTIME_TARGET_NAME})

SET(RTI_LIB_PREFIX rti_me)
SET(RTI_LIB_SUFFIX)

IF (NOT (DEFINED RTI_SHARED_LIB))
IF (${CMAKE_BUILD_TYPE} MATCHES "[Rr]elease")
        SET(RTI_LIB_SUFFIX "z")
    SET(RTI_DEBUG_BUILD FALSE)
    ADD_DEFINITIONS(-DNDEBUG)
ELSE()
        SET(RTI_LIB_SUFFIX "zd")
        SET(RTI_DEBUG_BUILD TRUE)
    ENDIF()
ELSE()
    IF (${CMAKE_BUILD_TYPE} MATCHES "[Rr]elease")
        SET(RTI_LIB_SUFFIX "")
        SET(RTI_DEBUG_BUILD FALSE)
        ADD_DEFINITIONS(-DNDEBUG)
    ELSE()
        SET(RTI_LIB_SUFFIX "d")
    SET(RTI_DEBUG_BUILD TRUE)
    ENDIF()
ENDIF()

SET(MICRO_C_LIBS rti_me_cpp${RTI_LIB_SUFFIX} 
                 rti_me_rhsm${RTI_LIB_SUFFIX} 
                 rti_me_whsm${RTI_LIB_SUFFIX} 
                 rti_me_discdpse${RTI_LIB_SUFFIX} 
                 rti_me_discdpde${RTI_LIB_SUFFIX} 
                 rti_me${RTI_LIB_SUFFIX}
                                  )

MESSAGE("RTIME_TARGET_NAME = ${RTIME_TARGET_NAME}")
INCLUDE_DIRECTORIES(gen/ $ENV{RTIMEHOME}/include $ENV{RTIMEHOME}/include/rti_me)
LINK_DIRECTORIES($ENV{RTIMEHOME}/lib/${RTIME_TARGET_NAME})

FOREACH(idl ${IDL_SOURCE_C})
    GET_FILENAME_COMPONENT(filename ${idl} NAME)
    STRING(REGEX REPLACE "\\.idl" "" basename ${filename})
    LIST(APPEND IDL_GEN_C ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.${SOURCE_EXTENSION}
                          ${CMAKE_CURRENT_SOURCE_DIR}/${basename}Plugin.${SOURCE_EXTENSION}
                          ${CMAKE_CURRENT_SOURCE_DIR}/${basename}Support.${SOURCE_EXTENSION})
                          
    LIST(APPEND IDL_GEN_H ${CMAKE_CURRENT_SOURCE_DIR}/${basename}.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/${basename}Plugin.h  
                          ${CMAKE_CURRENT_SOURCE_DIR}/${basename}Support.h)
ENDFOREACH()

ADD_EXECUTABLE(Chat_publisher        
               ${CMAKE_CURRENT_SOURCE_DIR}/Chat_publisher.${SOURCE_EXTENSION}
               ${CMAKE_CURRENT_SOURCE_DIR}/ChatApplication.${SOURCE_EXTENSION} 
               ${CMAKE_CURRENT_SOURCE_DIR}/ChatApplication.h 
                                             ${IDL_GEN_C} ${IDL_GEN_H})

ADD_EXECUTABLE(Chat_subscriber 
               ${CMAKE_CURRENT_SOURCE_DIR}/Chat_subscriber.${SOURCE_EXTENSION}
               ${CMAKE_CURRENT_SOURCE_DIR}/ChatApplication.${SOURCE_EXTENSION}
               ${CMAKE_CURRENT_SOURCE_DIR}/ChatApplication.h 
                                             ${IDL_GEN_C} ${IDL_GEN_H})

TARGET_LINK_LIBRARIES(Chat_publisher  ${MICRO_C_LIBS} ${PLATFORM_LIBS})
TARGET_LINK_LIBRARIES(Chat_subscriber ${MICRO_C_LIBS} ${PLATFORM_LIBS})
