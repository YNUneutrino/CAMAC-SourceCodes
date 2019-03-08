/* inhibit_test.c */
/* Created by Enomoto Sanshiro on 23 July 1999. */
/* Last updated by Enomoto Sanshiro on 23 July 1999. */
/* Edited by Kensuke Yamamoto on 22 November 2018. */


#include <stdio.h>
#include "camlib.h"


int main(void)
{
    COPEN();

    /* set crate number */
    CSETCR(1); 

    CGENZ();
    
    printf("press <ENTER> to set inhibit...");
    (void) getchar();
    CSETI();

    printf("press <ENTER> to release inhibit...");
    (void) getchar();
    CREMI();

    CCLOSE();

    return 0;
}
