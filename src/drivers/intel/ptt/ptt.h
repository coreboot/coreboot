/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
