#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int msr(int cpu, unsigned int index, unsigned long *lo, unsigned long *hi)
{
        char cpuname[16];
        unsigned char buffer[8];
        int fh;

        sprintf (cpuname, "/dev/cpu/%d/msr", cpu);

        fh = open (cpuname, O_RDONLY);
        if (fh==-1)
                return (0);

        lseek (fh, index, SEEK_CUR);

        if (fh != -1) {

                if (read (fh, &buffer[0], 8) != 8) {
                        close (fh);
                        return (0);
                }

                *lo = (*(unsigned long *)buffer);
                *hi = (*(unsigned long *)(buffer+4));
        }
        close (fh);
        return (1);
}


int main (void)
{
        unsigned long lo=0, hi=0;
        unsigned int index;

        for (index=0x00000000; index<0xFFFFFFFF; index++) {
                if ((index % 0x10000) == 0)
                        //printf("%#08x\b\b\b\b\b\b\b\b\r", index);
                        printf("%#08x\n", index);
                if (msr(0, index, &lo, &hi) == 1)
                        printf ("MSR: %lx, %08lx %08lx\n", index, lo, hi);
        }
        return (0);
}
