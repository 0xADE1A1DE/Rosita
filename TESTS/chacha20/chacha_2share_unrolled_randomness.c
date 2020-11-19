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

#include <stdint.h>
#define __add(_op1, _op2) \
    "add " #_op1 "," #_op2 "\n\t"
#define __sub(_op1, _op2) \
    "sub " #_op1 "," #_op2 "\n\t"
#define __add_imm(_op1, _op2) \
    "add " #_op1 ", #" #_op2 "\n\t"
#define __sub_imm(_op1, _op2) \
    "sub " #_op1 ", #" #_op2 "\n\t"
#define __ldr(_op1, _op2) \
    "ldr " #_op1 ",[" #_op2 "]\n\t"
#define __str(_op1, _op2) \
    "str " #_op1 ",[" #_op2 "]\n\t"
#define __mov(_op1, _op2) \
    "mov " #_op1 "," #_op2 "\n\t"
#define __lsl(_op1, _op2) \
    "lsl " #_op1 "," #_op2 "\n\t"
#define __lsr(_op1, _op2) \
    "lsr " #_op1 "," #_op2 "\n\t"
#define __lsl_imm(_op1, _op2) \
    "lsl " #_op1 ", #" #_op2 "\n\t"
#define __lsr_imm(_op1, _op2) \
    "lsr " #_op1 ", #" #_op2 "\n\t"
#define __and(_op1, _op2) \
    "and " #_op1 "," #_op2 "\n\t"
#define __eor(_op1, _op2) \
    "eor " #_op1 "," #_op2 "\n\t"

#define PRINT_SS \
                  "sub sp, #4\n\t" \
                  "push {r0-r11}\n\t" \
                          "ldr r3, =z0\n\t" \
                          "str r0, [r3]\n\t" \
                          "ldr r3, =z1\n\t" \
                          "str r1, [r3]\n\t" \
                  "bl print_ret\n\t" \
                  "pop {r0-r11}\n\t"\
                  "add sp, #4\n\t"
#define PRINT_II \
                  "sub sp, #4\n\t" \
                  "push {r0-r11}\n\t" \
                          "ldr r5, =x0\n\t" \
                          "str r0, [r5]\n\t" \
                          "ldr r5, =x1\n\t" \
                          "str r1, [r5]\n\t" \
                          "ldr r5, =y0\n\t" \
                          "str r2, [r5]\n\t" \
                          "ldr r5, =y1\n\t" \
                          "str r3, [r5]\n\t" \
                  "bl print_shares\n\t" \
                  "pop {r0-r11}\n\t"\
                  "add sp, #4\n\t"

