#include "../../include/RtCaster/VrsConnector.h"
#include "../../include/Rtklib/rtklib_fun.h"
#include "../../include/Com/VrsDecode.h"
#include "../../include/Com/Com.h"
using namespace std;
using namespace bamboo;
VrsConnector::VrsConnector(string path_in) {
	strcpy(path, path_in.c_str());
	strinit(&svr);
	stropen(&svr, STR_VRSCONCLI, STR_MODE_RW, path);
}
int VrsConnector::inputVrsData(unsigned char data) {
	/// decode the output info here
	int ret = VrsDecode::s_getInstance()->IODecode(&reply,data);
	if (ret == 104) {
		msgid = reply.id;
		nload = reply.rtcmlen;
		memcpy(payload, reply.rtcmbuf, sizeof(char)*reply.rtcmlen);
		return 1;
	}
	return 0;
}
void VrsConnector::inputVrsRequest(sol_t* sol,int msgid) {
	req.id = msgid;
	time2mjd(sol->time.time, &req.mjd, &req.sod);
	ecef2pos(sol->rr,req.geod);
	VrsDecode::s_getInstance()->IOEncode(&req, 101);
	strwrite(&svr, (unsigned char*)req.buff, sizeof(char)*req.nbyte);
}

