/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2008 Peter Stuge <peter@stuge.se>
 * Copyright (c) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <pci/pci.h>

#include "msrtool.h"

static struct cpuid_t id;

struct cpuid_t *cpuid(void) {
	uint32_t outeax;
	uint32_t outebx;

/* First, we need determine which vendor we have */
#if defined(__DARWIN__) && !defined(__LP64__)
	asm volatile (
		"pushl %%ebx    \n"
		"cpuid          \n"
		"popl %%ebx     \n"
		: "=b" (outebx) : "a" (0) : "%ecx", "%edx"
	);
#else
	asm ("cpuid" : "=b" (outebx) : "a" (0) : "%ecx", "%edx");
#endif

	id.vendor = outebx;

/* Then, identificate CPU itself */
#if defined(__DARWIN__) && !defined(__LP64__)
	asm volatile (
		"pushl %%ebx    \n"
		"cpuid          \n"
		"popl %%ebx     \n"
		: "=a" (outeax) : "a" (1) : "%ecx", "%edx"
	);
#else
	asm ("cpuid" : "=a" (outeax) : "a" (1) : "%ebx", "%ecx", "%edx");
#endif

	id.stepping = outeax & 0xf;
	outeax >>= 4;
	id.model = outeax & 0xf;
	outeax >>= 4;
	id.family = outeax & 0xf;
	outeax >>= 8;
	id.ext_model = outeax & 0xf;
	outeax >>= 4;
	id.ext_family = outeax & 0xff;
	if ((0xf == id.family) || ((VENDOR_INTEL == id.vendor)
			&& (0x6 == id.family))) {
		/* Intel says always do this, AMD says only for family f */
		id.model |= (id.ext_model << 4);
		id.family += id.ext_family;
	}
	printf_verbose("CPU: family %x, model %x, stepping %x\n",
			id.family, id.model, id.stepping);

	return &id;
}

struct pci_dev *pci_dev_find(uint16_t vendor, uint16_t device) {
	struct pci_dev *temp;
	struct pci_filter filter;

	pci_filter_init(NULL, &filter);
	filter.vendor = vendor;
	filter.device = device;

	for (temp = pacc->devices; temp; temp = temp->next)
		if (pci_filter_match(&filter, temp))
			return temp;

	return NULL;
}
