/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include "ppc.h"
#include "ppcreg.h"
#include <console/console.h>

void display_cpuid(void)
{
    unsigned type = __getpvr() >> 16;
    unsigned version = __getpvr() & 0xffff;
    const char *cpu_string = 0;
    switch(type) {
        case 1:
            cpu_string = "601";
            break;
        case 3:
            cpu_string = "603";
            break;
        case 4:
            cpu_string = "604";
            break;
        case 6:
            cpu_string = "603e";
            break;
        case 7:
            cpu_string = "603ev";
            break;
        case 8:
            cpu_string = "750";
            break;
        case 9:
            cpu_string = "604e";
            break;
        case 10:
            cpu_string = "604ev5 (MachV)";
            break;
        case 12:
            cpu_string = "7400";
            break;
        case 50:
            cpu_string = "821";
            break;
        case 80:
            cpu_string = "860";
            break;
	case 0x800c:
	    cpu_string = "7410";
	    break;
    }
    if (cpu_string)
        printk_info("PowerPC %s", cpu_string);       
    else
        printk_info("PowerPC unknown (0x%x)", type);
    printk_info(" CPU, version %d.%d\n", version >> 8, version & 0xff);       
}

