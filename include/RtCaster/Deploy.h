#ifndef DEPLOY_H_
#define DEPLOY_H_
#include <string>
#include <list>
#include <cstdlib>
#include <iostream>
using namespace std;
namespace bamboo {
	class Deploy {
	public:
		Deploy();
		inline static Deploy* s_getInstance(int argc, char* args[]) {
			if (sInstance == NULL)
				sInstance = new Deploy(argc, args);
			return sInstance;
		}
		inline static Deploy* s_getInstance() {
			if (sInstance == NULL) {
				cout << "Instance is not initialize yet" << endl;
				exit(1);
			}
			return sInstance;
		}
		bool m_checkIsVrs(string mnt);
		string m_genVrsSourceTable();
	public:
		// variables
		double log_dintv;
		char ptr_logfil[256];
		char ptr_sourcetable[1024], ptr_casterc[256], ptr_casters[256];
		list<string> ptr_castervrs;
		list<string> ptr_mnts,ptr_sourceinfo;
	protected:
		Deploy(int, char* args[]);
		void m_readConfiguresJson(bool);
	protected:
		time_t lastAct;
		char f_jsonConfigures[1024];
		
		static Deploy* sInstance;
	};
}
#endif