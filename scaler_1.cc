/* camac scaler KC3122 */
/* 2009.11.4 */

//#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "camlib.h"



//#define lam         0x11     /*     10001    */   
#define lam         0x10     /*     000010     */
#define SCsn	22	//station number
#define SCch	4	//number of scaler channel
#define Nch	4	//number of readout chanel

void CAMACinit(void);
void CAMACterm(void);
double gettimeofday_sec(void);

int main(int argc, char *argv[]){
  //checking argument
  if(argc != 2){
  	printf("usage : ./scaler [mesurement time(sec)\n");
// 	std::cout << "usage : ./scaler [mesurement time(sec)]" << std::endl;
	exit(1);
  }
//  std::cerr << "scaler counting for " << argv[1] << " sec!" << std::endl;
  	printf("scaler counting for %s  sec! \n", argv[1]);
  //initialize camac & scaler
  CAMACinit();
  int dat,q,x;
  CAMAC(NAF(SCsn,0,9),&dat,&q,&x);

  //counting start
//  std::cerr << "counting start!" << std::endl;
	printf("counting start!\n");
  double time=0.0,ts,te; int cnt =0,chdat[Nch];
  ts = gettimeofday_sec();  //starting timer 
  CAMAC(NAF(SCsn,0,9),&dat,&q,&x);	//clear
  
  //waiting loop
  while(1){
	cnt++;	//buffer loop
	if(cnt > 10000){
		te = gettimeofday_sec();
		time = te - ts;
		cnt=0;
	}
	if(time > atof(argv[1]) )break;
  }
  
  //data read
  for(int i=0;i < Nch;i++){
	CAMAC(NAF(SCsn,i,2),&chdat[i],&q,&x);
//	std::cout << i << "ch count  " << chdat[i] << std::endl;
	printf(" %d ch count %d \n",i,chdat[i]);

  }
  
  //terminate camac  
  CAMACterm();
  
  return 0;
}

//camac initialize
void CAMACinit(void){
//	std::cerr << "CAMAC initialize...";
	printf("CAMAC initialize...");
	COPEN();
	CSETI();
	CGENZ();
	CGENC();
	printf("finished.\n");
//	std::cerr << "finished." << std::endl;
}

//camac termination
void CAMACterm(void){
//	std::cerr << "CAMAC termination...";
	printf("CAMAC termination...");
	CGENZ();
	CGENC();
	CCLOSE();
//	std::cerr << "finished." << std::endl;
	printf("finished.\n");

}

double gettimeofday_sec(void){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

