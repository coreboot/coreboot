/*  COPYING:
 *  Dave Patterson at UCB modified this code from a program by Andrea 
 *  Dusseau of U.C. Berkeley,which was based on a description in 
 *  Saavedra-Barrera [1992]:
 *    Saavedra-Barrera, R. H. [1992]. CPU Performance Evaluation and 
 *    Execution Time Prediction Using Narrow Spectrum Benchmarking, 
 *    Ph.D. Dissertation, University of Calif., Berkeley (May).
 *  Patterson has given permission to use and modify this code as long
 *  as these people and the U. of Cal., Berkeley get proper credit.
 */

#include <stdio.h>
#include <sys/times.h>
#include <sys/types.h>
#include <time.h>
#define CACHE_MIN (1024) /* smallest cache */
#define CACHE_MAX (256*1024) /* largest cache */
#define SAMPLE 10 /* to get a larger time sample */
#ifndef CLK_TCK
#define CLK_TCK 60 /* number clock ticks per second */
#endif
int x[CACHE_MAX]; /* array going to stride through */
 
double get_seconds() { /* routine to read time */
    struct tms rusage;
    times(&rusage); /* UNIX utility: time in clock ticks */
    return (double) (rusage.tms_utime)/CLK_TCK;
}

int main() {
    int register i, index, stride, limit, temp;
    int steps, tsteps, csize;
    double sec0, sec; /* timing variables */
 
    for (csize=CACHE_MIN; csize <= CACHE_MAX; csize=csize*2) 
        for (stride=1; stride <= 128; stride=stride*2) {
            sec = 0; /* initialize timer */
            limit = csize-stride+1; /* cache size this loop */
     
            steps = 0;
            do { /* repeat until collect 1 second */
                sec0 = get_seconds(); /* start timer */
                for (i=SAMPLE*stride;i!=0;i=i-1) /* larger sample */
                    for (index=0; index < limit; index=index+stride) 
                        x[index] = x[index] + 1; /* cache access */
                steps = steps + 1; /* count while loop iterations */
                sec = sec + (get_seconds() - sec0);/* end timer */

            } while (sec < 1.0); /* until collect 1 second */
     
            /* Repeat empty loop to loop subtract overhead */
            tsteps = 0; /* used to match no. while iterations */
            do { /* repeat until same no. iterations as above */
                sec0 = get_seconds(); /* start timer */
                for (i=SAMPLE*stride;i!=0;i=i-1) /* larger sample */
                    for (index=0; index < limit; index=index+stride) 
                        temp = temp + index; /* dummy code */
                tsteps = tsteps + 1; /* count while iterations */
                sec = sec - (get_seconds() - sec0);/* - overhead */
            } while (tsteps<steps); /* until = no. iterations */
     
            printf("Size (bytes):%7d, Stride (bytes):%4d, read+write:%4.0f
ns\n",
               csize*sizeof(int), stride*sizeof(int),
               (double) sec*1e9/(steps*SAMPLE*stride*((limit-1)/stride+1)));
        }; /* end of both outer for loops */
}

