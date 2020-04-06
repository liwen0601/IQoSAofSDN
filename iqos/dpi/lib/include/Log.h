
#ifndef _LOG_H_
#define _LOG_H_
#include <stdarg.h>
#include <BasicType.h>

#define BUF_SIZE  (1024)

#ifdef _DEBUG_

inline VOID DebugLog(const CHAR* Format, ...)
{
    char Log[BUF_SIZE] = {0};

    va_list ap;
    va_start(ap, Format);
    (VOID)vsnprintf (Log, sizeof(Log)-1, Format, ap);
    va_end(ap);
 
    FILE *fd = fopen("mtr.log", "a");
    if (fd != NULL)
    {
        time_t Now;
        time(&Now);

        CHAR TimeBuf[64];
        sprintf(TimeBuf, "%s", ctime(&Now));
        TimeBuf[strlen(TimeBuf)-1] = 0;
 
        fprintf(fd, "[%s]%s\r\n", TimeBuf, Log);
        fflush(fd);
        fclose(fd);
    }
}

inline VOID DebugPrint(const CHAR* Format, ...)
{
    char Log[BUF_SIZE] = {0};

    va_list ap;
    va_start(ap, Format);
    (VOID)vsnprintf (Log, sizeof(Log)-1, Format, ap);
    va_end(ap);

    time_t Now;
    time(&Now);
 
    CHAR TimeBuf[64];
    sprintf(TimeBuf, "%s", ctime(&Now));
    TimeBuf[strlen(TimeBuf)-1] = 0;
 
    printf("[%s]%s\r\n", TimeBuf, Log);

    return;
}

#else

inline VOID DebugLog(const CHAR* Format, ...)
{
    return;
}

inline VOID DebugPrint(const CHAR* Format, ...)
{
    return;
}

#endif


#endif 
