/* testlam_test.c */
/* Created by Enomoto Sanshiro on 13 June 2002. */
/* Last updated by Enomoto Sanshiro on 13 July 2002. */
/* Edited by Kensuke Yamamoto on 22 November 2018. */


#include <stdio.h>
#include <unistd.h>
#include "camlib.h"


static const int station_number = 17;	/* station number */


int main(void)
{
    int address, function, data, q, x;

    COPEN();
    CGENZ();
    
    /* enable */
    function = 26;
    address = 0;
    CAMAC(NAF(station_number, address, function), &data, &q, &x);

    while (1) {
        /* test lam */
        function = 8;
        address = 0;
        CAMAC(NAF(station_number, address, function), &data, &q, &x);

	printf("%d-", q);
        fflush(stdout);

        /* clear */
        function = 9;  
        address = 0;
        CAMAC(NAF(station_number, address, function), &data, &q, &x);	

        /* 200ms sleep */
        usleep(200000);
    }

    CCLOSE();

    return 0;
}
