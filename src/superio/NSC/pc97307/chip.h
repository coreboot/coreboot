#ifndef _SUPERIO_NSC_PC97307
#define _SUPERIO_NSC_PC97307

#ifndef PNP_INDEX_REG
#define PNP_INDEX_REG   0x15C
#endif
#ifndef PNP_DATA_REG
#define PNP_DATA_REG    0x15D
#endif
#ifndef SIO_COM1
#define SIO_COM1_BASE   0x3F8
#endif
#ifndef SIO_COM2
#define SIO_COM2_BASE   0x2F8
#endif

extern struct chip_operations superio_NSC_pc97307_control;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_NSC_pc97307_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
#endif /* _SUPERIO_NSC_PC97307 */
