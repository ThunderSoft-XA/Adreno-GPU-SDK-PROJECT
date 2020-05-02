/*
 * lb_types.cpp
 *
 *  Created on: Dec 31, 2019
 *      Author: little
 */
#include <sys/time.h>

#include "../include/lb_types.h"






namespace little
{



uint64_t GetRawTime() //Microseconds
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (uint64_t) tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec;
}

double GetTime() //Seconds
{
	static uint64_t	base_time = 0;

	if ( 0 == base_time )
	{
		FOOTPRINT();

		base_time = GetRawTime();
	}

	return (double)(GetRawTime() - base_time) * 1e-6; // Microsecond
}



} // namespace little


