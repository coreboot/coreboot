/*
 * by
 * Steve M. Gehlbach
 *
 *      Read/write the special configuration registers on the STPC
 */

#include <cpu/stpc/consumer2/stpc.h>
#include <types.h>
#include <cpu/p5/io.h>

u8 stpc_conf_readb( u8 port) {
	outb(port, STPC_CONF_REG_INDEX);
	return inb(STPC_CONF_REG_DATA);
}

void stpc_conf_writeb( u8 data, u8 port) {
	outb(port, STPC_CONF_REG_INDEX);
	outb(data, STPC_CONF_REG_DATA);
}
