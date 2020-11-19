/*
 * University of Bristol – Open Access Software Licence
 * Copyright (c) 2016, The University of Bristol, a chartered
 * corporation having Royal Charter number RC000648 and a charity
 * (number X1121) and its place of administration being at Senate
 * House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Any use of the software for scientific publications or commercial
 * purposes should be reported to the University of Bristol
 * (OSI-notifications@bristol.ac.uk and quote reference 2668). This is
 * for impact and usage monitoring purposes only.
 *
 * Enquiries about further applications and development opportunities
 * are welcome. Please contact elisabeth.oswald@bristol.ac.uk
 */

void getenergytrace(double *averagepower, int len){
    
    int i;
    double energytrace;
    FILE *fp;
    
    fp = fopen(ENERGYTRACEFILE, "w+");

    energy = 0;
    
    for(i=0;i<len;i++){
        energytrace = averagepower[i] * CLOCKCYCLETIME;
        fprintf(fp, "%0.20f\n", energytrace);
        energy += energytrace;

    }
}

//-------------------------------------------------------------------

void getenergy(void){
    
    FILE *fp;
    int i, len, *points, starttrace, endtrace;
    
    double *averagepower, sumaveragepower;
    
    sumaveragepower = 0;
    
#ifdef FIXEDVSRANDOM
    starttrace = tracenumber+1;
    endtrace = (tracenumber*2)+1;
#else
    starttrace = 1;
    endtrace = tracenumber+1;
#endif

    // Get trace length
    fp = loadtrace(1);
    tracelength = gettracelength(fp);
    instructions = tracelength;
    fclose(fp);

    averagepower = malloc(tracelength*sizeof(double));
    points = malloc(tracelength*sizeof(int));
    
    for (i=0;i<tracelength;i++){
        averagepower[i] = 0;
        points[i] = i;
    }
    
    getaverage(averagepower, tracelength, starttrace, endtrace, points, 1);
    getenergytrace(averagepower, tracelength);
    
}

//-------------------------------------------------------------------
