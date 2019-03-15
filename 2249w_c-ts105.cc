/* ADC 2249W(LeCroy) and TDC C-TS105(Technoland) Operation Source Code */
/***** 2019/03/15 Kensuke Yamamoto *****/

extern "C" {
#include "camlib.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/errno.h>

using namespace std;

#define DEBUG

/***** ADC setting *****/
const int ADC_STATION = 15;	// ADC Station Number
const int ADCChannels[] = {0, 2};	// ADC Channel Number List
const int NCHADC=(sizeof(ADCChannels)/sizeof(int));

/***** TDC setting *****/
const int TDC_STATION = 17;	// TDC Station Number
const int TDCChannels[] = {0, 2};	// TDC Channel Number List
const int NCHTDC=(sizeof(TDCChannels)/sizeof(int));

/***** CAMAC functions *****/
const int READ_DATA = 0;
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
	int dummy=0, wdata=0, q=0, x=0, i, j, prn;
	char filename[100];

	/**** Check arguments ****/
	if( argc != 3 ) {
		cerr << "Arguments are not correct" << endl;
		Usage();
		return 1;
	}

	/**** Arguments setting ****/
	nloop = atol(argv[1]);
	sprintf(filename,"../data/%s",argv[2]);
	ofstream outputfile( filename );
	int adcdata[NCHADC], tdcdata[NCHTDC];
	for( i=0; i<NCHADC; i++ ){
		adcdata[i] = 0;
	}
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

	for(i=0;i<NCHADC;i++){
		outputfile << "ADCch" << ADCChannels[i] << "\t";
	}
	outputfile << "TDCch\tTDC cts..." << endl;


	while( iloop < nloop ){
		cerr << "TRIG:" << iloop+1 << "\t";

		/***** ADC and TDC start *****/

		Init_TDC();
		wdata=1;	// Maximum measurement time (us) (decimal number)
		CAMAC(NAF(TDC_STATION, 0, SET_MAX), &wdata, &q, &x);	// Set TDC measurement time
		CAMAC(NAF(ADC_STATION, 0, EN_LAM), &dummy, &q, &x);	// Enable ADC LAM
		CAMAC(NAF(TDC_STATION, 0, EN_LAM), &dummy, &q, &x);	// Enable TDC LAM
		CAMAC(NAF(ADC_STATION, 0, CLR), &dummy, &q, &x); // Clear ADC
		CAMAC(NAF(TDC_STATION, 0, TDC_START), &dummy, &q, &x);	// Start measurement

		/***** Waiting LAM *****/
		int qa = 0;
		int qt = 0;
		cerr << "\tWaiting LAM ......" << flush;
		while( qa==0 || qt==0 ){
			CAMAC(NAF(ADC_STATION, 0, LAM), &dummy, &qa, &x);
			CAMAC(NAF(TDC_STATION, 0, LAM), &dummy, &qt, &x);
			usleep(1);
		}
		cerr << "fire!!\t" << flush;

		/***** Read ADC Data *****/
		for(i=0;i<NCHADC;i++){
			CAMAC(NAF(ADC_STATION, ADCChannels[i], READ_DATA), &adcdata[i], &q, &x);
		}
		prn = 0;
		for(i=0;i<NCHADC;i++){
			if(adcdata[i] != 4095){
				prn++;
			}
		}
		if(prn == NCHADC){

			/***** Check HIT channel ******/
			int hitchs, hitchs_org;
			int hitch[8]={0};
			wdata = 0;
			CAMAC(NAF(TDC_STATION, 0, READ_HIT), &wdata, &q, &x);
			hitchs = 0xffffff - wdata;
			hitchs_org = hitchs;
#ifdef DEBUG
			hitch[0]=hitchs%2;
			cerr << "HIT CH: CH0<----->CH7 : " << hitch[0];
			for(i=1;i<8;i++){
				hitchs = (hitchs-hitch[i-1])/2;
				hitch[i] = hitchs%2;
				cerr << hitch[i];
			}
			cerr << endl;
#endif
			if(!hitchs_org){
#ifdef DEBUG
				cerr << "\tTDC NO hits :: Skip " << endl;
#endif
			}

			/***** Single Hit Mode *****/
			else{
				cerr << "\t";
				// Check # of hits and read TDC data
				int readdata=0;
				int Nofhits[NCHTDC], tdchits[NCHTDC];
				for(i=0;i<NCHTDC;i++){
					tdchits[i] = 0;
					CAMAC(NAF(TDC_STATION, TDCChannels[i], N_HITS), &tdchits[i], &q, &x);
					Nofhits[i] = tdchits[i] - 0xff0000;
					if(Nofhits[i] != 1){
						readdata++;
					}
				}

				if(readdata==0){
					for(i=0;i<NCHADC;i++){
			    	printf("Ch%d value %5d / ", ADCChannels[i], adcdata[i]);
			     	outputfile << adcdata[i] << "\t";
				}
					for(i=0;i<NCHTDC;i++){
						CAMAC(NAF(TDC_STATION, TDCChannels[i], READ_DATA), &tdcdata[i], &q, &x);
						printf("TDC Ch%d value %5d / ",TDCChannels[i],tdcdata[i]);
						outputfile << TDCChannels[i] << "\t" << tdcdata[i] << "\t";
					}
					outputfile << endl;
				}

				else{
					cerr << "TDC NO hits or TDC multi hits :: skip ";
				}

				printf("(%ld/%ld)", iloop+1, nloop);
				cerr << endl;
				for(i=0;i<NCHTDC;i++){
					printf("\tCH%d: %d hits!\t", TDCChannels[i], Nofhits[i]);
				}
			}


			/***** Multi Hits Mode *****/
/*			else{
				cerr << "\t";
				// check # of hits and read data
				int Nofhits[NCHTDC], tdchits[NCHTDC];

				for(i=0;i<NCHADC;i++){
					printf("Ch%d value %5d / ", ADCChannels[i], adcdata[i]);
					outputfile << adcdata[i] << "\t";
				}

				for(i=0;i<NCHTDC;i++){
					tdchits[i] = 0;
					CAMAC(NAF(TDC_STATION, TDCChannels[i], N_HITS), &tdchits[i], &q, &x);
					Nofhits[i] = tdchits[i] - 0xff0000;
					for(j=0;j<Nofhits[i];j++){
						CAMAC(NAF(TDC_STATION, TDCChannels[i], READ_DATA), &tdcdata[i], &q, &x);
						printf("TDC Ch%d value %5d / ", TDCChannels[i], tdcdata[i]);
						outputfile << TDCChannels[i] << "\t" << tdcdata[i] << "\t";
					}
				}
				printf("(%ld/%ld)\n", iloop+1, nloop);
				cerr << endl;
				for(i=0;i<NCHTDC;i++){
					printf("\tCH%d: %d hits!\t", TDCChannels[i], Nofhits[i]);
				}
				outputfile << endl;
			}*/

		}
		else{
			cerr << "Saturated ADC counts :: Skip" << endl;
		}

		iloop ++;
		CAMAC(NAF(ADC_STATION, 0, CLR), &dummy, &q, &x);
		CAMAC(NAF(TDC_STATION, 0, CLR), &dummy, &q, &x);
		cerr << endl;
	}

	/**** close CAMAC ****/
	CGENZ();
	CSETI();
	CAM_Close();

	outputfile.close();

	cerr << endl;
	cerr << "---------------------------" << endl;
	cerr << "### DAQ ended successfully." << endl;
	return 0;
}