#define doubleround(_x0,_x1,_y0,_y1,_t0,_t1) \
    /* =-=-=    QUARTERROUND 0    =-=-= */ \
    ".align 2\n\t" \
    "ti_doubleround:\n\t"             /* TI doubleround function                          */ \
    "push {lr}\n\t" \
    "push {r0-r6}\n\t" \
    "bl starttrigger\n\t" \
    "pop {r0-r6}\n\t" \
          "mov " #_t1 ", sp\n\t" \
    "add " #_t1 ", " #_t1 ", #4\n\t" \
 \
    "ldr " #_y1 ", [" #_t1 ",  #48]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #16]\n\t"        /* load share a0                                    */ \
 \
    "ldr " #_y0 ", [" #_t1 ",  #32]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",   #0]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a=a+b          */ \
 \
    "str " #_x1 ", [" #_t1 ",  #16]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #112]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #112]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #80]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #0]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #96]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ",  #96]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #64]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #80]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #48]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #48]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #16]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #64]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #32]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #32]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",   #0]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #16]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #112]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #112]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #80]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #0]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #96]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ",  #96]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #64]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #80]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #48]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #48]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #52]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #20]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #64]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #32]\n\t"      /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #32]\n\t"      /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #36]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",  #4]\n\t"         /* load share a1                                    */ \
 \
    "push {r0-r6}\n\t" \
    "bl endtrigger\n\t" \
    "pop {r0-r6}\n\t" \
    /* =-=-=    QUARTERROUND 1    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #20]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #116]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #116]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #84]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #4]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #100]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #100]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #68]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #84]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #52]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #52]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #20]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #68]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #36]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #36]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",   #4]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #20]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #116]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #116]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #84]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #4]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #100]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #100]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #68]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #84]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #52]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #52]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #56]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #24]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #68]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #36]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #36]\n\t"        /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #40]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",   #8]\n\t"        /* load share a1                                    */ \
 \
    /* =-=-=    QUARTERROUND 2    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #24]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #120]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #120]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #88]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #8]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #104]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #104]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #72]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #88]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #56]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #56]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #24]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #72]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #40]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #40]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",   #8]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #24]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #120]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 8                            */ \
    "str " #_y1 ", [" #_t1 ", #120]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #88]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #8]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #104]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 8                            */ \
    "str " #_y0 ", [" #_t1 ", #104]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #72]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #88]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #56]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 7                            */ \
    "str " #_y1 ", [" #_t1 ",  #56]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #60]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #28]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #72]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #40]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 7                            */ \
    "str " #_y0 ", [" #_t1 ",  #40]\n\t"        /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #44]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",  #12]\n\t"        /* load share a1                                    */ \
 \
    /* =-=-=    QUARTERROUND 3    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #28]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #124]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #124]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #92]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #12]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #108]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #108]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #76]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #92]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #60]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #60]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #28]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #76]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #44]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #44]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #12]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #28]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #124]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_x1 ", " #_y1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #124]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #92]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #12]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #108]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #108]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #76]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #92]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #60]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #60]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #52]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #16]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #76]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #44]\n\t"      /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #44]\n\t"        /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #36]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",   #0]\n\t"      /* load share a1                                    */ \
 \
    /* =-=-=    QUARTERROUND 4    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #16]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #124]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #124]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #88]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #0]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #108]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #108]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #72]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ", #88]\n\t"         /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #52]\n\t"         /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ", #52]\n\t"         /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ", #16]\n\t"         /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ", #72]\n\t"         /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #36]\n\t"         /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ", #36]\n\t"         /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #0]\n\t"         /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #16]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #124]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #124]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #88]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #0]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #108]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #108]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #72]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #88]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #52]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #52]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #56]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #20]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #72]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #36]\n\t"      /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #36]\n\t"        /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #40]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",   #4]\n\t"      /* load share a1                                    */ \
 \
    /* =-=-=    QUARTERROUND 5    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #20]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #112]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #112]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #92]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #4]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #96]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ",  #96]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #76]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #92]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #56]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #56]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #20]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #76]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #40]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #40]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",   #4]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #20]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #112]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #112]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #92]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #4]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #96]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ",  #96]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #76]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #92]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #56]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #56]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #60]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #24]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #76]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #40]\n\t"      /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #40]\n\t"        /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #44]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",   #8]\n\t"      /* load share a1                                    */ \
 \
    /* =-=-=    QUARTERROUND 6    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #24]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #116]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #116]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #80]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #8]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #100]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #100]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #64]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #80]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #60]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #60]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #24]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #64]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #44]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #44]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",   #8]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #24]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #116]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #116]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #80]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",   #8]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #100]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #100]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #64]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #80]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #60]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #60]\n\t"        /* store share b0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #48]\n\t"        /* load share b0                                    */ \
    "ldr " #_x1 ", [" #_t1 ",  #28]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #64]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #44]\n\t"      /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #44]\n\t"        /* store share b1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #32]\n\t"        /* load share b1                                    */ \
    "ldr " #_x0 ", [" #_t1 ",  #12]\n\t"      /* load share a1                                    */ \
 \
    /* =-=-=    QUARTERROUND 7    =-=-= */ \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #28]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #120]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #120]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #84]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #12]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #104]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #16\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #104]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #68]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #84]\n\t"        /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ",  #48]\n\t"        /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ",  #48]\n\t"        /* store share b0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #28]\n\t"        /* load share a0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #68]\n\t"        /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ",  #32]\n\t"        /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #20\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ",  #32]\n\t"        /* store share b1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #12]\n\t"        /* load share a1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute a = a + b      */ \
 \
    "str " #_x1 ", [" #_t1 ",  #28]\n\t"        /* store share a0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #120]\n\t"        /* load share d0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute d0 = a0 ^ d0                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute d0 = d0 <<< 16                           */ \
    "str " #_y1 ", [" #_t1 ", #120]\n\t"        /* store share d0                                   */ \
    "ldr " #_x1 ", [" #_t1 ",  #84]\n\t"        /* load share c0                                    */ \
 \
    "str " #_x0 ", [" #_t1 ",  #12]\n\t"        /* store share a1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #104]\n\t"        /* load share d1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute d1 = a1 ^ d1                             */ \
          "mov " #_t0 ", #24\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute d1 = d1 <<< 16                           */ \
    "str " #_y0 ", [" #_t1 ", #104]\n\t"        /* store share d1                                   */ \
    "ldr " #_x0 ", [" #_t1 ",  #68]\n\t"        /* load share c1                                    */ \
 \
    "bl ti_add\n\t"                   /* call addition function to compute c = c + d      */ \
 \
    "str " #_x1 ", [" #_t1 ", #84]\n\t"         /* store share c0                                   */ \
    "ldr " #_y1 ", [" #_t1 ", #48]\n\t"         /* load share b0                                    */ \
    "eor " #_y1 ", " #_y1 ", " #_x1 "\n\t"        /* compute b0 = c0 ^ b0                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y1 ", " #_y1 ", " #_t0 "\n\t"         /* compute b0 = b0 <<< 12                           */ \
    "str " #_y1 ", [" #_t1 ", #48]\n\t"         /* store share b0                                   */ \
 \
    "str " #_x0 ", [" #_t1 ", #68]\n\t"         /* store share c1                                   */ \
    "ldr " #_y0 ", [" #_t1 ", #32]\n\t"       /* load share b1                                    */ \
    "eor " #_y0 ", " #_y0 ", " #_x0 "\n\t"        /* compute b1 = c1 ^ b1                             */ \
          "mov " #_t0 ", #25\n\t" \
    "ror " #_y0 ", " #_y0 ", " #_t0 "\n\t"         /* compute b1 = b1 <<< 12                           */ \
    "str " #_y0 ", [" #_t1 ", #32]\n\t"         /* store share b1                                   */ \
 \
    "pop {pc}\n\t" 


