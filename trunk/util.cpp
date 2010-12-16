#include "util.h"
#include <mem.h>

#define swapb(x,y) {byte tmp; tmp=(x); (x)=(y); (y)=tmp;}

static int _swap=1;

int setswap(int swap)
{
    _swap=swap; return swap;
}


int swapi4(int x)
{
  if(_swap)
  {
    byte* cbuf;
    byte  tem;

    cbuf=(byte*)&x;        /* assign address of input to char array */

    tem=cbuf[0]; cbuf[0]=cbuf[3]; cbuf[3]=tem;
    tem=cbuf[2]; cbuf[2]=cbuf[1]; cbuf[1]=tem;
  }
  return x;
}

short swapi2(short x)
{
  if(_swap)
  {
     byte* cbuf;
     byte  tem;

     cbuf=(byte*)&x;        /* assign address of input to char array */

     tem=cbuf[0]; cbuf[0]=cbuf[1]; cbuf[1]=tem;
  }

  return x;
}



void ibm2ieee(float* input, int swap)
{
 typedef unsigned char byte;
 typedef unsigned long ulng;

  byte  *cbuf,expp,tem,sign;
  ulng  *umantis,expll;
  long *mantis;
  int  shift;

  cbuf=(byte*)&input[0];        /* assign address of input to char array */
  umantis=(ulng*)&input[0];     /* two differnt points to the same spot  */
  mantis =(long*)&input[0];     /* signned & unsigned                    */

  if(swap)
  {
  /* now byte reverce for PC use if swap true */
  tem=cbuf[0]; cbuf[0]=cbuf[3]; cbuf[3]=tem;
  tem=cbuf[2]; cbuf[2]=cbuf[1]; cbuf[1]=tem;
  }

  /* start extraction information from number */

  expp=*mantis>>24;     /* get expo fro upper byte      */
  *mantis=(*mantis)<<8; /* shift off upper byte         */
  shift=1;              /* set a counter to 1           */
  while(*mantis>0 && shift<23) /* start of shifting data*/
   {
         *mantis=*mantis<<1;
         shift++;
   } /* shift until a 1 in msb */

  *mantis=*mantis<<1; /* need one more shift to get implied one bit */
  sign=expp & 0x80;   /* set sign to msb of exponent            */
  expp=expp & 0x7F;   /* kill sign bit                          */

  if(expp!=0)        /* don't do anymore if zero exponent       */
   {
                expp=expp-64;   /* compute what shift was (old exponent)*/
                *umantis=*umantis>>9; /* MOST IMPORTANT an UNSIGNED shift back down */
                expll=0x7F+(expp*4-shift); /* add in excess 172 */

                /* now mantissa is correctly aligned, now create the other two pairs */
                /* needed the extended sign word and the exponent word               */

                expll=expll<<23;        /* shift exponent up */

                /* combine them into a floating point IEEE format !     */

                if(sign) *umantis=expll | *mantis | 0x80000000;
                else     *umantis=expll | *mantis; /* set or don't set sign bit */
   }
}

void toibm(long *addr, int ll)
{
    unsigned long real;

    unsigned long lng;
    unsigned char *ch;

    unsigned int gain,sign,t;

    //    ch= (long *)&lng;
    ch= (unsigned char*)&lng;

    while(ll--)
     {
        if(real = *addr)
         {
            gain = (real>>23);
            sign = (gain & 0x100)>>1;
            gain = (gain & 0xff) - 0x80 +2;
            real = ((real& 0x7FFFFFL)| 0x800000L);

            //      real = (real& 0x7FFFFFL);
            //      real = (real | 0x800000L);

            t = gain & 3;
            if(t)
             {
                real >>= 4-t;
                gain +=  4-t;
             }
            gain = ((gain>>2) + 0x40) & 0x7f;
            gain |= sign;
            lng = real;
            ch[3] = ch[0];
            t = ch[1];
            ch[1] = ch[2];
            ch[2] = t;
            ch[0] = gain;
            real = lng;
         }
        *addr++ = real;
     }
}

