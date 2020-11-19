/*
 *
Implementation by Ronny Van Keer, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <stdio.h>
#include <string.h>
#include "Xoodoo.h"

#define VERBOSE         0

#if (VERBOSE > 0)
    #define    Dump(__t)    printf(__t "\n"); \
                            printf("a00 %08x, a01 %08x, a02 %08x, a03 %08x\n", a00, a01, a02, a03 ); \
                            printf("a10 %08x, a11 %08x, a12 %08x, a13 %08x\n", a10, a11, a12, a13 ); \
                            printf("a20 %08x, a21 %08x, a22 %08x, a23 %08x\n\n", a20, a21, a22, a23 );
#else
    #define    Dump(__t)
#endif

#if (VERBOSE >= 1)
    #define    Dump1(__t)    Dump(__t)
#else
    #define    Dump1(__t)
#endif

#if (VERBOSE >= 2)
    #define    Dump2(__t)    Dump(__t)
#else
    #define    Dump2(__t)
#endif

#if (VERBOSE >= 3)
    #define    Dump3(__t)    Dump(__t)
#else
    #define    Dump3(__t)
#endif

uint32_t XoodooMask[12];

/* ---------------------------------------------------------------- */

void Xoodoo_Initialize(uint32_t *state)
{
    memset(state, 0, NLANES*sizeof(tXoodooLane));
}
void Xoodoo_Initialize_Masks0(uint32_t* masks)
{
  masks[0] = 0;
  masks[1] = 0;
  masks[2] = 0;
  masks[3] = 0;
}
void Xoodoo_Initialize_Masks(uint32_t* masks) {
  int i;
  for(i=0;i<48;i++) {
    randbyte(&(((uint8_t*)masks)[i]));
  }
  /*
   * only to check output 
  for(i=0;i<48;i++) {
    uint8_t t;
    randbyte(&t);
  }
  */
}

/* ---------------------------------------------------------------- */

void Xoodoo_AddBytes(void *argState, const unsigned char *argdata, unsigned int offset, unsigned int length)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    if (length == (3*4*4)) {
        uint32_t *state = (uint32_t *)argState;
        uint32_t *data = (uint32_t *)argdata;
        state[0] ^= data[0];
        state[1] ^= data[1];
        state[2] ^= data[2];
        state[3] ^= data[3];
        state[4] ^= data[4];
        state[5] ^= data[5];
        state[6] ^= data[6];
        state[7] ^= data[7];
        state[8] ^= data[8];
        state[9] ^= data[9];
        state[10] ^= data[10];
        state[11] ^= data[11];
    }
    else {
        unsigned int sizeLeft = length;
        unsigned int lanePosition = offset/4;
        unsigned int offsetInLane = offset%4;
        const unsigned char *curData = argdata;
        uint32_t *state = (uint32_t*)argState;

        state += lanePosition;
        if ((sizeLeft > 0) && (offsetInLane != 0)) {
            unsigned int bytesInLane = 4 - offsetInLane;
            uint32_t lane = 0;
            if (bytesInLane > sizeLeft)
                bytesInLane = sizeLeft;
            memcpy((unsigned char*)&lane + offsetInLane, curData, bytesInLane);
            *state++ ^= lane;
            sizeLeft -= bytesInLane;
            curData += bytesInLane;
        }

        while(sizeLeft >= 4) {
            *state++ ^= READ32_UNALIGNED( curData );
            sizeLeft -= 4;
            curData += 4;
        }

        if (sizeLeft > 0) {
            uint32_t lane = 0;
            memcpy(&lane, curData, sizeLeft);
            *state ^= lane;
        }
    }
#else
    #error "Not yet implemented"
#endif
}

/* ---------------------------------------------------------------- */

void Xoodoo_OverwriteBytes(void *argstate, const unsigned char *argdata, unsigned int offset, unsigned int length)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    if (length == (3*4*4)) {
        uint32_t *state = (uint32_t *)argstate;
        uint32_t *data = (uint32_t *)argdata;
        state[0] = data[0];
        state[1] = data[1];
        state[2] = data[2];
        state[3] = data[3];
        state[4] = data[4];
        state[5] = data[5];
        state[6] = data[6];
        state[7] = data[7];
        state[8] = data[8];
        state[9] = data[9];
        state[10] = data[10];
        state[11] = data[11];
    }
    else
        memcpy((unsigned char*)argstate+offset, argdata, length);
