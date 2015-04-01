#ifndef _EV_BENTO_H_
#define _EV_BENTO_H_
#include "ev_config.h"
#include "EVprotocol.h"


#define EV_BENTO_HEAD   0xC7

#define EV_BENTO_TYPE_OPEN 			0x52
#define EV_BENTO_TYPE_CHECK 		0x51
#define EV_BENTO_TYPE_LIGHT			0x56
#define EV_BENTO_TYPE_HOT 			0x53
#define EV_BENTO_TYPE_COOL 			0x55




#define EV_BENTO_TYPE_OPEN_ACK 			0x62
#define EV_BENTO_TYPE_CHECK_ACK 		0x61
#define EV_BENTO_TYPE_LIGHT_ACK			0x66
#define EV_BENTO_TYPE_HOT_ACK 			0x63
#define EV_BENTO_TYPE_COOL_ACK 			0x65


int EV_bento_open(int cabinet,int box);
int EV_bento_openSerial(char *portName,int baud,int databits,char parity,int stopbits)
;
int EV_bento_closeSerial();
int EV_bento_check(int cabinet,ST_COLUMN_RPT *st_bento);
int EV_bento_light(int cabinet,uint8 flag);
#endif
