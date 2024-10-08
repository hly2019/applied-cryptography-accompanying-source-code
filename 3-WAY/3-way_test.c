/********************************************************************\
*                                                                    *
* C specification of the threeway block cipher                       *
*                                                                    *
\********************************************************************/
/*file i/o main function by Pate Williams 1996*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CLK_TCK 1000000.0
#define   STRT_E   0x0b0b /* round constant of first encryption round */
#define   STRT_D   0xb1b1 /* round constant of first decryption round */
#define     NMBR       11 /* number of rounds is 11                   */

#define   BLK_SIZE     12 /*number of bytes per block*/

typedef   unsigned long int  word32 ;
                 /* the program only works correctly if long = 32bits */

void mu(word32 *a)       /* inverts the order of the bits of a */
{
int i ;
word32 b[3] ;

b[0] = b[1] = b[2] = 0 ;
for( i=0 ; i<32 ; i++ )
   {
   b[0] <<= 1 ; b[1] <<= 1 ; b[2] <<= 1 ;
   if(a[0]&1) b[2] |= 1 ;
   if(a[1]&1) b[1] |= 1 ;
   if(a[2]&1) b[0] |= 1 ;
   a[0] >>= 1 ; a[1] >>= 1 ; a[2] >>= 1 ;
   }

a[0] = b[0] ;      a[1] = b[1] ;      a[2] = b[2] ;
}

void gamma(word32 *a)   /* the nonlinear step */
{
word32 b[3] ;

b[0] = a[0] ^ (a[1]|(~a[2])) ;
b[1] = a[1] ^ (a[2]|(~a[0])) ;
b[2] = a[2] ^ (a[0]|(~a[1])) ;

a[0] = b[0] ;      a[1] = b[1] ;      a[2] = b[2] ;
}

void theta(word32 *a)    /* the linear step */
{
word32 b[3];

b[0] = a[0] ^  (a[0]>>16) ^ (a[1]<<16) ^     (a[1]>>16) ^ (a[2]<<16) ^
               (a[1]>>24) ^ (a[2]<<8)  ^     (a[2]>>8)  ^ (a[0]<<24) ^
               (a[2]>>16) ^ (a[0]<<16) ^     (a[2]>>24) ^ (a[0]<<8)  ;
b[1] = a[1] ^  (a[1]>>16) ^ (a[2]<<16) ^     (a[2]>>16) ^ (a[0]<<16) ^
               (a[2]>>24) ^ (a[0]<<8)  ^     (a[0]>>8)  ^ (a[1]<<24) ^
               (a[0]>>16) ^ (a[1]<<16) ^     (a[0]>>24) ^ (a[1]<<8)  ;
b[2] = a[2] ^  (a[2]>>16) ^ (a[0]<<16) ^     (a[0]>>16) ^ (a[1]<<16) ^
               (a[0]>>24) ^ (a[1]<<8)  ^     (a[1]>>8)  ^ (a[2]<<24) ^
               (a[1]>>16) ^ (a[2]<<16) ^     (a[1]>>24) ^ (a[2]<<8)  ;

a[0] = b[0] ;      a[1] = b[1] ;      a[2] = b[2] ;
}

void pi_1(word32 *a)
{
a[0] = (a[0]>>10) ^ (a[0]<<22);
a[2] = (a[2]<<1)  ^ (a[2]>>31);
}

void pi_2(word32 *a)
{
a[0] = (a[0]<<1)  ^ (a[0]>>31);
a[2] = (a[2]>>10) ^ (a[2]<<22);
}

void rho(word32 *a)    /* the round function       */
{
theta(a) ;
pi_1(a) ;
gamma(a) ;
pi_2(a) ;
}

void rndcon_gen(word32 strt,word32 *rtab)
{                           /* generates the round constants */
int i ;

for(i=0 ; i<=NMBR ; i++ )
   {
   rtab[i] = strt ;
   strt <<= 1 ;
   if( strt&0x10000 ) strt ^= 0x11011 ;
   }
}

void encrypt(word32 *a, word32 *k)
{
char i ;
word32 rcon[NMBR+1] ;

rndcon_gen(STRT_E,rcon) ;
for( i=0 ; i<NMBR ; i++ )
   {
   a[0] ^= k[0] ^ (rcon[i]<<16) ;
   a[1] ^= k[1] ;
   a[2] ^= k[2] ^ rcon[i] ;
   rho(a) ;
   }
a[0] ^= k[0] ^ (rcon[NMBR]<<16) ;
a[1] ^= k[1] ;
a[2] ^= k[2] ^ rcon[NMBR] ;
theta(a) ;
}

void decrypt(word32 *a, word32 *k)
{
char i ;
word32 ki[3] ;          /* the `inverse' key             */
word32 rcon[NMBR+1] ;   /* the `inverse' round constants */

ki[0] = k[0] ; ki[1] = k[1] ; ki[2] = k[2] ;
theta(ki) ;
mu(ki) ;

rndcon_gen(STRT_D,rcon) ;

mu(a) ;
for( i=0 ; i<NMBR ; i++ )
   {
   a[0] ^= ki[0] ^ (rcon[i]<<16) ;
   a[1] ^= ki[1] ;
   a[2] ^= ki[2] ^ rcon[i] ;
   rho(a) ;
   }
a[0] ^= ki[0] ^ (rcon[NMBR]<<16) ;
a[1] ^= ki[1] ;
a[2] ^= ki[2] ^ rcon[NMBR] ;
theta(a) ;
mu(a) ;
}

void threeway_test(
    char key0,
    char key1,
    char key2,
    char key3,
    char key4,
    char key5,
    char key6,
    char key7,
    char key8,
    char key9,
    char key10,
    char key11
)
{
  char key[13] = {key0,key1,key2,key3,key4,key5,key6,key7,key8,key9,key10,key11,'\0'}, 
  *kp = key;
  word32 buffer[12] = {0}, word_key[3] = {0};
  memcpy(word_key, key, 12);
  memcpy(buffer, "HelloWorld!!", 12 * sizeof(char));

  encrypt(buffer, word_key);
  decrypt(buffer, word_key);
//   printf("buffer: \n");
   // for(int i = 0; i < 12; i++) {
   //    printf("%d ", *((char*) ((char*)buffer + i * sizeof(char))));
   // }
   // printf("\n");
}


// int main() {
//    threeway_test('1', '1','1','1','1','1','1','1','1','1','1','1');
//    return 0;
// }