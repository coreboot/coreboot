/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef OPENSIL_DRIVER_H
#define OPENSIL_DRIVER_H

/* Set up openSIL env and call TP1 */
void amd_opensil_silicon_init(void);
/* Set global and per-device MPIO configurations */
void configure_mpio(void);

#endif /* OPENSIL_DRIVER_H */
