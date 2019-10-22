#include "../../include/RtCaster/VrsCaster.h"
#include "../../include/RtCaster/Deploy.h"
using namespace std;
using namespace bamboo;
int main(int argc, char* args[]) {
	Deploy* dly = Deploy::s_getInstance(argc,args);
	VrsCaster caster;
	caster.m_openCaster();
	caster.m_loopProcess();
	return 0;
}