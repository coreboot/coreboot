/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_CSE_H
#define SOC_INTEL_COMMON_CSE_H

#include <stdint.h>

/* set up device for use in early boot enviroument with temp bar */
void heci_init(uintptr_t bar);
/*
 * Receive message into buff not exceeding maxlen. Message is considered
 * successfully received if a 'complete' indication is read from ME side
 * and there was enough space in the buffer to fit that message. maxlen
 * is updated with size of message that was received. Returns 0 on failure
 * and 1 on success.
 * In case of error heci_reset() may be requiered.
 */
int heci_receive(void *buff, size_t *maxlen);
/*
 * Send message msg of size len to host from host_addr to cse_addr.
 * Returns 1 on success and 0 otherwise.
 * In case of error heci_reset() may be requiered.
 */
int
heci_send(const void *msg, size_t len, uint8_t host_addr, uint8_t cse_addr);
/*
 * Attempt device reset. This is useful and perhaps only thing left to do when
 * CPU and CSE are out of sync or CSE fails to respond.
 * Returns 0 on failure a 1 on success.
 */
int heci_reset(void);

#define BIOS_HOST_ADDR							0x00
#define HECI_MKHI_ADDR							0x07

#endif // SOC_INTEL_COMMON_MSR_H
