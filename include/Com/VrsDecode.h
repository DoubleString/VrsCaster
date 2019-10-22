/*
 * VrsDecode.h
 *
 *  Created on: 2019/5/21
 *      Author: jtao
 */

#ifndef INCLUDE_VRS_NEW_VRSDECODE_H_
#define INCLUDE_VRS_NEW_VRSDECODE_H_

#include <iostream>
#include <cmath>
#include "../Rtklib/rtklib.h"
#include "../Rtklib/rtklib_fun.h"
using namespace std;
namespace bamboo{
const int VRSPREAMB = 0xDA;
const double DEG2RAD = M_PI / 180.0;
const double RAD2DEG = 180.0 / M_PI;
class VrsReq {
public:
	VrsReq() {
		bstop = nbyte = nbit = len = 0;
	}
	///// results bytes
	int nbyte, nbit, len,mjd,id,bstop,rtcmlen;
	double geod[3], sod;
	char buff[2048], usrpsd[1024],authbuf[1024],rtcmbuf[2048];
};
class VrsDecode{
public:
	VrsDecode(){}
	static VrsDecode* s_getInstance(){
		if(m_Instance == NULL)
			m_Instance = new VrsDecode;
		return m_Instance;
	}
	int IOEncode(VrsReq* req,int type);
	int IOEncodeMsg(VrsReq* req,int type);
	int IOEncode103(VrsReq* req);
	int IOEncode102(VrsReq* req);
	int IOEncode101(VrsReq* req);
	int IOEncode100(VrsReq* req);
	int enbase64(char *str, const unsigned char *byte, int n);

	int IODecodeMsg(VrsReq* cli);
	int IODecode(VrsReq* cli, unsigned char data);
private:
	static VrsDecode* m_Instance;
};
}
#endif /* INCLUDE_VRS_NEW_VRSDECODE_H_ */
