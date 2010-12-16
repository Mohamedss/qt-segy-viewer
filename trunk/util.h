#ifndef UTILH
#define UTILH

typedef unsigned char byte;

void ibm2ieee(float* input, int swap);
void toibm   (long *addr, int ll);

unsigned char ebasc(unsigned char ascii);
void          ebasd(unsigned char* ascii, unsigned char* ebcd);
void          asebd(char* ebcd, char* ascii);
char          asebc(char ascii);

int setswap(int swap);

int   i4(char* buf, int nbyte);
short i2(char* buf, int nbyte);
char  i1(char* buf, int nbyte);

void si4(char* buf, int nbyte,  int i);
void si2(char* buf,int nbyte, short i);
void si1(char* buf, int nbyte, char i);

int   swapi4(int   x);
short swapi2(short x);

#endif
