/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __HALT_H__
#define __HALT_H__

/**
 * halt the system reliably
 */
void __noreturn halt(void);

/* Power off the system. */
void poweroff(void);

#endif /* __HALT_H__ */
