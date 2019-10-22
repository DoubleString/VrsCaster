#include <fstream>
#include "../../include/Json/json.h"
#include "../../include/RtCaster/Deploy.h"
#include "../../include/Com/Com.h"
#include "../../include/Rtklib/rtklib_fun.h"
using namespace std;
using namespace bamboo;
Deploy* Deploy::sInstance = NULL;
Deploy::Deploy(int nargc,char* args[]) {
	/// using init value 
	int iargc;
	lastAct = 0;
	for (iargc = 0; iargc < nargc; iargc++) {
		if (!strcmp(args[iargc], "-h") || !strcmp(args[iargc], "-help")) {
			cout << "Usage:RtConverter -conf configures.json" << endl;
			exit(1);
		}
		if (!strcmp(args[iargc], "-conf")) {
			strcpy(f_jsonConfigures, args[++iargc]);
		}
	}
	this->m_readConfiguresJson(true);
}
Deploy::Deploy() {
	lastAct = 0;
}
bool Deploy::m_checkIsVrs(string mnt) {
	list<string>::iterator strItr;
	for (strItr = ptr_mnts.begin(); strItr != ptr_mnts.end(); ++strItr) {
		if (*strItr == mnt) return true;
	}
	return false;
}
string Deploy::m_genVrsSourceTable() {
	string source = "\r\n";
	list<string>::iterator strItr;
	for (strItr = ptr_sourceinfo.begin(); strItr != ptr_sourceinfo.end(); ++strItr) {
		source += (*strItr) + "\r\n";
	}
	return source;
}
void Deploy::m_readConfiguresJson(bool lexit) {
	fstream f;
	int ivrs;
	double dsec, seslen;
	string item;
	char value[1024] = { 0 }, mnt[256] = { 0 };
	Json::CharReaderBuilder rbuilder;
	rbuilder["collectComments"] = false;
	Json::Value root;
	JSONCPP_STRING errs;
	f.open(f_jsonConfigures, ios::in);
	if (!f.is_open()) {
		printf("file = %s,failed to open file", f_jsonConfigures);
		if (lexit) exit(1); else return;
	}
	bool parse_ok = Json::parseFromStream(rbuilder, f, &root, &errs);
	f.close();
	if (!parse_ok) {
		printf("file = %s,failed to parse file", f_jsonConfigures);
		if (lexit) exit(1); else return;
	}
	try {
		item = "caster";
		for (ivrs = 0; ivrs < root[item]["log"].size(); ivrs++) {
			if(ivrs == 0)
				bamboo::excludeAnnoValue(ptr_logfil, root[item]["log"][ivrs].asCString());
			else {
				bamboo::excludeAnnoValue(value, root[item]["log"][ivrs].asCString());
				log_dintv = atof(value) * 3600.0;
			}
		}
		bamboo::excludeAnnoValue(ptr_sourcetable, root[item]["sourcetable"].asCString());
		bamboo::excludeAnnoValue(ptr_casterc, root[item]["casterc-path"].asCString());
		bamboo::excludeAnnoValue(ptr_casters, root[item]["casters-path"].asCString());

		for (ivrs = 0; ivrs < root[item]["caster-vrslist"].size(); ivrs++) {
			bamboo::excludeAnnoValue(value, root[item]["caster-vrslist"][ivrs].asCString());
			decodetcppath(value, NULL, NULL, NULL, NULL, mnt, NULL);
			ptr_castervrs.push_back(value);
			ptr_mnts.push_back(mnt);
		}
		for (ivrs = 0; ivrs < root[item]["caster-vrsinfo"].size(); ivrs++) {
			bamboo::excludeAnnoValue(value, root[item]["caster-vrsinfo"][ivrs].asCString());
			ptr_sourceinfo.push_back(value);
		}
	}
	catch (...) {
		printf("file = %s,failed to parse database item", f_jsonConfigures);
		if (lexit) exit(1); else return;
	}
}