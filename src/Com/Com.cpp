#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <fstream>
#include <cmath>
#include "../../include/Com/Com.h"
#include "../../include/Rtklib/rtklib_fun.h"
using namespace std;
namespace bamboo {
	int md_julday(int iyear, int imonth, int iday) {
		int iyr, result;
		int doy_of_month[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304,
				334 };
		if (iyear < 0 || imonth < 0 || iday < 0 || imonth > 12 || iday > 366
			|| (imonth != 0 && iday > 31)) {
			printf("iyear = %d,imonth = %d,iday = %d,incorrect argument", iyear, imonth, iday);
			exit(1);
		}
		iyr = iyear;
		if (imonth <= 2)
			iyr -= 1;
		result = 365 * iyear - 678941 + iyr / 4 - iyr / 100 + iyr / 400 + iday;
		if (imonth != 0)
			result = result + doy_of_month[imonth - 1];
		return result;
	}
	void gpsweek(int year, int month, int day, int* week, int* wd) {
		int mjd;
		if (year != 0) {
			if (month != 0)
				mjd = md_julday(year, month, day);
			else
				mjd = md_julday(year, 1, 1) + day - 1;
		}
		else
			mjd = day;
		*week = (mjd - 44244) / 7;
		*wd = mjd - 44244 - (*week) * 7;
	}

	void mjd2doy(int jd, int* iyear, int* idoy) {
		*iyear = (jd + 678940) / 365;
		*idoy = jd - md_julday(*iyear, 1, 1);
		while (*idoy <= 0) {
			(*iyear)--;
			*idoy = jd - md_julday(*iyear, 1, 1) + 1;
		}
	}

