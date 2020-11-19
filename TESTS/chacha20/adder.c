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

#include <stdio.h>
#include <stdint.h>

typedef uint32_t u32;
void SecAndShift(u32 x0, u32 x1, u32 u, u32 i, u32* z0, u32* z1)
{
    u32 m = (x0>>1) ^ (u << 31);
    u32 y0 = x0 << i;
    u32 y1 = x1 << i;

    u32 s0 = x0 & y0;
    u32 s1 = x0 & y1;
    u32 s2 = x1 & y0;
    u32 s3 = x1 & y1;
    
    u32 t0 = s0 ^ m;
    u32 t1 = s1 ^ m;

    *z0 = t0 ^ s2;
    *z1 = t1 ^ s3;

}
void SecAnd(u32 x0, u32 x1, u32 y0, u32 y1, u32 u, u32* z0, u32* z1)
{
    u32 m = (x0>>1) ^ (u << 31);
    u32 s0 = x0 & y0;
    u32 s1 = x0 & y1;
    u32 s2 = x1 & y0;
    u32 s3 = x1 & y1;
    u32 t0 = s0 ^ m;
    u32 t1 = s1 ^ m;

    *z0 = t0 ^ s2;
    *z1 = t1 ^ s3;
}

void SecXor(u32 x0, u32 x1, u32 y0, u32 y1, u32* z0, u32* z1)
{
    *z0 = x0 ^ y0;
    *z1 = x1 ^ y1;
}

void SecAndShiftXor(u32 x0, u32 x1, u32 y0, u32 y1, u32 i, u32* z0, u32* z1)
{
    u32 v0 = y0<<i;
    u32 v1 = y1<<i;

    u32 s0 = x0 & v0;
    u32 s1 = x0 & v1;
    u32 s2 = x1 & v0;
    u32 s3 = x1 & v1;

    u32 t0 = s0 ^ y0;
    u32 t1 = s1 ^ y1;
    *z0 = t0 ^ s2;
    *z1 = t1 ^ s3;
}
void KSA(u32 x0, u32 x1, u32 y0, u32 y1, u32 u, u32* z0, u32* z1)
{
    u32 v = x0 % 2;
    u32 g0;
    u32 g1;
    SecAnd(x0, x1, y0, y1, u, &g0, &g1);
    u32 p0;
    u32 p1;
    SecXor(x0, x1, y0, y1, &p0, &p1);
    u = v;
    printf("*%x %x %x %x\n", p0, p1, g0, g1);
    for (u32 i=1;i<5;i++)
    {
        u32 b = 1U << (i-1);
        v = p0 % 2;
        SecAndShiftXor(p0, p1, g0, g1, b, &g0, &g1);
        printf("-%x %x %x\n", g0, g1, u);
        SecAndShift(p0, p1, u, b, &p0, &p1);
        printf("+%x %x\n", p0, p1);
        u = v;
    }
    u32 b = 1U << 4;
    SecAndShiftXor(p0, p1, g0, g1, b, &g0, &g1);
    *z0 = x0 ^ y0 ^ (g0 << 1);
    *z1 = x1 ^ y1 ^ (g1 << 1);
}
int main()
{
    u32 z0, z1, x0, x1, y0, y1;
    u32 u=0,i=16;
    //x0=1253465433; x1=34262554; y0=334435; y1=54634623;
    //x0=0x2b; x1=0x200004a4; y0=0x22003100; y1=0xa124a424;
//    x0=0x35; x1=0x200004a4; y0=0x1b36017; y1=0xb56b4050;
//x0=0x5; x1=0x5; y0=0x30000; y1=0x78666170;
x0=0x6; x1=0x61707863; y0=0x30004000; y1=0x66174786;
//SecAndShift(x0, x1, u, i , &z0, &z1);
    //SecAndShift(x0, x1, 8, 3, &z0, &z1);
//x0=0x5; x1=0x61707860; y0=0x5; y1=0x5;
//
    //SecAnd(x0, x1, y0, y1, u, &z0, &z1);
    KSA(x0, x1, y0, y1, u, &z0, &z1);
    printf("%x %x\n", z0, z1);
    return 0;
}
