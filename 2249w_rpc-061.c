/* SLOT NUMBER of ADC/TDC */
/* ADC = 2249W(LeCroy), TDC = RPC-061(REPIC) */
#define ADC_STATION 15
#define TDC_STATION 21
int ch[]={0, 2};	// channel number list
#define NCH (sizeof(ch)/sizeof(ch[0]))

/**** Edited by Takashi Kanai ****/
/**** Last update at 2005/04/01 ****/
/*** Modified at 2008/05/22 by Yosuke Maeda ***/
/*** Modified at 2010/08/04 by Yasuyuki Furuichi ***/
/*** Modified at 2018/11/27 by Kensuke Yamamoto ***/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/errno.h>
#include "camlib.h"

#define READ 0
#define READCL 2
#define LAM 8
#define CLR 9
#define WRITE 16
#define ENLAM 26

int main(int argc, char *argv[]){
	FILE *fp;
	int i, prn;
	int adc_data[NCH],tdc_data[NCH];
	int nloop, iloop;
	int dummy, q=0, x=0;
	char fname[1000];


	if(argc != 3){
		printf("usage:\n");
		printf("  %s (number of events) (filename)\n", argv[0]);
		return 1;
	}
	sprintf(fname,"../data/%s",argv[2]);
  
	nloop = atoi(argv[1]);
	if(!(fp = fopen(fname,"w"))){
		printf("File: %s open error!!\n", fname);
		return 1;
	}
	printf("START DATA TAKING --- %d events ---\n",nloop);

	// fprintf(fp, "#ofCHs\t%d\n",NCH);

	/* init camac */
	if(CAMOPN()){
		printf("CAMAC OPEN ERROR!\n");
		return 1;
	}
	CSETCR(1);
	CGENZ();
	fprintf(stderr,"### CAMAC initialized.\n");

	CGENC();
	CAMAC(NAF(ADC_STATION,0,CLR), &dummy, &q, &x);	// Clear it ADC
	CAMAC(NAF(ADC_STATION,0,ENLAM), &dummy, &q, &x);	// Enable LAM of ADC
	CAMAC(NAF(TDC_STATION,0,CLR), &dummy, &q, &x);	// Clear it TDC
	CAMAC(NAF(TDC_STATION,0,ENLAM), &dummy, &q, &x);	// Enable LAM of TDC
	fprintf(stderr,"### START DAQ process.\n");

  
	for(iloop = 0;iloop < nloop;iloop++){
		q=0;
		while(!q){
			CAMAC(NAF(ADC_STATION,0,LAM),&dummy,&q,&x);	// LAM of ADC
			CAMAC(NAF(TDC_STATION,0,LAM),&dummy,&q,&x);	// LAM of TDC
		}
		for(i=0;i < NCH;i++){
			CAMAC(NAF(ADC_STATION,ch[i],READ), &adc_data[i], &q, &x);	// Read ADC data
			CAMAC(NAF(TDC_STATION,ch[i],READ), &tdc_data[i], &q, &x);	// Read TDC data
		}
		CAMAC(NAF(ADC_STATION,0,CLR), &dummy, &q, &x);	// Clear ADC
		CAMAC(NAF(TDC_STATION,0,CLR), &dummy, &q, &x);	// Clear TDC

		prn = 0;
		for(i=0;i<NCH;i++){
			if(adc_data[i] != 4095 && tdc_data[i] != 4095){
				prn++;
			}
		}
		if(prn == NCH){
			printf("\n");
			for(i=0;i<NCH;i++){
				printf("ADC Ch%d value %5d / TDC Ch%d value %5d / ",ch[i],adc_data[i],ch[i],tdc_data[i]);
				fprintf(fp,"%7d %7d ",adc_data[i],tdc_data[i]);	// Write data
			}
			printf("(%d/%d)", iloop, nloop);
			fprintf(fp, "\n");
			fflush(fp);
		}
		else{
			printf("*");
		}
		fflush(stdout);
	}

	CGENZ();
	CSETI();
	CAM_Close();
	fclose(fp);

	printf("---------------------------\n");
	printf("### DAQ ended successfully.\n");
	return 0;
}
