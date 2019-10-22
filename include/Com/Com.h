/*
 * Com.h
 *
 *  Created on: 2018/2/12
 *      Author: doublestring
 */

#ifndef INCLUDE_COM_COM_H_
#define INCLUDE_COM_COM_H_

#include <iostream>
#include <vector>
#include "../Rtklib/rtklib.h"
using namespace std;
namespace bamboo {
#define MAXFIELD 64
	class GPSEPH;
	extern int md_julday(int, int, int);

	extern void fillobs(char* line, int nobs, int itemlen, double ver);

	extern void filleph(char* line, double ver);

	extern int pointer_string(int, string[], string);

	extern int pointer_charstr(int row, int col, const char* string_array, const char* string);

	extern int skt_openTcp(string ip, int port);

	extern int skt_closeTcp(int sock);

	extern void skt_read(int& sock, char* one_obs, int* bytesRecv, string ip, int port);

	extern void skt_read_obs_debug(int& sock, char* one_obs, int* bytesRecv, string ip, int port);

	extern void skt_read_brdm_debug(int& sock, char* one_obs, int* bytesRecv, string ip, int port);

	void skt_write_file(char* buff);

	extern void skt_read_file(int& sock, char* one_obs, int* bytesRecv, string ip, int port);

	extern int index_string(const char* src, char key);

	char* substringEx(char* dest, const char* src, int start, int length);

	extern char* left_justify_string(char* string);

	extern int len_trim(const char* pStr);

	extern char* trim(char* pStr);

	extern void xyzblh(double* x, double scale, double a0, double b0, double dx, double dy,
		double dz, double* geod);

	void blhxyz(double* geod, double a0, double b0, double* x);

	extern void rot_enu2xyz(double lat, double lon, double(*rotmat)[3]);

	extern void m_wtclkhd(const char* filename, int mjd, double intv);

	extern void oceanload_coef(double lat, double lon, double(*olc)[11]);

	void brdtime(char* cprn, int *mjd, double *sod);

	extern void matmpy(double* A, double* B, double* C, int row, int colA, int colB);

	extern void transpose(double* in, double* out, int row, int col);

	extern void cross(double* v1, double*v2, double* vout);

	extern void unit_vector(int n, double* v, double* u, double* length);

	extern double dot(int n, double* v1, double* v2);

	extern double distance(int n, double *v1, double* v2);

	extern void betau(double* xsat, double* xsun, double* beta, double* u);

	extern void phase_windup_itrs(int* lfirst, double(*rot_f2j)[3], double(*rot_l2f)[3],
		double* xbf, double* ybf, double* zbf, double* xrec2sat, double* dphi0,
		double* dphi);

	extern void phase_windup(int* lfirst, double(*rot_f2j)[3], double(*rot_l2f)[3],
		double* xbf, double* ybf, double* zbf, double* xrec2sat, double* dphi0,
		double* dphi);

	extern void split_string(bool lnoempty, char* string, char c_start, char c_end,
		char seperator, int* nword, char* keys, int len);

	extern void gpsweek(int year, int month, int day, int* week, int* wd);

	extern void mjd2doy(int jd, int* iyear, int* idoy);

	extern void cur_time(int* mjd, double* sod);

	extern void timinc(int jd, double sec, double delt, int* jd1, double* sec1);

	extern char* runtime(char* buf);

	extern char* runlocaltime(char* buf);

	extern string runlocaltime_str();

	extern char* run_timefmt(int mjd, double sod, char* buf);

	extern void mjd2date(int jd, double sod, int* iyear, int* imonth, int* iday, int* ih,
		int* imin, double* sec);

	extern void yeardoy2monthday(int iyear, int idoy, int* imonth, int* iday);

	void findkey(ifstream& in, const char* keyword, const char* sinex_bracket, char* value);

	extern char* toLower(char* value);

	extern char* toUpper(char* value);

	int sum(int array[], int ndim);

	void wksow2mjd(int week, double sow, int* mjd, double* sod);

	void time2mjd(time_t time, int* mjd, double* sod);

	time_t mjd2time(int mjd, double sod);

	void mjd2wksow(int mjd, double sod, int *week, double *sow);

	void yr2year(int& yr);

	double timdif(int jd2, double sod2, int jd1, double sod1);

	int all(int* vx, int n, int value);

	int linit(int* ldat, char csys, int value);

	bool chos(int ndel, int imax, int* idel);

	int ambslv(int ncad, double* q22, double* bias, double* disall);

	int ambslv_(int ncad, double* q22, double* bias, double* bias_sec, double* disall);

	int pointer_int(int n, int* array, int i);

	long double GetX(double RadLat, double eSquare, double a);

	bool bl2Gaussxy(double RadLat, double RadLon, double *x, double *y,
		double RadLon0, double eSquare, double a, double heightchange);

	unsigned long CRC24(long size, const unsigned char *buf);

	unsigned int getbdsiode(GPSEPH& bdseph);

	int genAode(char csys, int mjd, double sod, double toe, int inade, GPSEPH* eph);

	void bdsCodeCorbyElv(int Prn, double elvRad, double* codeCor);

	int getNoZeroCount(double* xl, int n);

	void excludeAnnoValue(char* value, const char* in);

	string zipJson(string json);


	void CHI2(int N, double CH, double* P, double* D);

	void NORMAL(double U, double* P);

	void PNORMAL(double Q, double* U);

	void pchi2(int N, double Q, int L, double* XX);

	bool chitst(int flag, int ndof, double osig, double nsig, double zone);

	void testVRS();

	int decode_nmea(char *buff, sol_t *sol);

	int test_solstat(const char *buff);

	int test_nmea(const char *buff);

	int decode_nmeagga(char **val, int n, sol_t *sol);

	int decode_nmeazda(char **val, int n, sol_t *sol);

	int decode_nmearmc(char **val, int n, sol_t *sol);

	double dmm2deg(double dmm);

	void septime(double t, double *t1, double *t2, double *t3);

}
#endif /* INCLUDE_COM_COM_H_ */