#else
    #error "Not yet implemented"
#endif
}

/* ---------------------------------------------------------------- */

void Xoodoo_OverwriteWithZeroes(void *argstate, unsigned int byteCount)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    memset(argstate, 0, byteCount);
#else
    #error "Not yet implemented"
#endif
}

/* ---------------------------------------------------------------- */

void Xoodoo_ExtractBytes(const void *state, unsigned char *data, unsigned int offset, unsigned int length)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    memcpy(data, (unsigned char*)state+offset, length);
#else
    #error "Not yet implemented"
#endif
}

/* ---------------------------------------------------------------- */

void Xoodoo_ExtractAndAddBytes(const void *argState, const unsigned char *input, unsigned char *output, unsigned int offset, unsigned int length)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    if (length == (3*4*4)) {
        uint32_t *state = (uint32_t *)argState;
        const uint32_t *ii = (const uint32_t *)input;
        uint32_t *oo = (uint32_t *)output;

        oo[0] = state[0] ^ ii[0];
        oo[1] = state[1] ^ ii[1];
        oo[2] = state[2] ^ ii[2];
        oo[3] = state[3] ^ ii[3];
        oo[4] = state[4] ^ ii[4];
        oo[5] = state[5] ^ ii[5];
        oo[6] = state[6] ^ ii[6];
        oo[7] = state[7] ^ ii[7];
        oo[8] = state[8] ^ ii[8];
        oo[9] = state[9] ^ ii[9];
        oo[10] = state[10] ^ ii[10];
        oo[11] = state[11] ^ ii[11];
    }
    else {
        unsigned int sizeLeft = length;
        unsigned int lanePosition = offset/4;
        unsigned int offsetInLane = offset%4;
        const unsigned char *curInput = input;
        unsigned char *curOutput = output;
        const uint32_t *state = (const uint32_t*)argState;

        state += lanePosition;
        if ((sizeLeft > 0) && (offsetInLane != 0)) {
            unsigned int bytesInLane = 4 - offsetInLane;
            uint32_t  lane = *state++ >> (offsetInLane * 8);
            if (bytesInLane > sizeLeft)
                bytesInLane = sizeLeft;
            sizeLeft -= bytesInLane;
            do {
                *curOutput++ = (*curInput++) ^ (unsigned char)lane;
                lane >>= 8;
            }
            while ( --bytesInLane != 0);
        }

        while(sizeLeft >= 4) {
            WRITE32_UNALIGNED( curOutput, READ32_UNALIGNED( curInput ) ^ *state++ );
            sizeLeft -= 4;
            curInput += 4;
            curOutput += 4;
        }

        if (sizeLeft > 0) {
            uint32_t  lane = *state;
            do {
                *curOutput++ = (*curInput++) ^ (unsigned char)lane;
                lane >>= 8;
            }
            while ( --sizeLeft != 0 );
        }
    }
#else
    #error "Not yet implemented"
#endif
}

/* ---------------------------------------------------------------- */

#define    DeclareVars  uint32_t    a00, a01, a02, a03; \
                        uint32_t    a10, a11, a12, a13; \
                        uint32_t    a20, a21, a22, a23; \
                        uint32_t    va1, va2;           \
                        uint32_t    b00, b01, b02, b03; \
                        uint32_t    b10, b11, b12, b13; \
                        uint32_t    b20, b21, b22, b23; \
                        uint32_t    vb1, vb2;           \

//                        static volatile uint32_t __attribute__((used)) ddx;

#define    State2Vars   a00 = state[0+0], a01 = state[0+1], a02 = state[0+2], a03 = state[0+3]; \
                        a10 = state[4+0], a11 = state[4+1], a12 = state[4+2], a13 = state[4+3]; \
                        a20 = state[8+0], a21 = state[8+1], a22 = state[8+2], a23 = state[8+3]; \
                        b00 = masks[0+0], b01 = masks[0+1], b02 = masks[0+2], b03 = masks[0+3]; \
                        b10 = masks[4+0], b11 = masks[4+1], b12 = masks[4+2], b13 = masks[4+3]; \
                        b20 = masks[8+0], b21 = masks[8+1], b22 = masks[8+2], b23 = masks[8+3]; \

