/*
 * lb_common.h
 *
 *  Created on: Dec 30, 2019
 *      Author: little
 */
#ifndef LIB_INCLUDE_LB_COMMON_H_
#define LIB_INCLUDE_LB_COMMON_H_
#include "stdint.h"

#include <functional>
#include <fstream>
#include <sstream> 
using namespace std;
extern ofstream g_logfile;




#if 1

#include <iostream>
#include <iomanip>

#define __DEBUG_PRINT(stream, tag, m, v) \
	do { \
		stream << "[" << tag << "][" << __FILE__ << "@" << __LINE__ << "] " << (m) << " " << (v) << std::endl; \
	} while(0)

#define LOG_INFO(m, v)	__DEBUG_PRINT(g_logfile, "INFO", m, v)
#define LOG_ERROR(m, v)	__DEBUG_PRINT(g_logfile, "ERROR", m, v)

#define PRINT_SEPARATOR() LOG_INFO("----------------------------------------------------", "")

#else //!NDEBUG

#define LOG_INFO(m, v)
#define LOG_ERROR(m, v)

#define PRINT_SEPARATOR()

#endif //NDEBUG



#ifdef NDEBUG
#define FOOTPRINT()
#else //!NDEBUG
#define FOOTPRINT()	std::cout << "[INFO][" << __FILE__ << "@" << __LINE__ << "][" << __func__ << "]" << std::endl;
#endif //NDEBUG


#ifndef SAFE_DEL
#define SAFE_DEL(o) if (0 != (o)) { delete (o); (o) = 0; }
#endif //SAFE_DEL_ARRAY


#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(a) if (0 != (a)) { delete[] (a); (a) = 0; }
#endif //SAFE_DEL_ARRAY


#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))
#endif //ARRAY_LENGTH



#define POSIX_CHECK_ERROR(s) \
	if (0 != (s)) \
	{ \
		LOG_ERROR("ErrorNo:", errno); \
	}

#define GL_CHECK_CONDITED(condition, msg) \
	do { \
		if (!(condition)) { \
			LOG_ERROR("[GL ERROR]", msg); \
		} \
	} while(0)



namespace little
{






} // namespace little


#define LOG_FILE "/sdcard/wave4.log"
#endif /* LIB_INCLUDE_LB_COMMON_H_ */
