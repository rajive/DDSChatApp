/*
 * Filters.h
 *
 *  Created on: May 5, 2019
 *      Author: rajive
 */

#ifndef FILTERS_H_
#define FILTERS_H_

#include "MyServiceSupport.h"

/* #define USE_SAMPLE_FILTER */
/* #define FILTER_ON_DESERIALIZE */


extern DDS_Boolean
My_Type_Chat_Obj_on_before_sample_commit(
        void *listener_data,
        DDS_DataReader *reader,
        const void *const sample,
        const struct DDS_SampleInfo *const sample_info,
        DDS_Boolean *dropped);


extern DDS_Boolean
My_Type_Chat_Obj_on_before_sample_deserialize(
        void *listener_data,
        DDS_DataReader *reader,
        struct NDDS_Type_Plugin *plugin,
        struct CDR_Stream_t *stream,
        DDS_Boolean *dropped);

#endif /* FILTERS_H_ */
