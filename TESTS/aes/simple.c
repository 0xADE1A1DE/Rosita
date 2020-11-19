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

#define NTRACES /*@NTRACES{*/ 20000 /*}*/
#define TEST_TRACES (NTRACES/2)

#include <stdio.h>
#include <stdlib.h>

#include "byte_mask_aes.h"
#include "../elmoasmfunctionsdef.h"
#define ALIGN __attribute__ ((aligned (4)))
/*
 * 4 byte align arrays to make sure we don't load invalid data
 * for state of byte-wise operations (ARM loads/stores 4 bytes
 * from addr & ~(0b11) when load/storing from address addr).
 * In ARM, Addresses that are not divisable by 4 are invalid when used
 * with wordwise instructions (ldr, str)
 */ 
volatile uint8_t input_copy[16] ALIGN;
volatile uint8_t input[16] ALIGN;
volatile uint8_t output[16] ALIGN;
volatile uint8_t randombuf[16] ALIGN;
volatile uint8_t key[16] ALIGN;
volatile uint8_t throwaway[16] ALIGN;
volatile uint8_t rk[11*16] ALIGN;
volatile int domask=1;


extern void runrand();

extern void AES_128_keyschedule(const uint8_t *, uint8_t *);
extern void AES_128_keyschedule_dec(const uint8_t *, uint8_t *);
extern void AES_128_encrypt(const uint8_t *, const uint8_t *, uint8_t *);
extern void AES_128_decrypt(const uint8_t *, const uint8_t *, uint8_t *);
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
void run_byte_aes_masked_(uint8_t* inputbuf, uint8_t* keybuf)
{
    int i,j;
    unsigned char testthrowaway;
/*
    readrandombyte(&testthrowaway);
    
    for(j=0;j<16;j++)
    {
        readrandombyte(&randombuf[j]);
    }*/
    for(j=0;j<16;j++)
    {
        output[j] = inputbuf[j];
        key[j] = keybuf[j];
    }

    readrandombyte(&testthrowaway);
    readrandombyte(&testthrowaway);
    readrandombyte(&testthrowaway);
    readrandombyte(&testthrowaway);
    readrandombyte(&testthrowaway);
    readrandombyte(&testthrowaway);
    
    runrand();
    
    KeyExpansion(key);

    randomize_mask();
    init_masking();
    maskstate(output);
#ifdef __arm__ 
    // clear stack and clear all other registers (leaving out r7)
    // to remove any state held from previous runs
    asm volatile (

        "bl stack_clear\n"
        "push {r0-r7}\n"
        "movs r0, %[value]\n" 
        "mov r6, #0\n"
        "mov r5, #0\n"
        "mov r4, #0\n"
        "mov r3, #0\n"
        "mov r2, #0\n"
        "mov r1, #0\n"
        "bl aes128\n"
        "pop {r0-r7}\n"
        :
        : [value] "r" (output));
#else
    stack_clear();
    aes128(output);
#endif
    
    for(j=0;j<16;j++)
    {
        printbyte(&output[j]);
    }
        
}


#define RUN_FUNC(...) \
    run_byte_aes_masked(__VA_ARGS__)

//static const uint8_t fixedinput[16] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t fixedkey[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};

static const uint8_t fixedinput[16] = { /*@FIXED_INPUT{*/ 0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90 /*}*/ };


void simple_init()
{
    int j=0;
    for(;j<16;j++)
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
        
        
        if (i<TEST_TRACES)
        {
            for (j=0;j<16;j++)
            {
                
                output[j] = 0x00;
                readrandombyte(&randombuf[j]);
                
                readrandombyte(&randombuf[j]);
                key[j] = fixedkey[j];
        
                input[j] = fixedinput[j];
            }
        }
        else
        {
            for (j=0;j<16;j++)
            {
               
                output[j] = 0x00;
                readrandombyte(&key[j]);
                
                readrandombyte(&randombuf[j]);
                
                input[j] = randombuf[j];
            }
        }
        
        run_byte_aes_masked_(input, key);

    }
    endprogram();

}
