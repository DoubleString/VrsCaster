#include "../../include/RtCaster/VrsCaster.h"
#include "../../include/Rtklib/rtklib_fun.h"
#include "../../include/RtCaster/Deploy.h"
#include "../../include/Com/Com.h"
using namespace bamboo;
Log VrsCaster::logger;
VrsCaster::VrsCaster() {
	Deploy* dly = Deploy::s_getInstance();
	strinitcom();
	strinit(&ntripc_s);
	strinit(&ntripc_c);
	logger.m_openLog(dly->ptr_logfil, dly->log_dintv);
}
void VrsCaster::m_openCaster() {
	list<string>::iterator strItr, mntItr;
	Deploy* dly = Deploy::s_getInstance();
	stropen(&ntripc_c, STR_NTRIPC_C, STR_MODE_RW, dly->ptr_casterc);
	stropen(&ntripc_s, STR_NTRIPC_S, STR_MODE_RW, dly->ptr_casters);
	/* open vrs server */
	for (strItr = dly->ptr_castervrs.begin(),mntItr = dly->ptr_mnts.begin(); strItr != dly->ptr_castervrs.end(); ++strItr,++mntItr) {
		VrsConnector* connector = new VrsConnector(*strItr);
		this->vrs_s[*mntItr] = connector;
	}
	/* set source table configrue*/
	strsetsrctbl(&ntripc_c, dly->ptr_sourcetable);
	strsetsrctbl(&ntripc_s, dly->ptr_sourcetable);
	tracestat("/* --------------------------------------------------------------------------");
	tracestat("*  VrsCaster v1.0 Created by JunTao at Wuhan University based on Rtklib 2.4.3");
	tracestat("*  function: 1) normal caster functions");
	tracestat("*            2) vrs supported based on self defined protocol");
	tracestat("----------------------------------------------------------------------------*/");


}
void VrsCaster::m_loopProcess() {
	int nbyte, msgid, ret, i,laststat = 0;
	char buff[2048], mnt[1024] = { 0 },msg[2048];
	map<string, VrsConnector*>::iterator mapItr;
	/* loop for process */
	while (true) {
		/* vrs requst */
		nbyte = strread(&ntripc_c, (unsigned char*)buff, sizeof(buff));
		if (nbyte > 0) {
			strgetmsgid(&ntripc_c, &msgid);
			if (validMsg(msgid)) {
				/* test gga here*/
				buff[nbyte] = '\0';
				if (test_nmea(buff)) {
					/* sending corresponding data to reomte vrs server*/
					strgetsel(&ntripc_c, mnt);
					mapItr = vrs_s.find(mnt);
					if (mapItr != vrs_s.end()) {
						// sending request here 
						sol_t sol;
						sol.time.time = time(NULL);
						decode_nmea(buff, &sol);
						(*mapItr).second->inputVrsRequest(&sol, msgid);
					}
				}
			}
		}
		/* vrs response */
		for (mapItr = vrs_s.begin(); mapItr != vrs_s.end(); ++mapItr) {
			nbyte = strread(&(*mapItr).second->svr, (unsigned char*)buff, sizeof(buff));
			for (i = 0; i < nbyte; i++) {
				ret = (*mapItr).second->inputVrsData(buff[i]);
				if (ret == 1) {
					strsetmsgid(&ntripc_c, (*mapItr).second->msgid);
					strsetsel(&ntripc_c, (*mapItr).first.c_str());
					strwrite(&ntripc_c, (unsigned char*)(*mapItr).second->payload, (*mapItr).second->nload);
				}
			}
		}
		/* normal station request */
		nbyte = strread(&ntripc_s, (unsigned char*)buff, sizeof(buff));
		if (nbyte > 0) {
			/* setting data */
			strgetsel(&ntripc_s, mnt);
			strsetsel(&ntripc_c, mnt);
			strgetmsgid(&ntripc_s, &msgid);
			strsetmsgid(&ntripc_c, msgid);
			strwrite(&ntripc_c, (unsigned char*)buff, nbyte);
		}
		/* tracing state every 30s */
		if(time(NULL) - laststat > 0){
			laststat = laststat + 30;
			strsetsrctbl(&ntripc_c, dly->ptr_sourcetable);
			strsetsrctbl(&ntripc_s, dly->ptr_sourcetable);
		}
	}
}
