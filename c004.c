/* OUTPUT Register Operation Source Code */
#define OUTREG 20 // Station Number
int ch[]={0}; // Channel Number List
#define NCH (sizeof(ch)/sizeof(ch[0]))

/**** Edited by Takashi Kanai ****/
/**** Last update at 2005/4/1 ****/
/*** Modified at 2008/5/22 by Yosuke Maeda ***/
/*** Modified at 2010/8/04 by Yasuyuki Furuichi ***/
/*** Modified at 2019/03/13 by Kensuke Yamamoto ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/errno.h>
#include "camlib.h"

#define ENLAM 8
#define CLR 9
#define LAM 8
#define READ 0
#define READCL 2
#define WRITE 16

int main(int argc, char *argv[]){
  int dumy, q=0, x=0;
  int output_ch = 0x4001; // ?

  /* Initialize CAMAC */
  if(CAMOPN()){
    printf("CAMAC OPEN ERROR!\n");
    return 1;
  }
  CSETCR(1);  // Crate Number
  CGENZ();
  fprintf(stderr,"### CAMAC initialized.\n");

  CGENC();


  //  CAMAC(NAF(OUTREG,0,ENLAM), &dumy, &q, &x);
  CAMAC(NAF(OUTREG,0,CLR), &dumy, &q, &x);
  fprintf(stderr,"Clear output pulse.\n");

  CAMAC(NAF(OUTREG,0,WRITE), &output_ch, &q, &x);
  fprintf(stderr,"Output from ch1.\n");


  CGENZ();
  CSETI();
  CAM_Close();

  printf("---------------------------\n");
  printf("### DAQ ended successfully.\n");
  return 0;
}
