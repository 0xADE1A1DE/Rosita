/* Copyright 2020 University of Adelaide
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define NTRACES /*@NTRACES{*/ 10000 /*}*/
#define TEST_TRACES (NTRACES/2)


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../elmoasmfunctionsdef.h"
#define ALIGN __attribute__ ((aligned (4)))
#define KEY_SZ 32
#define PLT_SZ 16
#define POS_SZ 8
#define IV_SZ 8
#define STATE_SZ 48
/*
 * 4 byte align arrays to make sure we don't load invalid data
 * for state of byte-wise operations (ARM loads/stores 4 bytes
 * from addr & ~(0b11) when load/storing from address addr).
 * In ARM, Addresses that are not divisable by 4 are invalid when used
 * with wordwise instructions (ldr, str)
 */ 

extern void runrand();
uint32_t state[16]={0};
uint8_t tempstate[64]={0};
uint32_t masks[17]={0};
uint8_t key[32]={0};
uint8_t iv[8]={0};
uint8_t pos[8]={0};

void InitChacha20(uint32_t* state)
{
    for(int i=0;i<16;i++)
    {
        state[i] = 0;
    }
    state[0] = 1634760805;
    state[1] = 857760878;
    state[2] = 2036477234;
    state[3] = 1797285236;
    /*
    ctx[4 : 12] = struct.unpack('<8L', key) pycodestyle: E111 indentation is not a multiple of four
    ctx[12] = ctx[13] = position pycodestyle: E111 indentation is not a multiple of four
    ctx[14 : 16] = struct.unpack('<LL', iv) pycodestyle: E111 indentation is not a multiple of four
    */
}
void LoadMasks(uint32_t* masks)
{
    uint8_t* pm = (uint8_t*)&masks[0];
    for (int i=0;i<sizeof(uint32_t)*17;i++)
    {
        readrandombyte(&pm[i]);
    }
}
void MaskChacha20(uint32_t* state, uint32_t* masks)
{
    for (int i=0;i<16;i++)
    {
        state[i] ^= masks[i];
    }
}
void SetKeyIVChacha20(uint32_t* state, uint8_t* key /*32 bytes*/, uint8_t* iv /*8 bytes*/, uint8_t* pos /* 8 */)
{
    uint8_t* pkey = (uint8_t*)&state[4];
    for (int i=0;i<32;i++)
    {
        pkey[i] = key[i];
    }
    uint8_t* ppos = (uint8_t*)&state[12];
    uint8_t* piv = (uint8_t*)&state[14];
    for (int i=0;i<8;i++)
    {
        ppos[i] = pos[i];
        piv[i] = iv[i];
    }
}

/* 
 * Clear stack using a volatile buffer so that at each run the stack
 * is the same otherwise content from the previous run ends up leaking
 * this can be also mitigated by running each test on a coin flip (this 
 * is the way how that the real world counterpart of this test is being run).
 * However, as this is a simulated run -- we know what state related effects
 * change affect the T-test values therefore we clear all state related 
 * interactions.
 */
void stack_clear()
{
    volatile uint8_t arr[1024];
    int i=0;
    for (i=0;i<1024;i++)
    {
        arr[i] = 0;
    }
}



void run_byte_chacha20_masked_(uint8_t* ivbuf, uint8_t* keybuf, uint8_t* posbuf)
{
    int i,j;
    
    LoadMasks(masks);
    InitChacha20(state);
    SetKeyIVChacha20(state, keybuf, ivbuf, posbuf);
    MaskChacha20(state, masks); 
    stack_clear();
    runrand();
    //chacha_local_loop_2share_unrolled_randomness(state, masks);
    
    
    asm volatile (

    "push {r0-r7}\n"
    "movs r0, %0\n"                                                                                  
    "movs r1, %1\n"
    "mov r6, #0\n"                                                                                   
    "mov r5, #0\n"
    "mov r4, #0\n"
    "mov r3, #0\n"
    "mov r2, #0\n"
    "bl chacha_local_loop_2share_unrolled_randomness\n"
    "pop {r0-r7}\n"                                                                                  
    :                                                                                                
    : "r" (state), "r" (masks)
    //: "r0", "r1", "r2", "r3", "r4", "r5", "r6"
    );
    //Xoodoo_Permute_12rounds(state, XoodooMask);
    for(j=0;j<STATE_SZ;j++)
    {
        uint8_t* ps = (uint8_t*)&state[0];
        uint8_t* pm = (uint8_t*)&masks[0];
        uint8_t bb = ps[j] ^ pm[j];
        printbyte(&bb);
    }
       
}


#define RUN_FUNC(...) \
    run_byte_chacha20_masked_(__VA_ARGS__)


//static const uint8_t fixedinput[16] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t fixedkey[KEY_SZ] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
                                         0x1e, 0x2d, 0x3f, 0x4c, 0x51, 0x62, 0x73, 0x84, 0x95, 0xa1, 0xb2, 0xc3, 0xd4, 0xe5, 0xf6, 0x07	};

static const uint8_t fixedinput[PLT_SZ] = { /*@FIXED_INPUT{*/ 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90/*}*/ };

void simple_init()
{
    int j=0;
    for(;j<STATE_SZ;j++)
    {
    }
}


void simple_run()
{

    int i,j;

    // Use fixed key and fixed input as specified by CRI for use with AES
    //static const uint8_t fixedkey[16] = {0xf3, 0x34, 0x15, 0x6e, 0x32, 0x11, 0x23, 0xff, 0x53, 0x45, 0xde, 0xab, 0xb4, 0xb5, 0xd4, 0xa0};
    //static const uint8_t fixedkey[16] = {0x11, 0x43, 0x4e, 0xdd, 0x55, 0xa6, 0xcc, 0xef, 0x10, 0xde, 0x5d, 0x7f, 0xda, 0xbf, 0xdf, 0xff};

    for(i=0;i<NTRACES;i++)
    { 
        for (j=0;j<KEY_SZ;j++)
        {  
            key[j] = fixedkey[j];
        }
        unsigned char test;
        readrandombyte(&test);

        test = test & 0x01;
        //printgroup(&test);

        if (i < (NTRACES/2))
        {
            for (j=0;j<IV_SZ;j++)
            {
                readrandombyte(&iv[j]);
                iv[j] = fixedinput[j];
            }
            for (j=0;j<POS_SZ;j++)
            {
                readrandombyte(&pos[j]);
                pos[j] = fixedinput[j+8];
            }
        }
        else
        {
            for (j=0;j<IV_SZ;j++)
            {    
                readrandombyte(&iv[j]);
            }
            for (j=0;j<POS_SZ;j++)
            {
                readrandombyte(&pos[j]);
            }
        }
        run_byte_chacha20_masked_(iv, key, pos);
    }
    endprogram();
}