#define    Vars2State   state[0+0] = a00, state[0+1] = a01, state[0+2] = a02, state[0+3] = a03; \
                        state[4+0] = a10, state[4+1] = a11, state[4+2] = a12, state[4+3] = a13; \
                        state[8+0] = a20, state[8+1] = a21, state[8+2] = a22, state[8+3] = a23; \

/*
** Theta: Column Parity Mixer
*/
#define Theta_() \
                    va1 = a03 ^ a13 ^ a23;                   \
                    vb1 = b03 ^ b13 ^ b23;                   \
                    va2 = a00 ^ a10 ^ a20;                   \
                    va1 = ROTL32(va1, 5) ^ ROTL32(va1, 14);    \
                    a00 ^= va1;                              \
                    a10 ^= va1;                              \
                    a20 ^= va1;                              \
                    va1 = a01 ^ a11 ^ a21;                   \
                    va2 = ROTL32(va2, 5) ^ ROTL32(va2, 14);    \
                    a01 ^= va2;                              \
                    a11 ^= va2;                              \
                    a21 ^= va2;                              \
                    va2 = a02 ^ a12 ^ a22;                   \
                    va1 = ROTL32(va1, 5) ^ ROTL32(va1, 14);    \
                    a02 ^= va1;                              \
                    a12 ^= va1;                              \
                    a22 ^= va1;                              \
                    va2 = ROTL32(va2, 5) ^ ROTL32(va2, 14);    \
                    a03 ^= va2;                              \
                    a13 ^= va2;                              \
                    a23 ^= va2;                              \
                    vb1 = b03 ^ b13 ^ b23;                   \
                    vb2 = b00 ^ b10 ^ b20;                   \
                    vb1 = ROTL32(vb1, 5) ^ ROTL32(vb1, 14);    \
                    b00 ^= vb1;                              \
                    b10 ^= vb1;                              \
                    b20 ^= vb1;                              \
                    vb1 = b01 ^ b11 ^ b21;                   \
                    vb2 = ROTL32(vb2, 5) ^ ROTL32(vb2, 14);    \
                    b01 ^= vb2;                              \
                    b11 ^= vb2;                              \
                    b21 ^= vb2;                              \
                    vb2 = b02 ^ b12 ^ b22;                   \
                    vb1 = ROTL32(vb1, 5) ^ ROTL32(vb1, 14);    \
                    b02 ^= vb1;                              \
                    b12 ^= vb1;                              \
                    b22 ^= vb1;                              \
                    vb2 = ROTL32(vb2, 5) ^ ROTL32(vb2, 14);    \
                    b03 ^= vb2;                              \
                    b13 ^= vb2;                              \
                    b23 ^= vb2


#define Theta()                                             \
                    va1 = a03 ^ a13 ^ a23;                   \
                    vb1 = b03 ^ b13 ^ b23;                   \
                    va2 = a00 ^ a10 ^ a20;                   \
                    va1 = ROTL32(va1, 5) ^ ROTL32(va1, 14);    \
                    a00 ^= va1;                              \
                    a10 ^= va1;                              \
                    a20 ^= va1;                              \
                    va1 = a01 ^ a11 ^ a21;                   \
                    va2 = ROTL32(va2, 5) ^ ROTL32(va2, 14);    \
                    a01 ^= va2;                              \
                    a11 ^= va2;                              \
                    a21 ^= va2;                              \
                    va2 = a02 ^ a12 ^ a22;                   \
                    va1 = ROTL32(va1, 5) ^ ROTL32(va1, 14);    \
                    a02 ^= va1;                              \
                    a12 ^= va1;                              \
                    a22 ^= va1;                              \
                    va2 = ROTL32(va2, 5) ^ ROTL32(va2, 14);    \
                    a03 ^= va2;                              \
                    a13 ^= va2;                              \
                    a23 ^= va2;                              \
                    vb1 = b03 ^ b13 ^ b23;                   \
                    vb2 = b00 ^ b10 ^ b20;                   \
                    vb1 = ROTL32(vb1, 5) ^ ROTL32(vb1, 14);    \
                    b00 ^= vb1;                              \
                    b10 ^= vb1;                              \
                    b20 ^= vb1;                              \
                    vb1 = b01 ^ b11 ^ b21;                   \
                    vb2 = ROTL32(vb2, 5) ^ ROTL32(vb2, 14);    \
                    b01 ^= vb2;                              \
                    b11 ^= vb2;                              \
                    b21 ^= vb2;                              \
                    vb2 = b02 ^ b12 ^ b22;                   \
                    vb1 = ROTL32(vb1, 5) ^ ROTL32(vb1, 14);    \
                    b02 ^= vb1;                              \
                    b12 ^= vb1;                              \
                    b22 ^= vb1;                              \
                    vb2 = ROTL32(vb2, 5) ^ ROTL32(vb2, 14);    \
                    b03 ^= vb2;                              \
                    b13 ^= vb2;                              \
                    b23 ^= vb2