/* Input arguments never get written to the temporary arguments can be used
 * again */
/*verified*/
#define SecAndShiftXor(x0, x1, y0, y1, i,   /* inputs */    \
                       z0, z1,                  /* outputs */   \
                       t0, t1, t2)              /* temps */     \
    __mov (t0, y0)                                  \
    __lsl (t0, i)    /* ret v0 */                    \
    __mov (t1, y1)                                  \
    __lsl (t1, i)    /* ret v1 */                    \
                                                    \
    __mov (z0, x0)                                   \
    __and (z0, t0)    /* ret s0 */                   \
                                                    \
    __mov (t2, y0)                                     \
    __eor (z0, t2) /* ret t0 */                   \
                                                    \
    __mov (t2, x1)                                \
    __and (t2, t0) /* ret s2 */                   \
                                                    \
    __eor (z0, t2) /* ret z0 */                   \
                                                    \
    __mov (t2, x0)                                 \
    __and (t2, t1) /* ret s1 */                   \
                                                    \
    __mov (z1, y1)                                \
    __eor (z1, t2) /* ret t1 */                  \
                                                    \
    __mov (t0, x1)                                \
    __and (t0, t1)  /* ret s3 */                   \
                                                    \
    __eor (z1, t0) /* ret z1 */

#define SecXor(x0, x1, y0, y1, p0, p1, t0, t1) \
    __mov (p0, x0) \
    __mov (p1, x1) \
    __mov (t0, y0) \
    __mov (t1, y1) \
    __eor (p0, t0) \
    __eor (p1, t1)
/*verified*/
#define SecAnd(x0, x1, y0, y1, u, z0, z1, t0, t1, t2, t3) \
    /* z0 as (x0 >> 1) */       \
    __mov (z0, x0)              \
    __lsr_imm(z0, 1)            \
    /* z1 as (u << (k-1)) */    \
    __mov (z1, u)               \
    __lsl_imm(z1, 31)           \
    __eor (z1, z0) /* ret m */  \
                                \
    __mov (z0, x0)              \
    __mov (t0, y0)              \
    __and (t0, z0)              \
    __eor (t0, z1) /* ret s0 ^ m */ \
                                    \
    __mov (z0, x1)                  \
    __mov (t2, y0)                  \
    __and (t2, z0) /* ret s2 */     \
                                    \
    __eor (t0, t2) /* ret z0 */     \
    __mov (z0, t0)                  \
                                    \
    __mov (t1, x0)                  \
    __mov (t0, y1)          \
    __and (t1, t0)                  \
    __eor (z1, t1) /* ret t1 */ \
                                    \
    __mov (t3, x1)  \
    __and (t3, t0)      \
    __eor (z1, t3)
