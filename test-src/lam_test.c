/* lam_test.c */
/* Created by Enomoto Sanshiro on 23 July 1999. */
/* Last updated by Enomoto Sanshiro on 23 July 1999. */
/* Edited by Kensuke Yamamoto on 18 December 2018. */


#include <stdio.h>
#include <unistd.h>
#include "camlib.h"



/***** LeCroy 2249W *****/

int main(void)
{
    int n = 15;		// channel number
    int data, dummy, q, x;

    int timeout = 3;  // sec
    int result;
    int i;

    COPEN();

    CSETCR(1);	 // set crate number

    CGENZ();
    
    for (i = 0; i < 16; i++) {
	CAMAC(NAF(n, 0, 26), &dummy, &q, &x);
        printf("Waiting LAM ...");
        result = CWLAM(timeout);

        if (result == 0) {
	    printf("OK.\n");
	}
	else {
	    printf("timed out.\n");
	}	

        CAMAC(NAF(n, 0, 8), &dummy, &q, &x);
        printf("F=8(LAM), Q=%d, X=%d\n", q, x);
        CAMAC(NAF(n, 0, 0), &data, &q, &x);
        printf("F=0(READ_DATA), DATA=%d, Q=%d, X=%d\n", data, q, x);
	CAMAC(NAF(n, 0, 9), &dummy, &q, &x);
	CAMAC(NAF(n, 0, 10), &dummy, &q, &x);
	printf("F=10(CLEAR_LAM), Q=%d, X=%d\n", q, x);

	printf("\n");
    }

    CCLOSE();

    return 0;
}


/***** HOSHIN C009 *****/
/*
int main(void)
{
    int n = 17;		// channel number
    int data, dummy, q, x;

    int timeout = 3;  // sec
    int result;
    int i;

    COPEN();

    CSETCR(1);	 // set crate number

    CGENZ();
    
    CAMAC(NAF(n, 0, 9), &dummy, &q, &x);
    printf("F=9(CLEAR), Q=%d, X=%d\n", q, x);

    for (i = 0; i < 16; i++) {
        printf("Waiting LAM ...");
        result = CWLAM(timeout);

        if (result == 0) {
	    printf("OK.\n");
	 }
	 else {
	    printf("timed out.\n");
	 }	

        printf("F=8(LAM), Q=%d, X=%d\n", q, x);
	CAMAC(NAF(n, 0, 0), &data, &q, &x);
        printf("F=0(READ_DATA), DATA=%d, Q=%d, X=%d\n", data, q, x);

	usleep(1000);
	CAMAC(NAF(n, 0, 9), &dummy, &q, &x);
	printf("F=9(CLEAR), Q=%d, X=%d\n", q, x);

	printf("\n");
    }

    CCLOSE();

    return 0;
}*/


/***** REPIC RPC-061 *****/
/*
int main(void)
{
    int n = 21;		// channel number
    int data, dummy, q, x;

    int timeout = 3;  // sec
    int result;
    int i;

    COPEN();

    CSETCR(1);	// set crate number 

    CGENZ();
    
    for (i = 0; i < 16; i++) {
	CAMAC(NAF(n, 0, 26), &dummy, &q, &x);
        printf("Waiting LAM ...");
        result = CWLAM(timeout);

        if (result == 0) {
	    printf("OK.\n");
	}
	else {
	    printf("timed out.\n");
	}	

        CAMAC(NAF(n, 0, 8), &dummy, &q, &x);
        printf("F=8(LAM), Q=%d, X=%d\n", q, x);
        CAMAC(NAF(n, 0, 0), &data, &q, &x);
        printf("F=0(READ_DATA), DATA=%d, Q=%d, X=%d\n", data, q, x);
	CAMAC(NAF(n, 0, 9), &dummy, &q, &x);
	CAMAC(NAF(n, 0, 10), &dummy, &q, &x);
	printf("F=10(CLEAR_LAM), Q=%d, X=%d\n", q, x);

	printf("\n");
    }

    CCLOSE();

    return 0;
}*/

/***** Technoland C-TS105 *****/

/*
void TDC_INIT(void){
	int n=19;	// Station Number
	int wdata, A, F;
	int dummy=0;
	int q=0;
	int x=0;

	A=0; F=17; wdata=0xab;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=0; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0x620;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0x62;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0x2004;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0x6;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=2; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=3; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0x200;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=4; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=5; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0x800;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=6; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0x1fb4;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=7; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0x7ff;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=11; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=12; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=14; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=17; wdata=0x1;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=1; F=17; wdata=0x240;
	CAMAC(NAF(n,A,F), &wdata, &q, &x);
	usleep(1000);
	A=4; F=18;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
	A=0; F=9;
	CAMAC(NAF(n,A,F), &dummy, &q, &x);
	usleep(1000);
}

int main(void){
	int n = 19;	// channel number
	int data, wdata, dummy, q, x, hitchs, Nofhits;

	int timeout = 3;	// sec
	int result;
	int i;

	COPEN();

	CSETCR(1);	// set crate number 

	CGENZ();

	// write registers (C-TS105 setup)
	dummy=0;
	q=0;
	x=0;


	for(i = 0; i < 16; i++) {
		TDC_INIT();
		
		wdata=5;
		CAMAC(NAF(n, 0, 16), &wdata, &q, &x);	// set maximum measurement time
		CAMAC(NAF(n, 0, 26), &dummy, &q, &x);	// enable LAM
		CAMAC(NAF(n, 0, 25), &dummy, &q, &x);	// start a measurement

		printf("Waiting LAM ...");
		result = CWLAM(timeout);

		if (result == 0) {
			printf("OK.\n");
		}
		else {
			printf("timed out.\n");
		}	

		CAMAC(NAF(n, 0, 1), &data, &q, &x);	// hit ch
		hitchs = 0xffffff - data;
		printf("F=1, HITCH=%d, Q=%d, X=%d\n", hitchs, q, x);

		CAMAC(NAF(n, 0, 2), &data, &q, &x);	// # of hits
		Nofhits = data - 0xff0000;
		printf("F=2, # of hits=%d, Q=%d, X=%d\n", Nofhits, q, x);

		CAMAC(NAF(n, 0, 8), &dummy, &q, &x);	// LAM
		printf("F=8(LAM), Q=%d, X=%d\n", q, x);

		CAMAC(NAF(n, 0, 0), &data, &q, &x);	// read data
		data = data & 0x7fffff;
		printf("F=0(READ_DATA), DATA=%d, Q=%d, X=%d\n", data, q, x);

		usleep(1000);
		CAMAC(NAF(n, 0, 9), &dummy, &q, &x);	// clear
		printf("F=9, Q=%d, X=%d\n", q, x);

		printf("\n");
	 
	}

	CCLOSE();

	return 0;
}*/
