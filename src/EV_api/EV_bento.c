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

int EV_bento_openSerial(char *portName,int baud,int databits,char parity,int stopbits)
{
    Y_FD fd = yserial_open(portName);
    if (yserial_fdIsNull(fd)){
            EV_LOGE("EV_openSerialPort:open %s failed\n",portName);
			return -1;
	}
    yserial_setParity(fd,parity);
    yserial_setBaudRate(fd,baud);
    yserial_setDataBits(fd,databits);
    yserial_setStopBits(fd,stopbits);
    yserial_setTimeout(fd,10);
    yserial_clear(fd);
	bento_fd = fd;
    EV_LOGI("EV_openSerialPort:Serial[%s] open suc\n",portName);
    return 1;
}

int EV_bento_closeSerial()
{
    EV_LOGI("EV_closeSerialPort:closed...\n");
    yserial_close(bento_fd);

    return 1;
}


uint8 EV_bento_recv(uint8 *rdata,uint8 *rlen)
{
    uint8 timeout = 100,buf[10]= {0},len = 0,temp,startFlag = 0;
    uint16 crc;
	*rlen = 0;
	while(timeout--)
	{
        if(yserial_read(bento_fd,(char *)&temp,1) > 0)
		{
			if(temp == EV_BENTO_HEAD + 1)
			{
				startFlag = 1;
				
			}
			if(startFlag == 1)
			{
				buf[len++] = temp;
				if(len >= (buf[1] + 2))
				{
					crc = EV_crcCheck(buf,6);
					if(crc == INTEG16(buf[6],buf[7]))
					{
						if(rdata != NULL)
							memcpy(rdata,buf,8);
						*rlen = 8;
						return 1;
					}
					else
						return 0;
					
				}
			}
			else
				EV_msleep(2);
		}
		else
			EV_msleep(2);
	}
	return 0;
	
}


int EV_bento_send(uint8 cmd,uint8 cabinet,uint8 arg,uint8 *data)
{
    uint8 buf[20] = {0},len = 0,ret,rbuf[20] = {0};
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
	int ret = 0;
	if(cabinet <= 0 || box <= 0)
		return 0;
	
	ret = EV_bento_send(EV_BENTO_TYPE_OPEN,cabinet&0xFF,box &0xFF,NULL);
	return ret;
}


int EV_bento_light(int cabinet,uint8 flag)
{
	int ret = 0;
	if(cabinet <= 0)
		return 0;
	ret = EV_bento_send(EV_BENTO_TYPE_LIGHT,cabinet&0xFF,flag &0xFF,NULL);
	return ret;
}


int EV_bento_check(int cabinet,ST_BENTO_FEATURE *st_bento)
{
	int ret = 0;
    uint8 buf[20] = {0},i;
	if(st_bento == NULL) return 0;
	if(cabinet <= 0)
		return 0;

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
		return 1;
		
	}
	return ret;
}









