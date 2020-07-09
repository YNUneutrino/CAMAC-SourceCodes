/* ADC C009(HOSHIN) Operation Source Code */
#define STATION 16	// Station Number
int ch[]={0}; // Channel Number List
#define NCH (sizeof(ch)/sizeof(ch[0]))

/**** Edited by Takashi Kanai ****/
/**** Last update at 2005/04/01 ****/
/*** Modified at 2008/05/22 by Yosuke Maeda ***/
/*** Modified at 2010/08/04 by Yasuyuki Furuichi ***/
/*** Modified at 2019/03/15 by Kensuke Yamamoto ***/
/*** Test at 2020/07/09 by Sano & Wada ***/

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

int main(int argc, char *argv[]){
	FILE *fp;
	int i, prn;
	int data[NCH]={0};
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

	for(i=0;i<NCH;i++){
		fprintf(fp, "CH%3d ", ch[i]);
	}
	fprintf(fp, "\n");


	/* Initialize CAMAC */
	if(CAMOPN()){
		printf("CAMAC OPEN ERROR!\n");
		return 1;
	}

	CSETCR(1);	// Crate Number
	CGENZ();
	fprintf(stderr,"### CAMAC initialized.\n");

	CGENC();
	CAMAC(NAF(STATION, 0, CLR), &dummy, &q, &x); // Clear ADC
	fprintf(stderr,"### START DAQ process.\n");

	for(iloop = 0;iloop < nloop;iloop++){
		q=0;
		while(q==0){
			CAMAC(NAF(STATION, 0, LAM), &dummy, &q, &x);
		}

		for(i=0;i<NCH;i++){
			CAMAC(NAF(STATION, ch[i], READ), &data[i], &q, &x);
		}
		CAMAC(NAF(STATION, 0, CLR), &dummy, &q, &x);

		prn = 0;
		for(i=0;i<NCH;i++){
			if(data[i] != 4095){
				prn++;
			}
		}
		if(prn == NCH){
			printf("\n");
			for(i=0;i<NCH;i++){
				printf("Ch%d value %5d / ", ch[i], data[i]);
				fprintf(fp,"%5d ",data[i]);
			}
			printf("(%d/%d)", iloop+1, nloop);
			fprintf(fp, "\n");
			fflush(fp);
		}
		else{
			printf("Saturated ADC counts :: Skip\n");
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
