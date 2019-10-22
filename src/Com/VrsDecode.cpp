/*
 * VrsDecode.cpp
 *
 *  Created on: 2019/5/21
 *      Author: jtao
 */
#include "../../include/Com/VrsDecode.h"
#include "../../include/Com/Com.h"
using namespace std;
using namespace bamboo;
VrsDecode* VrsDecode::m_Instance = NULL;
int VrsDecode::enbase64(char *str, const unsigned char *byte, int n){
    const char table[]=
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i,j,k,b;

    tracet(4,"encbase64: n=%d\n",n);

    for (i=j=0;i/8<n;) {
        for (k=b=0;k<6;k++,i++) {
            b<<=1; if (i/8<n) b|=(byte[i/8]>>(7-i%8))&0x1;
        }
        str[j++]=table[b];
    }
    while (j&0x3) str[j++]='=';
    str[j]='\0';
    tracet(5,"encbase64: str=%s\n",str);
    return j;
}

int VrsDecode::IOEncode100(VrsReq* req){
	int i = 32;
	char buf[1024];
	enbase64(buf,(unsigned char*)req->usrpsd,strlen(req->usrpsd));
	memcpy(req->buff + 4,buf,strlen(buf));
	i += strlen(buf) * 8;
	req->nbit = i;
	return 1;
}
int VrsDecode::IOEncode101(VrsReq* req){
	int i = 32,week;
	double sow;
	char reqMsg[512];
	mjd2wksow(req->mjd,req->sod,&week,&sow);
	sprintf(reqMsg,"%d,%d,%lf,%12.8lf,%12.8lf,%7.2lf",req->id,week,sow,req->geod[1] * RAD2DEG,req->geod[0] * RAD2DEG,req->geod[2]);
	memcpy(req->buff + 4,reqMsg,strlen(reqMsg));
	i += strlen(reqMsg) * 8;
	req->nbit = i;
	return 1;
}
int VrsDecode::IOEncode102(VrsReq* req){
	int i = 32;
	const char* msg = "Unauthorized client\r\n";
	memcpy(req->buff + 4,msg,strlen(msg));
	i += strlen(msg) * 8;
	req->nbit = i;
	return 1;
}
int VrsDecode::IOEncode103(VrsReq* req){
	int i = 32;
	const char* msg = "ICY OK\r\n";
	memcpy(req->buff + 4,msg,strlen(msg));
	i += strlen(msg) * 8;
	req->nbit = i;
	return 1;
}
int VrsDecode::IOEncodeMsg(VrsReq* req,int type){
	int ret = 0;
	setbitu((unsigned char*)req->buff,24,8,type);
	switch(type){
	case 100:
		ret = IOEncode100(req);
		break;
	case 101:
		ret = IOEncode101(req);
		break;
	case 102:
		ret = IOEncode102(req);
		break;
	case 103:
		ret = IOEncode103(req);
		break;
	}
	return ret;
}
int VrsDecode::IOEncode(VrsReq* req,int type){
    unsigned int crc;
	int i = 0;
	req->nbyte = 0;
	req->len = 0;
	req->nbit = 0;
	setbitu((unsigned char*)req->buff,i,8,VRSPREAMB); i+=8;
	setbitu((unsigned char*)req->buff,i,16,0);        i+=16;
	if(!IOEncodeMsg(req,type)){
		req->nbyte = req->nbit = req->len = 0;
		return 0;
	}
    /* padding to align 8 bit boundary */
    for (i=req->nbit;i%8;i++) {
        setbitu((unsigned char*)req->buff,i,1,0);
    }
    /* message length (header+data) (bytes) */
    if ((req->len=i/8)>= sizeof(req->buff) - 3) {
        req->nbyte = req->nbit = req->len = 0;
        return 0;
    }
    /* message length without header and parity */
    setbitu((unsigned char*)req->buff,8,16,req->len-3);

    /* crc-24q */
    crc=crc24q((unsigned char*)req->buff,req->len);
    setbitu((unsigned char*)req->buff,i,32,crc);

    /* length total (bytes) */
    req->nbyte=req->len+4;

	return 1;
}
/*            +----------+--------+-----------+--------------------+----------+
 *            | preamble |   length   |    data message    |  parity  |
 *            +----------+--------+-----------+--------------------+----------+
 *            |<-- 8 --->|<--- 16 --->|<--- length x 8 --->|<-- 32 -->| */
int VrsDecode::IODecode(VrsReq* cli, unsigned char data) {
	if (cli->nbyte == 0) {
		if (data != VRSPREAMB) return 0;
		cli->buff[cli->nbyte++] = data;
		return 0;
	}
	cli->buff[cli->nbyte++] = data;
	if (cli->nbyte == 3) {
		cli->len = getbitu((unsigned char*)cli->buff, 8, 16) + 3; /* length without parity */
	}
	// in case of the message is too large
	if (cli->nbyte >= sizeof(cli->buff))
		cli->nbyte = 0;
	if (cli->nbyte < 3 || cli->nbyte < cli->len + 4) return 0;
	cli->nbyte = 0;
	if (crc24q((unsigned char*)cli->buff, cli->len) != getbitu((unsigned char*)cli->buff, cli->len * 8, 32)) {
		cout << "IODecode parity error:len = " << cli->len << endl;
		return 0;
	}
	return IODecodeMsg(cli);
}
int VrsDecode::IODecodeMsg(VrsReq* cli) {
	int ret = 0, type = getbitu((unsigned char*)cli->buff, 24, 8), length, i, nc;
	char buff[2400];
	char values[10][128] = { 0 };
	switch (type) {
	case 100:  // Authorized Message
		length = cli->len - 3 - 1;
		memcpy(cli->authbuf, cli->buff + 4, sizeof(char) * length);
		cli->authbuf[length] = '\0';
		ret = type;
		break;
	case 101: // Request Message
		i = 32;
		length = cli->len - 3 - 1;
		memcpy(buff, cli->buff + 4, sizeof(char) * length);
		buff[length] = '\0';
		/// Decode the message here
		split_string(true, buff, ' ', ' ', ',', &nc, (char*)values, 128);
		if (nc == 6 || nc == 7) {
			if (len_trim(values[0]) == 0 || len_trim(values[3]) == 0 ||
				len_trim(values[4]) == 0 || len_trim(values[5]) == 0)
				break;
			cli->id = atoi(values[0]);
			cli->geod[1] = atof(values[3]) * DEG2RAD;
			cli->geod[0] = atof(values[4]) * DEG2RAD;
			cli->geod[2] = atof(values[5]);
			if (cli->geod[1] < 0)
				cli->geod[1] += 2 * M_PI;
			if (cli->geod[0] < -M_PI / 2.0 || cli->geod[0] > M_PI / 2)
				break;
			if (cli->geod[1] < 0 || cli->geod[1] > M_PI * 2)
				break;
			cli->bstop = 0;
			if (nc == 7)
				cli->bstop = atoi(values[6]);
		}
		ret = type;
		break;
	case 102:
		ret = type;
		break;
	case 103:
		ret = type;
		break;
	case 104:
		i = 32;
		cli->id = getbitu((unsigned char*)cli->buff, i, 32);
		i = i + 32 + 16;
		memcpy(cli->rtcmbuf, cli->buff + i / 8, cli->len - i / 8);
		cli->rtcmlen = cli->len - i / 8;
		ret = type;
		break;
	}
	return ret;
}
