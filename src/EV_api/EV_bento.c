#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h> //定时器
#include <string.h>
#include <fcntl.h>
#include "EV_bento.h"
#include "yoc_serialport.h"
#include "timer.h"
#include "ev_config.h"

static Y_FD bento_fd;


static int EV_getCh(char *ch)
{
    uint8 i = 0;
#ifdef EV_WIN32
    i = yserial_read(bento_fd,ch,1);
#else
    if(yserial_bytesAvailable(bento_fd) > 0)
    {
        i = yserial_read(bento_fd,ch,1);
    }
#endif
     return i;
}


int EV_bento_openSerial(char *portName,int baud,int databits,char parity,int stopbits)
{
    EV_createLog();
    Y_FD fd = yserial_open(portName);
    if (yserial_fdIsNull(fd)){
            EV_LOGE("EV_bento_openSerial:open %s failed\n",portName);
			return -1;
	}
    yserial_setParity(fd,parity);
    yserial_setBaudRate(fd,baud);
    yserial_setDataBits(fd,databits);
    yserial_setStopBits(fd,stopbits);
    yserial_setTimeout(fd,10);
    yserial_clear(fd);
	bento_fd = fd;
    EV_LOGI("EV_bento_openSerial:Serial[%s] open suc\n",portName);
    return 1;
}

int EV_bento_closeSerial()
{
    EV_LOGI("EV_bento_closeSerial:closed...\n");
    yserial_close(bento_fd);

    return 1;
}


uint8 EV_bento_recv(uint8 *rdata,uint8 *rlen)
{
    uint8 timeout = 100,buf[80]= {0},index = 0,len = 0,temp;
    uint16 crc;
	*rlen = 0;
    if(rdata == NULL)
        return 0;
    while(timeout)
	{ 
        if(EV_getCh((char *)&temp) > 0)//有数据接收
		{
            buf[index++] = temp;
            if(index == 1){
                if(temp != EV_BENTO_HEAD + 1)
                    index = 0;
            }
            else if(index == 2){
                len = temp;
            }
            else if(index >= (len + 2)){
                crc = EV_crcCheck(buf,len);
                if(crc == INTEG16(buf[len],buf[len + 1]))
                {
                     memcpy(rdata,buf,len + 2);
                    *rlen = len + 2;
                     return 1;
                }
                else
                    return 0;
            }
		}
        else{
            EV_msleep(5);
            timeout--;
        }
	}
	return 0;
	
}


int EV_bento_send(uint8 cmd,uint8 cabinet,uint8 arg,uint8 *data)
{
    uint8 buf[20] = {0},len = 0,ret,rbuf[80] = {0};
    uint16 crc;
	buf[len++] = EV_BENTO_HEAD;
	buf[len++] = 0x07;
	buf[len++] = cabinet - 1;
	buf[len++] = cmd;
	buf[len++] = cabinet - 1;
	buf[len++] = cabinet - 1;//0x08;
	buf[len++] = arg;//0x00;	
	crc = EV_crcCheck(buf,len);
	buf[len++] = HUINT16(crc);
	buf[len++] = LUINT16(crc);
    yserial_clear(bento_fd);
    yserial_write(bento_fd,(const char *)buf,len);
	
	ret = EV_bento_recv(rbuf,&len);
	
	if(ret == 1)
	{
		if(cmd == EV_BENTO_TYPE_OPEN)
		{
			if(rbuf[3] == EV_BENTO_TYPE_OPEN_ACK)
			{
				return 1;//开门成功
			}
		}
		else if(cmd == EV_BENTO_TYPE_CHECK)
		{
			if(rbuf[3] == EV_BENTO_TYPE_CHECK_ACK)
			{
				if(data != NULL)
					memcpy(data,rbuf,rbuf[1]);
				return 1;
			}
		}
		else if(cmd == EV_BENTO_TYPE_LIGHT)
		{
			if(rbuf[3] == EV_BENTO_TYPE_LIGHT_ACK)
				return 1;
			else
				return 0;
		}
	}
	return 0;
	
}


int EV_bento_open(int cabinet,int box)
{
    int ret;
	if(cabinet <= 0 || box <= 0)
		return 0;	
    ret = EV_bento_send(EV_BENTO_TYPE_OPEN,cabinet&0xFF,box &0xFF,NULL);

    EV_LOGD("EV_bento_open: cabinet = %d,box = %d ret = %d",cabinet,box,ret);
    return ret;
}


int EV_bento_light(int cabinet,uint8 flag)
{
	int ret = 0;
	if(cabinet <= 0)
		return 0;
    ret = EV_bento_send(EV_BENTO_TYPE_LIGHT,cabinet,flag,NULL);
    EV_LOGD("EV_bento_light: cabinet=%d flag=%d ret =%d\n",cabinet,flag,ret);
	return ret;
}


int  EV_bento_check(int cabinet,ST_BENTO_FEATURE *st_bento)
{
	int ret = 0;
    uint8 buf[20] = {0},i;
    if(st_bento == NULL) {
        EV_LOGW("EV_bento_check:st_bento == null\n");
        return 0;
    }

    if(cabinet <= 0){
        EV_LOGW("EV_bento_check:cabinet = %d\n",cabinet);
        return 0;

    }

	ret = EV_bento_send(EV_BENTO_TYPE_CHECK,cabinet&0xFF,0x00,buf);
	if(ret == 1)
	{
		st_bento->boxNum = buf[6];
		st_bento->ishot = (buf[8] & 0x01);
		st_bento->iscool = ((buf[8] >> 1) & 0x01);
		st_bento->islight = ((buf[8] >> 2) & 0x01);
		for(i = 0;i < 7;i++)
		{
			st_bento->id[i] = buf[9 + i];
		}
        EV_LOGD("EV_bento_check:num=%d islight=%d id=%s\n",st_bento->boxNum,
                st_bento->islight,st_bento->id);
		return 1;
		
	}
    EV_LOGD("EV_bento_check: cabinet = %d Fail!!!\n",cabinet);
    return 0;
}