	void cur_time(int* mjd, double* sod) {
		//	struct timeval tv;
		//    struct tm* ptr;
		//	gettimeofday(&tv,NULL);
		//	ptr = gmtime (&tv.tv_sec);
		//	*mjd = md_julday(ptr->tm_year + 1900,ptr->tm_mon + 1,ptr->tm_mday);
		//	*sod = ptr->tm_hour * 3600.0 + ptr->tm_min * 60.0 + ptr->tm_sec + tv.tv_usec / 1e6;
		//
		//	timinc(*mjd, *sod, Taiutc::s_getInstance()->m_getTaiutc(*mjd) - 19.0, mjd, sod);
		struct tm *ptr;
		time_t rawtime;
		time(&rawtime);
		ptr = gmtime(&rawtime);
		*mjd = md_julday(ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday);
		*sod = ptr->tm_hour * 3600.0 + ptr->tm_min * 60.0 + ptr->tm_sec;
		// ´ÓUTC×ª³ÉGPST
		timinc(*mjd, *sod, 38 - 19.0, mjd, sod);
	}
	void timinc(int jd, double sec, double delt, int* jd1, double* sec1) {
		*sec1 = sec + delt;
		int inc = (int)(*sec1 / 86400.0);
		*jd1 = jd + inc;
		*sec1 = *sec1 - inc * 86400.0;
		if (*sec1 >= 0)
			return;
		*jd1 = *jd1 - 1;
		*sec1 = *sec1 + 86400;
	}
	char* runtime(char* buf) {
		int iy, im, id, ih, imin, mjd;
		double dsec, sod;
		cur_time(&mjd, &sod);
		mjd2date(mjd, sod, &iy, &im, &id, &ih, &imin, &dsec);
		if (buf != NULL)
			sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", iy, im, id, ih, imin, (int)dsec);
		return buf;
	}
	char* runlocaltime(char* buf) {
		int iy, im, id, ih, imin, mjd;
		double dsec, sod;
		struct tm *ptr;
		time_t rawtime;
		time(&rawtime);
		ptr = localtime(&rawtime);
		mjd = md_julday(ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday);
		sod = ptr->tm_hour * 3600.0 + ptr->tm_min * 60.0 + ptr->tm_sec;
		mjd2date(mjd, sod, &iy, &im, &id, &ih, &imin, &dsec);
		if (buf != NULL)
			sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", iy, im, id, ih, imin,
			(int)dsec);
		return buf;
	}
	string runlocaltime_str() {
		int iy, im, id, ih, imin, mjd;
		double dsec, sod;
		struct tm *ptr;
		char buf[1024] = { 0 };
		time_t rawtime;
		time(&rawtime);
		ptr = localtime(&rawtime);
		mjd = md_julday(ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday);
		sod = ptr->tm_hour * 3600.0 + ptr->tm_min * 60.0 + ptr->tm_sec;
		mjd2date(mjd, sod, &iy, &im, &id, &ih, &imin, &dsec);
		
		sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", iy, im, id, ih, imin, (int)dsec);
		return string(buf);
	}


	char* run_timefmt(int mjd, double sod, char* buf) {
		int iy, im, id, ih, imin;
		double dsec;
		mjd2date(mjd, sod, &iy, &im, &id, &ih, &imin, &dsec);
		if (buf != NULL)
			sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", iy, im, id, ih, imin, (int)dsec);
		return buf;
	}
	void yeardoy2monthday(int iyear, int idoy, int* imonth, int* iday) {
		int days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		int id, i;
		if ((iyear % 4 == 0 && iyear % 100 != 0) || iyear % 400 == 0)
			days_in_month[1] = 29;
		id = idoy;
		for (i = 0; i < 12; i++) {
			id = id - days_in_month[i];
			if (id > 0)
				continue;
			*iday = id + days_in_month[i];
			*imonth = i + 1;
			break;
		}
	}
	double timdif(int jd2, double sod2, int jd1, double sod1) {
		return 86400.0 * (jd2 - jd1) + sod2 - sod1;
	}
	void yr2year(int& yr) {
		if (yr > 1900)
			return;
		if (yr <= 30)
			yr += 2000;
		else
			yr += 1900;
	}
	void mjd2wksow(int mjd, double sod, int *week, double *sow) {
		*week = (int)((mjd + sod / 86400.0 - 44244.0) / 7.0);
		*sow = (mjd - 44244.0 - *week * 7) * 86400.0 + sod;
	}
	time_t mjd2time(int mjd, double sod) {
		const int doy[] = { 1,32,60,91,121,152,182,213,244,274,305,335 };
		time_t time = 0;
		int days, sec, year, mon, day, hour, min;
		double dsec;
		mjd2date(mjd, sod, &year, &mon, &day, &hour, &min, &dsec);
		if (year < 1970 || 2099 < year || mon < 1 || 12 < mon) return time;

		/* leap year if year%4==0 in 1901-2099 */
		days = (year - 1970) * 365 + (year - 1969) / 4 + doy[mon - 1] + day - 2 + (year % 4 == 0 && mon >= 3 ? 1 : 0);
		sec = (int)floor(dsec);
		time = (time_t)days * 86400 + hour * 3600 + min * 60 + sec;
		return time;
	}
	void time2mjd(time_t time, int* mjd, double* sod) {
		double ep[6];
		const int mday[] = { /* # of days in a month */
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31,
				31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
				31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		int days, sec, mon, day;

		/* leap year if year%4==0 in 1901-2099 */
		days = (int)(time / 86400);
		sec = (int)(time - (time_t)days * 86400);
		for (day = days % 1461, mon = 0; mon < 48; mon++) {
			if (day >= mday[mon])
				day -= mday[mon];
			else
				break;
		}
		ep[0] = 1970 + days / 1461 * 4 + mon / 12;
		ep[1] = mon % 12 + 1;
		ep[2] = day + 1;
		ep[3] = sec / 3600;
		ep[4] = sec % 3600 / 60;
		ep[5] = sec % 60;

		*mjd = md_julday((int)ep[0], (int)ep[1], (int)ep[2]);
		*sod = ep[3] * 3600.0 + ep[4] * 60.0 + ep[5];
	}
	void wksow2mjd(int week, double sow, int* mjd, double* sod) {
		if (mjd != NULL)
			*mjd = (int)(sow / 86400.0) + week * 7 + 44244;
		if (sod != NULL)
			*sod = fmod(sow, 86400.0);
	}
	void mjd2date(int jd, double sod, int* iyear, int* imonth, int* iday, int* ih,
		int* imin, double* sec) {
		int doy = 0;
		mjd2doy(jd, iyear, &doy);
		yeardoy2monthday(*iyear, doy, imonth, iday);

		*ih = static_cast<int>(sod / 3600.0);
		*imin = static_cast<int> ((sod - (*ih) * 3600.0) / 60.0);
		*sec = sod - (*ih) * 3600.0 - (*imin) * 60.0;
	}
	int pointer_string(int cnt, string string_array[], string str) {
		int itr, idx = -1;
		for (itr = 0; itr < cnt; itr++) {
			if (string_array[itr] == str) {
				idx = itr;
				break;
			}
		}
		return idx;
	}
	/**********************CHANGED*****************************/
	int pointer_charstr(int row, int col, const char* string_array, const char* string) {
		int i;
		const char* pStr = (const char*)string_array;
		for (i = 0; i < row; i++) {
			if (strcmp(pStr + i * col, string) == 0)
				break;
		}
		if (i == row)
			i = -1;
		return i;
	}
	int index_string(const char* src, char key) {
		int len = strlen(src);
		int i;
		for (i = 0; i < len; i++) {
			if (src[i] == key)
				break;
		}
		if (i == len)
			return -1;
		else
			return i;
	}
	// start: started with index of zero
	char* substringEx(char* dest, const char* src, int start, int length) {
		int i, j = 0;
		int len = strlen(src);
		if (start < 0 || start >= len || start + length > len) {
			dest[0] = '\0';
			return NULL;
		}
		for (i = start; i < start + length; i++) {
			dest[j] = src[i];
			j++;
		}
		dest[j] = '\0';
		return dest;
	}
	char* left_justify_string(char* string) {
		int p = 0;
		while (*(string + p) == ' ' || *(string + p) == '\n'
			|| *(string + p) == '\r')
			p++;
		return string + p;
	}
	char* toUpper(char* value) {
		int len = strlen((const char*)value);
		int i;
		for (i = 0; i < len; i++) {
			if (value[i] >= 'a' && value[i] <= 'z')
				value[i] -= 32;
		}
		return value;
	}

	char* toLower(char* value) {
		int len = strlen((const char*)value);
		int i;
		for (i = 0; i < len; i++) {
			if (value[i] >= 'A' && value[i] <= 'Z')
				value[i] += 32;
		}
		return value;
	}
	char* trim(char* pStr) {
		int len = strlen(pStr);
		char* pIndex = pStr + len - 1;
		while (*pIndex == '\n' || *pIndex == '\r' || *pIndex == '\0'
			|| *pIndex == ' ') {
			*pIndex = '\0';
			if (pStr == pIndex)
				break;
			pIndex--;
		}
		return pStr;
	}
	int len_trim(const char* pStr) {
		int length = strlen(pStr);
		int count = length;
		int i;
		for (i = length - 1; i >= 0; i--) {
			if (pStr[i] == '\0' || pStr[i] == '\n' || pStr[i] == '\r'
				|| pStr[i] == ' ')
				count--;
			else
				break;
		}
		return count;
	}
	void split_string(bool lnoempty, char* string, char c_start, char c_end,
		char seperator, int* nword, char* keys, int len) {
		int i0 = 0, i1, ilast, i;
		char varword[1024];
		char* word = (char*)keys;
		i1 = len_trim(string) - 1;
		if (c_start != ' ')
			i0 = index_string(string, c_start);
		if (c_end != ' ')
			i1 = index_string(string, c_end);
		*nword = 0;
		if (i1 == -1 || i0 > i1)
			return;

		ilast = i0;
		if (string[i1] != seperator)
			string[i1 + 1] = seperator;
		string[i1 + 2] = '\0';

		for (i = i0; i < i1 + 2; i++) {
			if (string[i] == seperator) {
				if (i - 1 >= ilast) {
					substringEx(varword, string, ilast, i - ilast);
					strcpy(word + (*nword)*len, varword);
					(*nword)++;
				}
				else {
					if (!lnoempty) {
						strcpy(word + (*nword)*len, "  ");
						(*nword)++;
					}
				}
				ilast = i + 1;
			}
		}
	}
	void excludeAnnoValue(char* value, const char* in) {
		int num = 0, lstart = false;
		const char* ptr = in;
		while (*ptr != '\0') {
			if (lstart == false && (*ptr == ' ' || *ptr == '\n' || *ptr == '\t')) {
				++ptr;
				continue;
			}
			lstart = true;
			if (*ptr != '#' && *ptr != '!') {
				value[num++] = *ptr;
				++ptr;
			}
			else
				break;
		}
		value[num++] = '\0';
		trim(value);
	}
	string zipJson(string json) {
		string zipStr = "";
		int isize = 0, jsize = json.size();
		char *zipPtr = new char[jsize];
		const char *ptr = json.c_str();
		memset(zipPtr, 0, sizeof(char) * jsize);
		while (ptr - json.c_str() < json.size()) {
			if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
				++ptr;
				continue;
			}
			zipPtr[isize++] = *ptr;
			++ptr;
		}
		zipStr = string(zipPtr);
		delete[] zipPtr;
		return zipStr;
	}
	/* convert time in nmea format to time ---------------------------------------*/
	void septime(double t, double *t1, double *t2, double *t3)
	{
		*t1 = floor(t / 10000.0);
		t -= *t1*10000.0;
		*t2 = floor(t / 100.0);
		*t3 = t - *t2*100.0;
	}
	double dmm2deg(double dmm)
	{
		return floor(dmm / 100.0) + fmod(dmm, 100.0) / 60.0;
	}
	/* decode nmea gxrmc: recommended minumum data for gps -----------------------*/
	int decode_nmearmc(char **val, int n, sol_t *sol)
	{
		double tod = 0.0, lat = 0.0, lon = 0.0, vel = 0.0, dir = 0.0, date = 0.0, ang = 0.0, ep[6];
		double pos[3] = { 0 };
		char act = ' ', ns = 'N', ew = 'E', mew = 'E', mode = 'A';
		int i;

		trace(4, "decode_nmearmc: n=%d\n", n);

		for (i = 0; i < n; i++) {
			switch (i) {
			case  0: tod = atof(val[i]); break; /* time in utc (hhmmss) */
			case  1: act = *val[i];      break; /* A=active,V=void */
			case  2: lat = atof(val[i]); break; /* latitude (ddmm.mmm) */
			case  3: ns = *val[i];      break; /* N=north,S=south */
			case  4: lon = atof(val[i]); break; /* longitude (dddmm.mmm) */
			case  5: ew = *val[i];      break; /* E=east,W=west */
			case  6: vel = atof(val[i]); break; /* speed (knots) */
			case  7: dir = atof(val[i]); break; /* track angle (deg) */
			case  8: date = atof(val[i]); break; /* date (ddmmyy) */
			case  9: ang = atof(val[i]); break; /* magnetic variation */
			case 10: mew = *val[i];      break; /* E=east,W=west */
			case 11: mode = *val[i];      break; /* mode indicator (>nmea 2) */
									  /* A=autonomous,D=differential */
									  /* E=estimated,N=not valid,S=simulator */
			}
		}
		if ((act != 'A'&&act != 'V') || (ns != 'N'&&ns != 'S') || (ew != 'E'&&ew != 'W')) {
			trace(2, "invalid nmea gprmc format\n");
			return 0;
		}
		pos[0] = (ns == 'S' ? -1.0 : 1.0)*dmm2deg(lat)*D2R;
		pos[1] = (ew == 'W' ? -1.0 : 1.0)*dmm2deg(lon)*D2R;
		septime(date, ep + 2, ep + 1, ep);
		septime(tod, ep + 3, ep + 4, ep + 5);
		ep[0] += ep[0] < 80.0 ? 2000.0 : 1900.0;
		sol->time = utc2gpst(epoch2time(ep));
		pos2ecef(pos, sol->rr);
		sol->stat = mode == 'D' ? SOLQ_DGPS : SOLQ_SINGLE;
		sol->ns = 0;

		sol->type = 0; /* postion type = xyz */

		trace(5, "decode_nmearmc: %s rr=%.3f %.3f %.3f stat=%d ns=%d vel=%.2f dir=%.0f ang=%.0f mew=%c mode=%c\n",
			time_str(sol->time, 0), sol->rr[0], sol->rr[1], sol->rr[2], sol->stat, sol->ns,
			vel, dir, ang, mew, mode);

		return 2; /* update time */
	}
	/* decode nmea gxzda: utc day,month,year and local time zone offset ----------*/
	int decode_nmeazda(char **val, int n, sol_t *sol)
	{
		double tod = 0.0, ep[6] = { 0 };
		int i;

		trace(4, "decode_nmeazda: n=%d\n", n);

		for (i = 0; i < n; i++) {
			switch (i) {
			case  0: tod = atof(val[i]); break; /* time in utc (hhmmss) */
			case  1: ep[2] = atof(val[i]); break; /* day (0-31) */
			case  2: ep[1] = atof(val[i]); break; /* mon (1-12) */
			case  3: ep[0] = atof(val[i]); break; /* year */
			}
		}
		septime(tod, ep + 3, ep + 4, ep + 5);
		sol->time = utc2gpst(epoch2time(ep));
		sol->ns = 0;

		trace(5, "decode_nmeazda: %s\n", time_str(sol->time, 0));

		return 2; /* update time */
	}
	/* decode nmea gxgga: fix information ----------------------------------------*/
	int decode_nmeagga(char **val, int n, sol_t *sol)
	{
		const int solq_nmea[] = { 
			/* nmea quality flags to rtklib sol quality */
			/* nmea 0183 v.2.3 quality flags: */
			/*  0=invalid, 1=gps fix (sps), 2=dgps fix, 3=pps fix, 4=rtk, 5=float rtk */
			/*  6=estimated (dead reckoning), 7=manual input, 8=simulation */
			SOLQ_NONE ,SOLQ_SINGLE, SOLQ_DGPS, SOLQ_PPP , SOLQ_FIX,
			SOLQ_FLOAT,SOLQ_DR    , SOLQ_NONE, SOLQ_NONE, SOLQ_NONE
		};
		gtime_t time;
		double tod = 0.0, lat = 0.0, lon = 0.0, hdop = 0.0, alt = 0.0, msl = 0.0, ep[6], tt;
		double pos[3] = { 0 };
		char ns = 'N', ew = 'E', ua = ' ', um = ' ';
		int i, solq = 0, nrcv = 0;

		trace(4, "decode_nmeagga: n=%d\n", n);

		for (i = 0; i < n; i++) {
			switch (i) {
			case  0: tod = atof(val[i]); break; /* time in utc (hhmmss) */
			case  1: lat = atof(val[i]); break; /* latitude (ddmm.mmm) */
			case  2: ns = *val[i];      break; /* N=north,S=south */
			case  3: lon = atof(val[i]); break; /* longitude (dddmm.mmm) */
			case  4: ew = *val[i];      break; /* E=east,W=west */
			case  5: solq = atoi(val[i]); break; /* fix quality */
			case  6: nrcv = atoi(val[i]); break; /* # of satellite tracked */
			case  7: hdop = atof(val[i]); break; /* hdop */
			case  8: alt = atof(val[i]); break; /* altitude in msl */
			case  9: ua = *val[i];      break; /* unit (M) */
			case 10: msl = atof(val[i]); break; /* height of geoid */
			case 11: um = *val[i];      break; /* unit (M) */
			}
		}
		if ((ns != 'N'&&ns != 'S') || (ew != 'E'&&ew != 'W')) {
			trace(2, "invalid nmea gpgga format\n");
			return 0;
		}
		if (sol->time.time == 0.0) {
			trace(2, "no date info for nmea gpgga\n");
			return 0;
		}
		pos[0] = (ns == 'N' ? 1.0 : -1.0)*dmm2deg(lat)*D2R;
		pos[1] = (ew == 'E' ? 1.0 : -1.0)*dmm2deg(lon)*D2R;
		pos[2] = alt + msl;

		time2epoch(sol->time, ep);
		septime(tod, ep + 3, ep + 4, ep + 5);
		time = utc2gpst(epoch2time(ep));
		tt = timediff(time, sol->time);
		if (tt < -43200.0) sol->time = timeadd(time, 86400.0);
		else if (tt > 43200.0) sol->time = timeadd(time, -86400.0);
		else sol->time = time;
		pos2ecef(pos, sol->rr);
		sol->stat = 0 <= solq && solq <= 8 ? solq_nmea[solq] : SOLQ_NONE;
		sol->ns = nrcv;

		sol->type = 0; /* postion type = xyz */

		trace(5, "decode_nmeagga: %s rr=%.3f %.3f %.3f stat=%d ns=%d hdop=%.1f ua=%c um=%c\n",
			time_str(sol->time, 0), sol->rr[0], sol->rr[1], sol->rr[2], sol->stat, sol->ns,
			hdop, ua, um);

		return 1;
	}
	/* test nmea -----------------------------------------------------------------*/
	int test_nmea(const char *buff)
	{
		if (strlen(buff) < 6 || buff[0] != '$') return 0;
		return !strncmp(buff + 1, "GP", 2) || !strncmp(buff + 1, "GA", 2) ||
			!strncmp(buff + 1, "GL", 2) || !strncmp(buff + 1, "GN", 2) ||
			!strncmp(buff + 1, "GB", 2) || !strncmp(buff + 1, "BD", 2) ||
			!strncmp(buff + 1, "QZ", 2);
	}
	/* test solution status ------------------------------------------------------*/
	int test_solstat(const char *buff)
	{
		if (strlen(buff) < 7 || buff[0] != '$') return 0;
		return !strncmp(buff + 1, "POS", 3) || !strncmp(buff + 1, "VELACC", 6) ||
			!strncmp(buff + 1, "CLK", 3) || !strncmp(buff + 1, "ION", 3) ||
			!strncmp(buff + 1, "TROP", 4) || !strncmp(buff + 1, "HWBIAS", 6) ||
			!strncmp(buff + 1, "TRPG", 4) || !strncmp(buff + 1, "AMB", 3) ||
			!strncmp(buff + 1, "SAT", 3);
	}
	/* decode nmea ---------------------------------------------------------------*/
	int decode_nmea(char *buff, sol_t *sol)
	{
		char *p, *q, *val[MAXFIELD];
		int n = 0;

		trace(4, "decode_nmea: buff=%s\n", buff);

		/* parse fields */
		for (p = buff; *p&&n < MAXFIELD; p = q + 1) {
			if ((q = strchr(p, ',')) || (q = strchr(p, '*'))) {
				val[n++] = p; *q = '\0';
			}
			else break;
		}
		if (!strcmp(val[0] + 3, "RMC")) { /* $xxRMC */
			return decode_nmearmc(val + 1, n - 1, sol);
		}
		else if (!strcmp(val[0] + 3, "ZDA")) { /* $xxZDA */
			return decode_nmeazda(val + 1, n - 1, sol);
		}
		else if (!strcmp(val[0] + 3, "GGA")) { /* $xxGGA */
			return decode_nmeagga(val + 1, n - 1, sol);
		}
		return 0;
	}
	
}
