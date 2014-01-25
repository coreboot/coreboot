#ifndef _QEMU_DEBUGCON_H_
#define _QEMU_DEBUGCON_H_

#include <console/streams.h>

void qemu_debugcon_init(void);
void qemu_debugcon_tx_byte(unsigned char data);

#if CONFIG_CONSOLE_QEMU_DEBUGCON && (ENV_ROMSTAGE || ENV_RAMSTAGE)
#define	__qemu_debugcon_init		qemu_debugcon_init()
#define	__qemu_debugcon_tx_byte(x) 	qemu_debugcon_tx_byte(x)
#else
#define	__qemu_debugcon_init()
#define	__qemu_debugcon_tx_byte(x)
#endif

#endif
