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
#define PLT_SZ 16
#define KEY_SZ 16
#define STATE_SZ 48
/*
 * 4 byte align arrays to make sure we don't load invalid data
 * for state of byte-wise operations (ARM loads/stores 4 bytes
 * from addr & ~(0b11) when load/storing from address addr).
 * In ARM, Addresses that are not divisable by 4 are invalid when used
 * with wordwise instructions (ldr, str)
 */ 
volatile uint8_t input[PLT_SZ] ALIGN;
volatile uint8_t XoodooState[STATE_SZ] ALIGN;
volatile uint8_t key[KEY_SZ] ALIGN;
volatile int domask=1;

extern void runrand();
extern uint32_t XoodooMask[12];
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


void Absorb_Block(uint32_t *state, uint32_t *block)
{
  int i;
  for(i=0;i<4;i++) {
    state[i] ^= block[i];
  }
}

void Init_State(uint32_t *state)
{
  state[0] = 0xa144f6af;
  state[1] = 0x0a09bef3;
  state[2] = 0xf69b27da;
  state[3] = 0xebf1aa2f;
  state[4] = 0x79ec427d;
  state[5] = 0x394b82c3;
  state[6] = 0x15d52030;
  state[7] = 0xe3d85ac4;
  state[8] = 0x661b20a0;
  state[9] = 0x01fc8349;
  state[10] = 0x76868f25;
  state[11] = 0x98fcda2a;
}


void run_byte_xoodoo_masked_(uint8_t* inputbuf, uint8_t* keybuf)
{
    int i,j;
    Init_State(XoodooState);

    Xoodoo_Initialize_Masks(XoodooMask);
    runrand();
    Absorb_Block(XoodooState, inputbuf);
    stack_clear();
    asm volatile (

        "push {r0-r7}\n"
        "movs r0, %0\n"                                                                                  
        "movs r1, %1\n"
        "mov r6, #0\n"                                                                                   
        "mov r5, #0\n"
        "mov r4, #0\n"
        "mov r3, #0\n"
        "mov r2, #0\n"
        "bl Xoodoo_Permute_12rounds\n"
        "pop {r0-r7}\n"                                                                                  
        :                                                                                                
        : "r" (XoodooState), "r" (XoodooMask)
        //: "r0", "r1", "r2", "r3", "r4", "r5", "r6"
        );
    //Xoodoo_Permute_12rounds(state, XoodooMask);
 
    for(j=0;j<PLT_SZ;j++)
    {
        printbyte(&XoodooState[j]);
    }
        
}


#define RUN_FUNC(...) \
    run_byte_xoodoo_masked_(__VA_ARGS__)


//static const uint8_t fixedinput[16] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t fixedkey[KEY_SZ] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0	};

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
            for (j=0;j<PLT_SZ;j++)
	    {
                readrandombyte(&input[j]);
                input[j] = fixedinput[j];
            }
        }
        else
        {
            for (j=0;j<PLT_SZ;j++)
            {    
                readrandombyte(&input[j]);
            }
        }
        run_byte_xoodoo_masked_(input, key);
    }
    endprogram();
}