/* verified */
#define SecAndShift(x0, x1, u, i, z0, z1, t0, t1, t2) \
    /* z0 as (x0 >> 1) */       \
    __mov (z0, x0)              \
    __lsr_imm(z0, 1)            \
    /* z1 as (u << (k-1)) */    \
    __mov (z1, u)               \
    __lsl_imm(z1, 31)           \
    __eor(z1, z0)   /* ret m */ /* fix algo 5 in paper */ \
    \
    __mov(z0, x0) \
    __lsl(z0, i) /* ret y0 */ \
    __mov(t0, x1) \
    __lsl(t0, i) /* ret y1 */ \
    \
    __mov(t1, x0) \
    __and(t1, z0) /* ret s0 */ \
    __eor(t1, z1) /* ret t0 */ \
    \
    __mov(t2, x1) \
    __and(z0, t2) /* ret s2 */ \
    __eor(z0, t1) /* ret z0 */ \
    \
    __mov(t1, x0) \
    __and(t1, t0) /* ret s1*/ \
    __eor(z1, t1) /* ret t1*/ \
    \
    __mov(t1, x1) \
    __and(t1, t0) /* ret s3 */ \
    __eor(z1, t1) /* ret z1 */
#define __ldmdb(base, z0, z1, z2, z3)\
    __sub_imm(base, 4) \
    __ldr(z0, base) \
    __sub_imm(base, 4) \
    __ldr(z1, base) \
    __sub_imm(base, 4) \
    __ldr(z2, base) \
    __sub_imm(base, 4) \
    __ldr(z3, base)
#define __stm(base, z0, z1, z2, z3)\
    __str(z0, base) \
    __add_imm(base, 4) \
    __str(z1, base) \
    __add_imm(base, 4) \
    __str(z2, base) \
    __add_imm(base, 4) \
    __str(z3, base) \
    __add_imm(base, 4)
uint32_t x0,x1,y0,y1;
uint32_t z0,z1;
uint32_t m16=123;

