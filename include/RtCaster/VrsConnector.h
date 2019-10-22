#ifndef VRSCONNECTOR_H_
#define VRSCONNECTOR_H_
#include <string>
#include "../Rtklib/rtklib.h"
#include "../Com/VrsDecode.h"
using namespace std;
namespace bamboo {
	class VrsConnector {
	public:
		//functions;
		VrsConnector(string path);
		int inputVrsData(unsigned char);
		void inputVrsRequest(sol_t* sol,int msgid);

		stream_t svr;
		char payload[1024],mntpnt[1024];
		int nload,msgid;
	protected:
		// variables
		VrsReq req,reply;
		char path[1024];
	};
}
#endif
