/*
 * Log.cpp
 *
 *  Created on: 2018年2月16日
 *      Author: juntao
 */

#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <string>
#include <stdarg.h>
#include <cstdlib>
#include "../../include/Com/Log.h"
#include "../../include/Com/Com.h"
#include "../../include/Rtklib/rtklib_fun.h"
using namespace std;
using namespace bamboo;
map<string,unsigned int> Log::timeLog;

// Done at 8.26
Log::Log(){
	initlock(&_lock);
}
Log::~Log(){
	map<string, LogItem*>::iterator itr;
	itr = this->mapOut.begin();
	while(itr != mapOut.end()){
		(*(*itr).second).of->flush();
		(*(*itr).second).of->close();
		delete (*itr).second->of;
		delete (*itr).second;
		itr = mapOut.erase(itr);
	}
}
void Log::m_openLog(string filName) {
	map<string, LogItem*>::iterator itr;
	itr = mapOut.find(string(filName));
	if (itr != mapOut.end()) {
		cout << "***WARINING(m_openLog):existing the same file " << filName
				<< endl;
		return;
	}
	ofstream* out = new ofstream;
	LogItem* item = new LogItem;
	item->of = out;
	out->open(filName.c_str(), ios::out);
	if (!(*out)) {
		cout << "cant create logger file " << filName << ",please the directory is exist!" << endl;
		exit(1);
	}
	lock(&_lock);
	this->mapOut.insert(pair<string, LogItem*>(filName, item));
	unlock(&_lock);
}
void Log::m_openLog(string filName,int interval){
	map<string, LogItem*>::iterator itr;
	itr = mapOut.find(string(filName));
	if (itr != mapOut.end()) {
		cout << "***WARINING(m_openLog):existing the same file " << filName
				<< endl;
		return;
	}
	ofstream* out = new ofstream;
	LogItem* item = new LogItem(interval);
	item->lupd = true;
	item->of = out;
	out->open(filName.c_str(), ios::out);
	if (!(*out)) {
		cout << "can't create logger file " << filName << ",please check the directory is exist!" << endl;
		exit(1);
	}
	lock(&_lock);
	this->mapOut.insert(pair<string, LogItem*>(filName, item));
	unlock(&_lock);
}
bool Log::m_lexist(string filName){
	map<string, LogItem*>::iterator itr;
	itr = mapOut.find(string(filName));
	if (itr != mapOut.end())
		return true;
	return false;
}
void Log::m_chkUpdate(const char* fil,LogItem* item){
	// check whether update the log file when the interval is coming
	if(item->lupd){
		char filname[1024] = {0};
		time_t now = time(NULL);
		struct tm* ptr;
		ptr = localtime(&now);
		int mjd = md_julday(ptr->tm_year + 1900,ptr->tm_mon + 1,ptr->tm_mday),gwk;
		double sod = ptr->tm_hour * 3600.0 + ptr->tm_min * 60.0 + ptr->tm_sec,gsow;
		mjd2wksow(mjd,sod,&gwk,&gsow);
		if(now - item->lastActive > item->interval){
			item->lastActive = (now / item->interval) * item->interval;
			item->of->flush();
			item->of->close();
			delete item->of;
			sprintf(filname,"%s.%04d%02d%02d-%02d:%02d:%02d",fil,ptr->tm_year + 1900,ptr->tm_mon + 1,ptr->tm_mday,ptr->tm_hour,
					ptr->tm_min,ptr->tm_sec);
			rename(fil,filname);
			item->of = new ofstream;
			item->of->open(fil, ios::out);
		}
	}
}
void Log::m_wtMsg(const char * fmt_msg, ...) {
	char log_msg_buff[40960] = { 0 };
	char filName[256] = {0};
	bool l_wtAll;
	map<string, LogItem*>::iterator itr;
	va_list args;
	va_start(args, fmt_msg);
	vsprintf(log_msg_buff, fmt_msg, args);
	va_end(args);
	l_wtAll = false;
	lock(&_lock);
	if (log_msg_buff[0] == '@') {
		sscanf(log_msg_buff, "@%s", filName);
		if(!strncmp(filName,"ALL",3) && strlen(filName) == 3)
			l_wtAll = true;
		itr = mapOut.find(filName);
		if (itr != mapOut.end()) {
			l_wtAll = false;
			// check whether update the file
			m_chkUpdate(filName,(*itr).second);
			(*(*(*itr).second).of) << log_msg_buff + strlen(filName) + 1 + 1; // remove the blankspace
			(*(*itr).second).of->flush();
		}
	}
	if (l_wtAll) {
		for (itr = mapOut.begin(); itr != mapOut.end(); itr++) {
			// check whether update the file
			m_chkUpdate((*itr).first.c_str(),(*itr).second);
			(*(*(*itr).second).of) << log_msg_buff;
			(*(*itr).second).of->flush();
		}
	}
	unlock(&_lock);
}
void Log::m_closeLog(string filname){
	map<string, LogItem*>::iterator itr;
	itr = mapOut.find(string(filname));
	lock(&_lock);
	if (itr != mapOut.end()) {
		(*(*itr).second).of->flush();
		(*(*itr).second).of->close();
		delete (*itr).second->of;
		delete (*itr).second;
		mapOut.erase(itr);
	}
	map<string,map<string,unsigned int> >::iterator iTtr;
	iTtr = mtimeLog.find(filname);
	if(iTtr != mtimeLog.end()){
		mtimeLog.erase(iTtr);
	}
	unlock(&_lock);
}
void Log::m_tmeTag(string filName,string tag){
	unsigned int tt = tickget();
	map<string,map<string,unsigned int> >::iterator itr;
	lock(&_lock);
	itr = mtimeLog.find(filName);
	if(itr == mtimeLog.end()){
		map<string,unsigned int> tmp;
		tmp.insert(make_pair(tag, tt));
		mtimeLog.insert(make_pair(filName,tmp));
	}
	else
		(*itr).second.insert(make_pair(tag, tt));
	unlock(&_lock);
}
void Log::m_tmeConsume(string filName,string tag){
	double sec;
	unsigned int tv_end = tickget();
	map<string,map<string,unsigned int> >::iterator itr;
	lock(&_lock);
	itr = mtimeLog.find(filName);
	if(itr == mtimeLog.end()){
		cout << "***WARNING(m_tmeConsume) Wrong input FileName!" << endl;
		unlock(&_lock);
		return;
	}
	map<string,unsigned int>::iterator sItr;
	sItr = (*itr).second.find(tag);
	if(sItr == (*itr).second.end()){
		cout << "***WARNING(m_tmeConsume) Wrong input Tag!" << endl;
		unlock(&_lock);
		return;
	}
	sec = tv_end - (*sItr).second;
	this->m_wtMsg("@%s %s TIME CONSUME %lf\n",filName.c_str(),tag.c_str(),sec / 1000.0);
	(*itr).second.erase(sItr);
	unlock(&_lock);
}
void Log::s_tmeTag(string tag){
	unsigned int tv = tickget();
	timeLog.insert(make_pair(tag, tv));
}
void Log::s_tmeConsume(string tag){
	double sec;
	unsigned int tv_end = tickget();
	map<string,unsigned int>::iterator itr;

	itr = timeLog.find(tag);
	if(itr == timeLog.end()){
		cout << "***WARNING(m_tmeConsume) Wrong input tag!" << endl;
		return;
	}
	sec = tv_end - (*itr).second;
	printf("%-60s TIME CONSUME: %10.3lf\n",tag.c_str(),sec / 1000.0);
	timeLog.erase(itr);
}
