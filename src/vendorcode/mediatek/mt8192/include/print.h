/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef PRINT_H
#define PRINT_H

#include <console/console.h>

//int print(const char *fmt, ...);
#define print(_x_...)	printk(BIOS_INFO, _x_)
#define printf   print

#endif
