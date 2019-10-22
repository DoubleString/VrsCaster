/*
 * Log.h
 *
 *  Created on: 2018/2/16
 *      Author: doublestring
 */

#ifndef INCLUDE_BAMBOO_LOG_H_
#define INCLUDE_BAMBOO_LOG_H_

#include <iostream>
#include <map>
#include <time.h>
#include <fcntl.h>
#include "../Rtklib/rtklib.h"
using namespace std;
namespace bamboo{
class LogItem{
public:
	LogItem(){
		of = NULL;
		interval = 86400;
		lupd = false;
		time(&lastActive);
	}
	LogItem(int intv){
		of = NULL;
		interval = intv;
		lupd = false;
		time(&lastActive);
		lastActive = (lastActive / intv) * intv; // make it into the nearest
	}
	ofstream* of;
	time_t lastActive;
	int interval,lupd;
};
class Log {
public:
	Log();
	~Log();
	void m_openLog(string filName);
	void m_openLog(string filName,int interval);
	void m_closeLog(string filName);
	bool m_lexist(string filName);
	void m_wtMsg(const char * fmt_msg, ...);
	void m_chkUpdate(const char* ,LogItem*);
	void m_tmeTag(string filName,string tag);  // thread safe
	void m_tmeConsume(string filName,string tag);
	static void s_tmeTag(string);  // not thread safe
	static void s_tmeConsume(string);
protected:
	map<string, LogItem*> mapOut;
	map<string,map<string, unsigned int> >  mtimeLog;
	static map<string,unsigned int> timeLog;
	lock_t _lock;
};
}
#endif /* INCLUDE_BAMBOO_LOG_H_ */
