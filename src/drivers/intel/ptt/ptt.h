/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * This driver checks if the PTT Bit is set correctly within the FWSTS4
 * register. This is needed in order to use the iTPM, because we have to
 * check prior using the interface that this bit is set correctly - otherwise
 * it could work unpredictable. The bit should already be set if the Intel ME
 * is still in the preboot phase.
 *
 */
#include <stdint.h>
/*
 * ptt_active
 *
 * Checks if the Intel PTT is active. If PTT is active, returns true,
 * false otherwise.
 */
bool ptt_active(void);