/*
** Rho-west: Plane shift
*/
#define Rho_west()                          \
                    a20 = ROTL32(a20, 11);  \
                    a21 = ROTL32(a21, 11);  \
                    a22 = ROTL32(a22, 11);  \
                    a23 = ROTL32(a23, 11);  \
                    va1 = a13;               \
                    a13 = a12;              \
                    a12 = a11;              \
                    a11 = a10;              \
                    a10 = va1;              \
                    b20 = ROTL32(b20, 11);  \
                    b21 = ROTL32(b21, 11);  \
                    b22 = ROTL32(b22, 11);  \
                    b23 = ROTL32(b23, 11);  \
                    vb1 = b13;               \
                    b13 = b12;              \
                    b12 = b11;              \
                    b11 = b10;              \
                    b10 = vb1

/*
** Iota: Round constants
*/
#define Iota(__rc)  a00 ^= __rc

/*
** Chi: Non linear step, on colums
*/
#define Chi()                                           \
                    a00 ^= (~a10 & a20) ^ (a10 & b20);  \
                    b00 ^= (~b10 & b20) ^ (b10 & a20);  \
                    a01 ^= (~a11 & a21) ^ (a11 & b21);  \
                    b01 ^= (~b11 & b21) ^ (b11 & a21);  \
                    a02 ^= (~a12 & a22) ^ (a12 & b22);  \
                    b02 ^= (~b12 & b22) ^ (b12 & a22);  \
                    a03 ^= (~a13 & a23) ^ (a13 & b23);  \
                    b03 ^= (~b13 & b23) ^ (b13 & a23);  \
                                                        \
                    a10 ^= (~a20 & a00) ^ (a20 & b00);  \
                    b10 ^= (~b20 & b00) ^ (b20 & a00);  \
                    a11 ^= (~a21 & a01) ^ (a21 & b01);  \
                    b11 ^= (~b21 & b01) ^ (b21 & a01);  \
                    a12 ^= (~a22 & a02) ^ (a22 & b02);  \
                    b12 ^= (~b22 & b02) ^ (b22 & a02);  \
                    a13 ^= (~a23 & a03) ^ (a23 & b03);  \
                    b13 ^= (~b23 & b03) ^ (b23 & a03);  \
                                                        \
                    a20 ^= (~a00 & a10) ^ (a00 & b10);  \
                    b20 ^= (~b00 & b10) ^ (b00 & a10);  \
                    a21 ^= (~a01 & a11) ^ (a01 & b11);  \
                    b21 ^= (~b01 & b11) ^ (b01 & a11);  \
                    a22 ^= (~a02 & a12) ^ (a02 & b12);  \
                    b22 ^= (~b02 & b12) ^ (b02 & a12);  \
                    a23 ^= (~a03 & a13) ^ (a03 & b13);  \
                    b23 ^= (~b03 & b13) ^ (b03 & a13)

/*
** Rho-east: Plane shift
*/
#define Rho_east()                          \
                    a10 = ROTL32(a10, 1);   \
                    a11 = ROTL32(a11, 1);   \
                    a12 = ROTL32(a12, 1);   \
                    a13 = ROTL32(a13, 1);   \
                    va1 = ROTL32(a23, 8);   \
                    a23 = ROTL32(a21, 8);   \
                    a21 = va1;              \
                    va1 = ROTL32(a22, 8);   \
                    a22 = ROTL32(a20, 8);   \
                    a20 = va1;              \
                    b10 = ROTL32(b10, 1);   \
                    b11 = ROTL32(b11, 1);   \
                    b12 = ROTL32(b12, 1);   \
                    b13 = ROTL32(b13, 1);   \
                    vb1 = ROTL32(b23, 8);   \
                    b23 = ROTL32(b21, 8);   \
                    b21 = vb1;              \
                    vb1 = ROTL32(b22, 8);   \
                    b22 = ROTL32(b20, 8);   \
                    b20 = vb1

