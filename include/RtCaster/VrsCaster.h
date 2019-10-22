#ifndef VRSCASTER_H_
#define VRSCASTER_H_
/* --------------------------------------------------------------------------
*  VrsCaster v1.0 Created by JunTao at Wuhan University based on Rtklib 2.4.3
*  function: 1) normal caster functions
*            2) vrs supported based on self defined protocol
----------------------------------------------------------------------------*/
#include "../Rtklib/rtklib.h"
#include "VrsConnector.h"
#include "VrsTables.h"
#include "../../include/Com/Log.h"
#include <list>
#include <map>
using namespace std;
namespace bamboo {
#define tracestat(format,...) VrsCaster::logger.m_wtMsg("@%s [%s]: " format "\n",Deploy::s_getInstance()->ptr_logfil, runlocaltime_str().c_str(), ##__VA_ARGS__);
	class VrsCaster {
	public:
		// functions 
		VrsCaster();
		void m_openCaster();
		void m_loopProcess();
	protected:
		// variables
		static Log logger;
		stream_t ntripc_c, ntripc_s;
		map<string, VrsConnector*> vrs_s;
	};
}
#endif