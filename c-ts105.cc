/***** TDC C-TS105(Technoland) *****/
/***** 2012/06/09 Hajime Kubo *****/
/***** 2018/12/19 Kensuke Yamamoto *****/

extern "C" {
#include "camlib.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

using namespace std;

#define DEBUG

/***** TDC setting *****/
const int TDC_STATION = 19;	// Station number
const int TDCChannels[] = {0, 2};	// Channel number
const int NCHTDC=(sizeof(TDCChannels)/sizeof(int));

/***** CAMAC function *****/
const int READ_TDC = 0;
const int READ_HIT = 1;
const int N_HITS = 2;
const int LAM = 8;
const int CLR = 9;
const int SET_MAX = 16;
const int LOAD_REG = 17;
const int WRITE = 18;
const int DIS_LAM = 24;
const int TDC_START = 25;
const int EN_LAM = 26;

void Usage( void );
int Init_CAMAC( void );
void Init_TDC( void );

int main(int argc, char *argv[]){

	long nloop, iloop=0;
	int dummy=0, wdata=0, q=0, x=0, i, j;
	char filename[100];

	/**** check arguments ****/
	if( argc != 3 ) {
		cerr << "Arguments are not correct" << endl;
		Usage();
		return 1;
	}

	/**** Arguments setting ****/
	nloop = atol(argv[1]);
	sprintf(filename,"../data/%s",argv[2]);
	ofstream outputfile( filename );
	int tdcdata[NCHTDC];
	for( i=0; i<NCHTDC; i++ ){
		tdcdata[i] = 0;
	}

	if( Init_CAMAC() ){
		cerr << "CAMAC initialization failed" << endl;
		return 1;
	}
	else{
		cerr << "CAMAC initialization done." << endl;
	}

	cerr << "START DATA TAKING --- " << nloop << " events ---" << endl;

//	outputfile << "#ofCHs\t" << NCHTDC << endl;
//	outputfile << "#ofCHs\t" << NCHADC + NCHTDC << endl;

	while( iloop < nloop ){
		cerr << "TRIG:" << iloop+1 << "\t";

		/***** TDC start *****/

		Init_TDC();
		wdata=8;
		CAMAC(NAF(TDC_STATION, 0, SET_MAX), &wdata, &q, &x);
		CAMAC(NAF(TDC_STATION, 0, EN_LAM), &dummy, &q, &x);
		CAMAC(NAF(TDC_STATION, 0, TDC_START), &dummy, &q, &x);

		/***** waiting LAM *****/
		q=0;
		cerr << "\tWaiting LAM ......" << flush;
		while( !q ){
			CAMAC(NAF(TDC_STATION, 0, LAM), &dummy, &q, &x);
			usleep(5);
		}
		cerr << "fire!!" << flush;

		/***** check HIT channel ******/
		int hitchs;
		wdata = 0;
		CAMAC(NAF(TDC_STATION, 0, READ_HIT), &wdata, &q, &x);
		hitchs = 0xffffff - wdata;
#ifdef DEBUG
		// cerr << "  HIT CHs = " << hex << hitchs << dec << endl;
		cerr << "  HIT CHs = 0x" << hex << wdata << dec << endl;
#endif

		if(!hitchs){
#ifdef DEBUG
			cerr << "\tNO hits :: skip " << endl;
#endif
		}
		else{
			cerr << "\t";
			/***** check # of hits and read data *****/
			int Nofhits[NCHTDC], tdchits[NCHTDC];
			for(i=0;i<NCHTDC;i++){
				tdchits[i] = 0;
				CAMAC(NAF(TDC_STATION, TDCChannels[i], N_HITS), &tdchits[i], &q, &x);
				Nofhits[i] = tdchits[i] - 0xff0000;
				for(j=0;j<Nofhits[i];j++){
					CAMAC(NAF(TDC_STATION, TDCChannels[i], READ_TDC), &tdcdata[i], &q, &x);
					printf("TDC Ch%d value %7d / ",TDCChannels[i],tdcdata[i]);
					outputfile << tdcdata[i] << "\t";
				}
			}
			printf("(%ld/%ld)", iloop+1, nloop);
			cerr << endl;
			for(i=0;i<NCHTDC;i++){
				cerr << "\t" << Nofhits[i] << " hits!\t";
			}
			outputfile << endl;
		}
		iloop ++;
		CAMAC(NAF(TDC_STATION, 0, CLR), &dummy, &q, &x);

		cerr << endl;
	}

	/**** close CAMAC ****/
	CGENZ();
	CSETI();
	CAM_Close();

	cerr << endl;
	cerr << "---------------------------" << endl;
	cerr << "### DAQ ended successfully." << endl;
	return 0;
}

void Usage( void ){
	cerr << "\n/**********  Usage ****************/\n" << endl;
	cerr << "./c-ts105 (number of events) (filename)  \n " << endl;
	cerr << "/*****************************************/" << endl;
}

int Init_CAMAC( void ){

	if(COPEN()) {
		cerr << "Camac : not opened." << endl;
		return 1;
	}

	//-------------------------------------------------------------

	if(CSETCR(1)) {
		cerr << "Camac : no crate #0." << endl;
		return 1;
	}

	//-------------------------------------------------------------

	if(CGENZ()) {
		cerr << "Camac : not initialized." << endl;
		return 1;
	}

	//-------------------------------------------------------------

	if(CGENC()) {
		cerr << "Camac : not cleared." << endl;
		return 1;
	}

	//-------------------------------------------------------------

	if(CREMI()) {
		cerr << "Camac : inhibit is not cleared." << endl;
		return 1;
	}

	//-------------------------------------------------------------

	return 0;
}

void Init_TDC(void){

	int N = TDC_STATION;
	int A,F;
	int dummy=0, wdata=0, q=0, x=0;

	/***** C-TS105 setup *****/

	// write registers
	A=0; F=LOAD_REG; wdata=0xab;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=0; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << "C-TS105 setting ... 1" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0x620;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0x62;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 2" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0x2004;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0x6;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=2; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 3" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=3; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 4" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0x200;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=4; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 5" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=5; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 6" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0x800;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=6; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 7" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0x1fb4;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=7; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 8" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0x7ff;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=11; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 9" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=12; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 10" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=14; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 11" << flush;
	usleep(10);
	A=0; F=LOAD_REG; wdata=0x1;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=1; F=LOAD_REG; wdata=0x240;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(10);
	A=4; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	usleep(10);
	cerr << ", 12" << flush;
	A=0; F=CLR;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 13 done." << endl;
	usleep(10);
}