#define Round(__rc)                         \
                    Theta();                \
                    Dump3("Theta");         \
                    Rho_west();             \
                    Dump3("Rho-west");      \
                    Iota(__rc);             \
                    Dump3("Iota");          \
                    Chi();                  \
                    Dump3("Chi");           \
                    Rho_east();             \
                    Dump3("Rho-east")
#define Round_(__rc) \
                    starttrigger();\
                    Theta();               \
                    Dump3("Theta");         \
                    Rho_west();             \
                    Dump3("Rho-west");      \
                    Iota(__rc);             \
                    Dump3("Iota");          \
                    Chi();                  \
                    Dump3("Chi");           \
                    Rho_east();             \
                    endtrigger();\
                    Dump3("Rho-east")

static const uint32_t    RC[MAXROUNDS] = {
    _rc12,
    _rc11,
    _rc10,
    _rc9,
    _rc8,
    _rc7,
    _rc6,
    _rc5,
    _rc4,
    _rc3,
    _rc2,
    _rc1
};

void Xoodoo_Permute_Nrounds( uint32_t * state, uint32_t nr, uint32_t* masks)
{
    DeclareVars;
    uint32_t    i;

    State2Vars;
    for (i = MAXROUNDS - nr; i < MAXROUNDS; ++i ) {
        Round(RC[i]);
        Dump2("Round");
    }
    Dump1("Permutation");
    Vars2State;
}

void Xoodoo_Permute_6rounds( uint32_t * state, uint32_t* masks)
{
    DeclareVars;

    State2Vars;
    Round(_rc6);
    Round(_rc5);
    Round(_rc4);
    Round(_rc3);
    Round(_rc2);
    Round(_rc1);
    Dump1("Permutation");
    Vars2State;
}
void Xoodoo_Permute_12rounds( uint32_t * state, uint32_t *masks)
{    
    DeclareVars;
    State2Vars;
    // Mask
    a00 ^= b00 ; a01 ^= b01 ; a02 ^= b02 ; a03 ^= b03 ;
    a10 ^= b10 ; a11 ^= b11 ; a12 ^= b12 ; a13 ^= b13 ;
    a20 ^= b20 ; a21 ^= b21 ; a22 ^= b22 ; a23 ^= b23 ;
     
    Round_(_rc12); 
     
    Round(_rc11);
    Round(_rc10);
    Round(_rc9);
    Round(_rc8);
    Round(_rc7);
    Round(_rc6);
    Round(_rc5);
    Round(_rc4);
    Round(_rc3);
    Round(_rc2);
    Round(_rc1);
    Dump1("Permutation");
    // Unmask
    a00 ^= b00 ; a01 ^= b01 ; a02 ^= b02 ; a03 ^= b03 ;
    a10 ^= b10 ; a11 ^= b11 ; a12 ^= b12 ; a13 ^= b13 ;
    a20 ^= b20 ; a21 ^= b21 ; a22 ^= b22 ; a23 ^= b23 ;
    Vars2State;
}
// wrong endianness
/*void Xoodoo_Permute_12rounds( uint32_t * state, uint32_t* masks)
{
    int i;
    DeclareVars;

    // Mask
    for(i=0;i<12;i++) {
      state[i] ^= masks[i];
    }

    //starttrigger();
    State2Vars;
    Round_(_rc12);

    //endtrigger();
    Round(_rc11);
    Round(_rc10);

    Round(_rc9);
    Round(_rc8);
    Round(_rc7);
    Round(_rc6);
    Round(_rc5); 
    Round(_rc4);
    Round(_rc3);
    Round(_rc2);

    Round(_rc1);

    Dump1("Permutation");

    // Unmask
    for(i=0;i<12;i++) {
      state[i] ^= masks[i];
    } 
 
    Vars2State;
}

*/