void chacha_local_loop_2share_unrolled_randomness(uint32_t* state, uint32_t* masks)
{
  asm volatile(
    "push {r0}\n\t"                  /* store location of original state */
    "push {r1}\n\t"                  /* store location of original state */

    "mov r10, r1\n\t"

    "add r0, r0, #64\n\t"            /* load the last word first, so manipulate r0 */
    "add r1, r1, #64\n\t"            /* load the last word first, so manipulate r0 */
          "ldr r2,=m16\n\t"
          "ldr r6, [r1]\n\t"
          "str r6, [r2]\n\t"
          "b gg\n\t"
          ".ltorg\n\t"
    //"mov r11, sp\n\t"                 /* store current stack pointer to r7 */  /** save stack */
    ".align 2\n\t"
"gg:\n\t"
    /* Stack organization is as follows:

    s[15], s[14], s[13], s[12] --> load 1st s[15] -> s2, s[14] -> s3, s[13] -> s4, s[12] -> s5
    s[11], s[10], s[ 9], s[ 8] --> load 2nd s[11] -> s2, s[10] -> s3, s[ 9] -> s4, s[ 8] -> s5
    s[ 7], s[ 6], s[ 5], s[ 4] --> load 3rd s[ 7] -> s2, s[ 6] -> s3, s[ 5] -> s4, s[ 4] -> s5
    s[ 3], s[ 2], s[ 1], s[ 0] --> load 4th s[ 3] -> s2, s[ 2] -> s3, s[ 1] -> s4, s[ 0] -> s5
                         ^^^^^
                         sp (in the beginning)
    push masked state (1):
    oms0[15] +252, oms0[14] +248, oms0[13] +244, oms0[12] +240 d
    oms1[15] +236, oms1[14] +232, oms1[13] +212, oms1[12] +216
    push masked state (2):
    oms0[11] +220, oms0[10] +216, oms0[ 9] +212, oms0[ 8] +208 c
    oms1[11] +204, oms1[10] +200, oms1[ 9] +196, oms1[ 8] +192
    push masked state (3):
    oms0[ 7] +188, oms0[ 6] +184, oms0[ 5] +180, oms0[ 4] +176 b
    oms1[ 7] +172, oms1[ 6] +168, oms1[ 5] +164, oms1[ 4] +160
    push masked state (4):
    oms0[ 3] +156, oms0[ 2] +152, oms0[ 1] +148, oms0[ 0] +144 a
    oms1[ 3] +140, oms1[ 2] +136, oms1[ 1] +132, oms1[ 0] +128
                                                 ^^^^^^^
                                                 sp (after masking)
    copy masked state (1):
    ms0[15] +124, ms0[14] +120, ms0[13] +116, ms0[12] +112 d
    ms1[15] +108, ms1[14] +104, ms1[13] +100, ms1[12]  +96
    copy masked state (2):
    ms0[11]  +92, ms0[10]  +88, ms0[ 9]  +84, ms0[ 8]  +80 c
    ms1[11]  +76, ms1[10]  +72, ms1[ 9]  +68, ms1[ 8]  +64
    copy masked state (3):
    ms0[ 7]  +60, ms0[ 6]  +56, ms0[ 5]  +52, ms0[ 4]  +48 b
    ms1[ 7]  +44, ms1[ 6]  +40, ms1[ 5]  +36, ms1[ 4]  +32
    copy masked state (4):
    ms0[ 3]  +28, ms0[ 2]  +24, ms0[ 1]  +20, ms0[ 0]  +16 a
    ms1[ 3]  +12, ms1[ 2]   +8, ms1[ 1]   +4, ms1[ 0]   +0
                                               ^^^^^^^
                                               sp (after copying)
    */
          /** copy state and masks to stack*/
          "loop_1:"
          __ldmdb(r0, r5, r4, r3, r2) /*state*/
          "push {r2-r5}\n\t"
          __ldmdb(r1, r5, r4, r3, r2) /*masks*/
          "push {r2-r5}\n\t"
          "cmp r1, r10\n\t"
          "bne loop_1\n\t"

            "add r0, r0, #64\n\t"
            "add r1, r1, #64\n\t"

          "loop_2:"
          __ldmdb(r0, r5, r4, r3, r2) /*state*/
          "push {r2-r5}\n\t"
          __ldmdb(r1, r5, r4, r3, r2) /*masks*/
          "push {r2-r5}\n\t"
          "cmp r1, r10\n\t"
          "bne loop_2\n\t"


    "bl ti_doubleround\n\t"            /* call double round function 0 */
    // "bl ti_doubleround\n\t"            /* call double round function 1 */
    // "bl ti_doubleround\n\t"            /* call double round function 2 */
    // "bl ti_doubleround\n\t"            /* call double round function 3 */
    // "bl ti_doubleround\n\t"            /* call double round function 4 */
    // "bl ti_doubleround\n\t"            /* call double round function 5 */
    // "bl ti_doubleround\n\t"            /* call double round function 6 */
    // "bl ti_doubleround\n\t"            /* call double round function 7 */
    // "bl ti_doubleround\n\t"            /* call double round function 8 */
    // "bl ti_doubleround\n\t"            /* call double round function 9 */


    /* macro for the final addition to remove code duplication                            */
    ".macro final_add nr, a, b, c, d, e, f x0, x1, y0, y1\n\t"

    ".align 2\n\t"
    "ti_final_add\\nr:\n\t"           /* final addition 0                                 */
    "str \\x0, [sp, \\e]\n\t"         /* store final share oms0[e]                        */
    "ldr \\x0, [sp, \\a]\n\t"         /* load share  ms0[a]                               */
    "ldr \\y0, [sp, \\c]\n\t"         /* load share oms0[c]                               */
    "str \\x1, [sp, \\f]\n\t"         /* store final share oms1[f]                        */
    "ldr \\x1, [sp, \\b]\n\t"         /* load share  ms1[b]                               */
    "ldr \\y1, [sp, \\d]\n\t"         /* load share oms1[d]                               */
    "bl ti_add\n\t"                   /* call TI addition                                 */

    ".endm\n\t"

    /* the first final addition is special, since it does not have to store anything.     */
    ".align 2\n\t"
    "ti_final_add0:\n\t"              /* final addition 0                                 */
    "ldr r0, [sp, #16]\n\t"           /* load share  ms0[ 0]                              */
    "ldr r2, [sp,#144]\n\t"           /* load share oms0[ 0]                              */
    "ldr r1, [sp,  #0]\n\t"           /* load share  ms1[ 0]                              */
    "ldr r3, [sp,#128]\n\t"           /* load share oms1[ 0]                              */
    "bl ti_add\n\t"                   /* call TI addition                                 */

    /* addresses of: x0,   x1,   y0,   y1, o_y0, o_y1                                     */
    "final_add  1,  #20,   #4, #148, #132,  #16,   #0, r0, r1, r2, r3\n\t"
    "final_add  2,  #24,   #8, #152, #136,  #20,   #4, r0, r1, r2, r3\n\t"
    "final_add  3,  #28,  #12, #156, #140,  #24,   #8, r0, r1, r2, r3\n\t"
    "final_add  4,  #48,  #32, #176, #160,  #28,  #12, r0, r1, r2, r3\n\t"
    "final_add  5,  #52,  #36, #180, #164,  #48,  #32, r0, r1, r2, r3\n\t"
    "final_add  6,  #56,  #40, #184, #168,  #52,  #36, r0, r1, r2, r3\n\t"
    "final_add  7,  #60,  #44, #188, #172,  #56,  #40, r0, r1, r2, r3\n\t"
    "final_add  8,  #80,  #64, #208, #192,  #60,  #44, r0, r1, r2, r3\n\t"
    "final_add  9,  #84,  #68, #212, #196,  #80,  #64, r0, r1, r2, r3\n\t"
    "final_add 10,  #88,  #72, #216, #200,  #84,  #68, r0, r1, r2, r3\n\t"
    "final_add 11,  #92,  #76, #220, #204,  #88,  #72, r0, r1, r2, r3\n\t"
    "final_add 12, #112,  #96, #240, #224,  #92,  #76, r0, r1, r2, r3\n\t"
    "final_add 13, #116, #100, #244, #228, #112,  #96, r0, r1, r2, r3\n\t"
    "final_add 14, #120, #104, #248, #232, #116, #100, r0, r1, r2, r3\n\t"
    "final_add 15, #124, #108, #252, #236, #120, #104, r0, r1, r2, r3\n\t"



                                     /* unmask the state and store to original memory location */
    "str r0, [sp,#124]\n\t"          /* store final share s15_0 */
    "str r1, [sp,#108]\n\t"          /* store final share s15_1 */

    "ldr r0, [sp, #260]\n\t"         /* load location of original state to r0 */
    "ldr r1, [sp, #256]\n\t"         /* load location of original state to r0 */

                  "movs r6, r1\n\t"
                  "add r6, #64\n\t"
                  "loop_3:"
                  "pop {r2-r5}\n\t"
                  //"stmib r0!, {r2,r3,r4,r5}\n\t" /*state*/
                  __stm(r0, r2,r3,r4,r5)
                  "pop {r2-r5}\n\t"
                  __stm(r1, r2,r3,r4,r5)
                  //"stmib r1!, {r2,r3,r4,r5}\n\t" /*masks*/
                  //"add r1, #16\n\t"
                  //"add r0, #16\n\t"
                  "cmp r1, r6\n\t"
                  "bne loop_3\n\t"


    "add sp, sp, #136\n\t"           /* correct stack pointer */

    "b ti_chacha_end\n\t"            /* end ChaCha20 TI */



    doubleround(r0,r1,r2,r3,r5,r6)     /* instantiate doubleround macro with specific
                                         register scheduling                              */

    /* ========= TI ADD ========= */
    ".align 2\n\t"
    "ti_add:\n\t"
    /* r12 = xx
     * r11 = xx
     * r10 = xx
     * r9 = xx
     * r8 = additional mask location (masks[16])
     * r7 = xx
     * r6 = xx
     * r5 = xx
     * r4 = xx
     * r3 = y1
     * r2 = y0
     * r1 = x1
     * r0 = x0
     * */
          /* algorithm 8 */
          /* load u */
          //"ldr r12, [r8]\n\t"
                  "push {r6,lr}\n\t"

          "ldr r5, =m16\n\t"
          "ldr r5, [r5]\n\t"
          "mov r12, r5\n\t"

          "mov r8, r0\n\t"
          "mov r9, r1\n\t"
          "mov r10, r2\n\t"
          "mov r11, r3\n\t"

          /* store shares to memory */
          "ldr r4, =x0\n\t"
          "str r0, [r4]\n\t"
          "ldr r4, =y0\n\t"
          "str r2, [r4]\n\t"
          "ldr r4, =x1\n\t"
          "str r1, [r4]\n\t"
          "ldr r4, =y1\n\t"
          "str r3, [r4]\n\t"
          SecAnd(r8, r9, r10, r11, r12, r0, r1, r2, r3, r4, r5)

          SecXor(r8, r9, r10, r11, r2, r3, r4, r5)
          "mov r4, r8\n\t"
          "mov r5, #1\n\t"
          "and r4, r5\n\t"
          "mov r12, r4\n\t" /* u =v */

          /* g0 = r8, g1 = r9, p0 = r10, p1 = r11 */
          "mov r8, r0\n\t"
          "mov r9, r1\n\t"
          "mov r10, r2\n\t"
          "mov r11, r3\n\t"

          /* begin loop */
          /* i = 1 */
          "mov r5, #1\n\t"
          "mov r6, r10\n\t"
          "mov r4, #1\n\t"
          "and r6, r4\n\t"
          SecAndShiftXor(r10, r11, r8, r9, r5 /*i*/,r0, r1, r2, r3, r4)
          "mov r8, r0\n\t"
          "mov r9, r1\n\t"

          SecAndShift(r10, r11, r12, r5, r0, r1, r2, r3, r4)
          "mov r10, r0\n\t"
          "mov r11, r1\n\t"

          "mov r12, r6\n\t" /* update u */

          /* i = 2 */
          "mov r5, #2\n\t"
          "mov r6, r10\n\t"
          "mov r4, #1\n\t"
          "and r6, r4\n\t"
          SecAndShiftXor(r10, r11, r8, r9, r5 ,r0, r1, r2, r3, r4)
          "mov r8, r0\n\t"
          "mov r9, r1\n\t"

          SecAndShift(r10, r11, r12, r5, r0, r1, r2, r3, r4)
          "mov r10, r0\n\t"
          "mov r11, r1\n\t"

          "mov r12, r6\n\t" /* update u */

          /* i = 3 */
          "mov r5, #4\n\t"
          "mov r6, r10\n\t"
          "mov r4, #1\n\t"
          "and r6, r4\n\t"
          SecAndShiftXor(r10, r11, r8, r9, r5 ,r0, r1, r2, r3, r4)
          "mov r8, r0\n\t"
          "mov r9, r1\n\t"
          SecAndShift(r10, r11, r12, r5, r0, r1, r2, r3, r4)
          "mov r10, r0\n\t"
          "mov r11, r1\n\t"
          "mov r12, r6\n\t" /* update u */

          /* i = 4 */
          "mov r5, #8\n\t"
          "mov r6, r10\n\t"
          "mov r4, #1\n\t"
          "and r6, r4\n\t"
          SecAndShiftXor(r10, r11, r8, r9, r5 ,r0, r1, r2, r3, r4)
          "mov r8, r0\n\t"
          "mov r9, r1\n\t"
          SecAndShift(r10, r11, r12, r5, r0, r1, r2, r3, r4)
          "mov r10, r0\n\t"
          "mov r11, r1\n\t"
          "mov r12, r6\n\t" /* update u */
          /* end loop */

          "mov r5, #16\n\t"
          SecAndShiftXor(r10, r11, r8, r9, r5 ,r0, r1, r2, r3, r4)

          /* load from memory */
          "ldr r2, =x0\n\t"
          "ldr r2, [r2]\n\t"
          
          "ldr r4, =y0\n\t"
          "ldr r4, [r4]\n\t"

          "ldr r3, =x1\n\t"
          "ldr r3, [r3]\n\t"
          
          "ldr r5, =y1\n\t"
          "ldr r5, [r5]\n\t"

          "lsl r0, #1\n\t" /* 2*g0 */
          "lsl r1, #1\n\t" /* 2*g1 */

          "eor r0, r4\n\t"
          "eor r0, r2\n\t"

          "eor r1, r5\n\t"
          "eor r1, r3\n\t"

          "ldr r4, =m16\n\t"
          "mov r5, r12\n\t"
          "str r5, [r4]\n\t"
          /*"ldr r3, =z0\n\t"
          "str r0, [r3]\n\t"
          "ldr r3, =z1\n\t"
          "str r1, [r3]\n\t"
          "push {r0-r12}\n\t"
          "b print_ret\n\t"
          "pop {r0-r12}\n\t"*/
                  "pop {r6,pc}\n\t"
    //"bx lr\n\t"                     /* return */

    "ti_chacha_end:"
      ".ltorg\n\t"
    : : [r0] "r" (state), [r1] "r" (masks) :
       "r2", "r3", "r4", "r5", "r6","memory");
}