void Usage( void ){
	cerr << "\n/**********  Usage ****************/\n" << endl;
	cerr << "./2249w_c-ts105 (number of events) (filename)  \n " << endl;
	cerr << "/*****************************************/" << endl;
}

int Init_CAMAC( void ){

	if(COPEN()) {
		cerr << "Camac : not opened." << endl;
		return 1;
	}

	//-------------------------------------------------------------

	if(CSETCR(1)) {	// Crate Number
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

	// Write registers
	A=0; F=LOAD_REG; wdata=0xab;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=0; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << "C-TS105 setting ... 1" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0x620;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0x62;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 2" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0x2004;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0x6;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=2; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 3" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=3; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 4" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0x200;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=4; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 5" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=5; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 6" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0x800;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=6; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 7" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0x1fb4;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=7; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 8" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0x7ff;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=11; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 9" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=12; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 10" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=14; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 11" << flush;
	usleep(1);
	A=0; F=LOAD_REG; wdata=0x1;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=1; F=LOAD_REG; wdata=0x240;
	CAMAC(NAF(N,A,F), &wdata, &q, &x);
	usleep(1);
	A=4; F=WRITE;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	usleep(1);
	cerr << ", 12" << flush;
	A=0; F=CLR;
	CAMAC(NAF(N,A,F), &dummy, &q, &x);
	cerr << ", 13 done." << endl;
	usleep(1);
}