int tt[256]=
 {
    32,  32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 91, 46, 60, 40, 43, 33,
    38, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 93, 36, 42, 41, 59, 94,
    45, 47, 32, 32, 32, 32, 32, 32,
    32, 32,124, 44, 37, 95, 62, 63,
    32, 32, 32, 32, 32, 32,238,160,
    161, 96, 58, 35, 64, 39, 61, 34,
    230, 97, 98, 99,100,101,102,103,
    104,105,164,165,228,163,229,168,
    169,106,107,108,109,110,111,112,
    113,114,170,171,172,173,174,175,
    239,126,115,116,117,118,119,120,
    121,122,224,225,226,227,166,162,
    236,235,167,232,237,233,231,234,
    158,128,129,150,132,133,148,131,
    123, 65, 66, 67, 68, 69, 70, 71,
    72, 73,149,136,137,138,139,140,
    125, 74, 75, 76, 77, 78, 79, 80,
    81, 82,141,142,143,159,144,145,
    92, 32, 83, 84, 85, 86, 87, 88,
    89, 90,146,147,134,130,156,155,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57,135,152,157,153,151, 32
 };

unsigned char ebasc(unsigned char ascii)
 {
    unsigned char	ebcd;
    ebcd=tt[ascii];
    return ebcd;
 }


void ebasd(unsigned char* ascii, unsigned char* ebcd)
 {
    int     n;

    for(n=0;ebcd[n];n++)
     {
        ascii[n]=ebasc(ebcd[n]);
     }
    ascii[n]=0;
 }

char asebc(char ascii)
{
        char	        ebcd;
        static char     tt[256]=
        {
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x4F,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,	/*  !"#$%&'	*/
        0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61,	/* ()*+,-./	*/
        0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,	/* 01234567	*/
        0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F,	/* 89:;<=>?	*/
        0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,	/* @ABCDEFG	*/
        0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,	/* HIJKLMNO	*/
        0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,	/* PQRSTUVW	*/
        0xE7,0xE8,0xE9,0x4A,0xE0,0x5A,0x5F,0x6D,	/* XYZ[\]^_	*/
        0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,	/* `abcdefg	*/
        0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,	/* hijklmno	*/
        0x97,0x98,0x99,0xA2,0xA3,0xA4,0xA5,0xA6,	/* pqrstuvw	*/
        0xA7,0xA8,0xA9,0xC0,0x6A,0xD0,0xA1,0x40,	/* xyz{|}~ 	*/
        0xB9,0xBA,0xED,0xBF,0xBC,0xBD,0xEC,0xFA,	/*              */
        0xCB,0xCC,0xCD,0xCE,0xCF,0xDA,0xDB,0xDC,	/*             	*/
        0xDE,0xDF,0xEA,0xEB,0xBE,0xCA,0xBB,0xFE,	/*		*/
        0xFB,0xFD,0x7d,0xEF,0xEE,0xFC,0xB8,0xDD,	/*		*/
        0x77,0x78,0xAF,0x8D,0x8A,0x8B,0xAE,0xB2,	/*		*/
        0x8F,0x90,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,	/*		*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0xAA,0xAB,0xAC,0xAD,0x8C,0x8E,0x80,0xB6,	/* ðñòóôõö÷	*/
        0xB3,0xB5,0xB7,0xB1,0xB0,0xB4,0x76,0xA0,	/* øùúûüýþÿ	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*        	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40 	/*        	*/
        };
                ebcd=tt[ascii];
                return ebcd;
}


void asebd( char* ebcd, char* ascii)
{
    int     n;

    for(n=0;ascii[n];n++)
     {
        ebcd[n]=asebc(ascii[n]);
     }
    ebcd[n]=0;
}


void si4(char* buf, int nbyte, int i)
 {
    union { byte h[4]; int x; } u;

    u.x=i;

    if(_swap) { swapb(u.h[0], u.h[3]); swapb(u.h[1], u.h[2]);}

    memcpy(buf+nbyte-1, &u.x, sizeof(int));
 }

void si2(char* buf,int nbyte, short i)
 {
    union { byte h[2]; short x; } u;

    u.x=i;

    if(_swap) swapb(u.h[0], u.h[1]);

    memcpy(buf+nbyte-1, &u.x, sizeof(short));
 }

void si1(char* buf, int nbyte, char i)
 {
    memcpy(buf+nbyte-1,&i,sizeof(char));
 }


int i4(char* buf, int nbyte)
 {
    int i;
    union { byte h[4]; long x; } u;

    memcpy(&i,buf+nbyte-1,sizeof(int));
    u.x=i;

    if(_swap)
         swapb(u.h[0], u.h[3]); swapb(u.h[1], u.h[2]);
    return u.x;
 }

short i2(char* buf, int nbyte)
 {
    short i;
    union { byte h[2]; int x; } u;

    memcpy(&i,buf+nbyte-1,sizeof(short));
    u.x=i;
    if(_swap)
     swapb(u.h[0], u.h[1]);

    return u.x;
 }

char i1(char* buf, int nbyte)
 {
    char i;

    memcpy(&i,buf+nbyte-1,sizeof(char));

    return i;
 }
