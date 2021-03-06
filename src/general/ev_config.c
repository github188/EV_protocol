#include "ev_config.h"


int EV_createLog()
{
    char date[80] = {0};
    struct timeval	tv ;
    struct tm	stime ;
#if ( defined __linux__ ) || ( defined __unix ) || ( defined _AIX )
    gettimeofday( & tv , NULL );
    localtime_r( &(tv.tv_sec) , & stime );
#elif ( defined _WIN32 )
    {
    SYSTEMTIME	stNow ;
    GetLocalTime( & stNow );
    tv.tv_usec = stNow.wMilliseconds;
    stime.tm_year = stNow.wYear - 1900 ;
    stime.tm_mon = stNow.wMonth - 1 ;
    stime.tm_mday = stNow.wDay ;
    stime.tm_hour = stNow.wHour ;
    stime.tm_min = stNow.wMinute ;
    stime.tm_sec = stNow.wSecond ;
    }
#endif
#ifdef EV_ANDROID
    int len = strftime( date , sizeof(date) - 1 , "/mnt/sdcard/logs/EV_%Y%m%d.log" , &stime ) ;
#else
    int len = strftime( date , sizeof(date) - 1 , "EV_%Y%m%d.log" , &stime ) ;
#endif


    SetLogFile( date , getenv("HOME") );
    SetLogLevel( LOGLEVEL_DEBUG );

}





uint16 EV_crcCheck(uint8 *msg,uint8 len)
{
    uint16 i, j, crc = 0, current = 0;
        for(i=0;i<len;i++) {
            current = msg[i] << 8;
            for(j=0;j<8;j++) {
                if((short)(crc^current)<0)
                    crc = (crc<<1)^0x1021;
                else
                    crc <<= 1;
                current <<= 1;
            }
        }
        return crc;

}
