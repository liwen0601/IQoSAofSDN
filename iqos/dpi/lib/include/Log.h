
#ifndef _LOG_H_
#define _LOG_H_
#include <BasicType.h>

extern bool g_PringSwitch;

#define DebugLog(format, ...) if (g_PringSwitch) printf(format, ##__VA_ARGS__)



#endif 
