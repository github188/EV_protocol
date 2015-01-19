#include "ev_config.h"




quint16 EV_crcCheck(quint8 *msg,quint8 len)
{
    quint16 i, j, crc = 0, current = 0;
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
