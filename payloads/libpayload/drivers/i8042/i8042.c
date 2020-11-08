/*
 *
 * Patrick Rudolph 2017 <siro@das-labor.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload-config.h>
#include <libpayload.h>
#include <stdbool.h>
#include <stddef.h>

#include "i8042.h"

/* Overflowing FIFO implementation */

struct fifo {
	u8 *buf;
	size_t tx;
	size_t rx;
	size_t len;
};

/** Initialize a new fifo queue.
 * Initialize a new fifo with length @len.
 * @len: Length of new fifo
 * Returns NULL on error.
 */
static struct fifo *fifo_init(size_t len)
{
	struct fifo *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	memset(ret, 0, sizeof(*ret));

	ret->buf = malloc(len);
	if (!ret->buf) {
		free(ret);
		return NULL;
	}

	ret->len = len;

	return ret;
}

/** Push object onto fifo queue.
 * Pushes a new object onto the fifo. In case the fifo
 * is full the oldest object is overwritten.
 * @fifo: Fifo to use
 * @c: Element to push
 */
static void fifo_push(struct fifo *fifo, u8 c)
{
	fifo->buf[fifo->tx++] = c;
	fifo->tx = fifo->tx % fifo->len;
	if (fifo->tx == fifo->rx)
		fifo->rx++;
	fifo->rx = fifo->rx % fifo->len;
}

/** Test fifo queue element count.
 * Returns 1 if fifo is empty.
 * @fifo: Fifo to use
 */
static int fifo_is_empty(struct fifo *fifo)
{
	if (!fifo)
		return 1;
	return fifo->tx == fifo->rx;
}

/** Pop element from fifo queue.
 * Returns the oldest object from queue if any.
 * In case the queue is empty 0 is returned.
 * @fifo: Fifo to use
 */
static u8 fifo_pop(struct fifo *fifo)
{
	u8 ret;

	if (fifo_is_empty(fifo))
		return 0;

	ret = fifo->buf[fifo->rx++];
	fifo->rx = fifo->rx % fifo->len;

	return ret;
}

/** Peek on the head of fifo queue.
 * Returns the oldest object on the queue if any.
 * In case the queue is empty 0 is returned.
 * @fifo: Fifo to use
 */
static u8 fifo_peek(struct fifo *fifo)
{
	if (fifo_is_empty(fifo))
		return 0;

	return fifo->buf[fifo->rx];
}

/** Destroys a fifo queue.
 * @fifo: Fifo to use
 */
static void fifo_destroy(struct fifo *fifo)
{
	if (fifo && fifo->buf)
		free(fifo->buf);
	if (fifo)
		free(fifo);
}

/* i8042 keyboard controller implementation */

static inline u8 read_status(void) { return inb(0x64); }
static inline u8 read_data(void) { return inb(0x60); }
static inline void write_cmd(u8 cmd) { outb(cmd, 0x64); }
static inline void write_data(u8 data) { outb(data, 0x60); }

#define OBF 1
#define IBF 2

/* Keyboard controller methods */
static int initialized = 0;
static int kbc_init = 0;
static struct fifo *aux_fifo = NULL;
static struct fifo *ps2_fifo = NULL;

static int i8042_cmd_with_response(u8 cmd);

/** Wait for command ready.
 * Wait for the keyboard controller to accept a new command.
 * Returns: 0 on timeout
 */
static u8 i8042_wait_cmd_rdy(void)
{
	int retries = 10000;
	while (retries-- && (read_status() & IBF))
		udelay(50);

	return retries > 0;
}

/** Wait for data ready.
 * Wait for the keyboard controller to accept new data.
 * Returns: 0 on timeout
 */
static u8 i8042_wait_data_rdy(void)
{
	int retries = 30000;
	while (retries-- && !(read_status() & OBF))
		udelay(50);

	return retries > 0;
}

/** Keyboard controller has a ps2 port.
 * Returns if ps2 port is available.
 */
size_t i8042_has_ps2(void)
{
	return !!ps2_fifo;
}

/** Keyboard controller has an aux port.
 * Returns if aux port is available.
 */
size_t i8042_has_aux(void)
{
	return !!aux_fifo;
}

/**
 * Probe for keyboard controller
 * Returns: 1 for success, 0 for failure
 */
u8 i8042_probe(void)
{
	if (initialized)
		return 1;

	aux_fifo = NULL;
	ps2_fifo = NULL;

	/* If 0x64 returns 0xff, then we have no keyboard
	 * controller */
	if (read_status() == 0xFF) {
		printf("ERROR: No keyboard controller found!\n");
		return 0;
	}

	if (!i8042_wait_cmd_rdy()) {
		printf("ERROR: i8042_wait_cmd_rdy failed!\n");
		return 0;
	}

	kbc_init = 1;

	/* Disable first device */
	if (i8042_cmd(I8042_CMD_DIS_KB) != 0) {
		kbc_init = 0;
		printf("ERROR: i8042_cmd I8042_CMD_DIS_KB failed!\n");
		return 0;
	}

	/* Disable second device */
	if (i8042_cmd(I8042_CMD_DIS_AUX) != 0) {
		kbc_init = 0;
		printf("ERROR: i8042_cmd I8042_CMD_DIS_AUX failed!\n");
		return 0;
	}

	/* Flush buffer */
	while (read_status() & OBF)
		read_data();

	/* Self test. */
	if (i8042_cmd_with_response(I8042_CMD_SELF_TEST)
	    != I8042_SELF_TEST_RSP) {
		kbc_init = 0;
		printf("ERROR: i8042_cmd I8042_CMD_SELF_TEST failed!\n");
		return 0;
	}

	/* Test secondary port */
	if (CONFIG(LP_PC_MOUSE)) {
		if (i8042_cmd_with_response(I8042_CMD_AUX_TEST) == 0)
			aux_fifo = fifo_init(4 * 32);
	}

	/* Test first PS/2 port */
	if (i8042_cmd_with_response(I8042_CMD_KB_TEST) == 0)
		ps2_fifo = fifo_init(2 * 16);

	kbc_init = 0;

	initialized = aux_fifo || ps2_fifo;

	return initialized;
}

/* Close the keyboard controller */
void i8042_close(void)
{
	if (!initialized)
		return;

	fifo_destroy(aux_fifo);
	fifo_destroy(ps2_fifo);

	initialized = 0;
	aux_fifo = NULL;
	ps2_fifo = NULL;
}

/** Send command to keyboard controller.
 * @param cmd: The command to be send.
 * returns: 0 on success, -1 on failure.
 */
int i8042_cmd(u8 cmd)
{
	if (!initialized && !kbc_init)
		return -1;

	if (!i8042_wait_cmd_rdy())
		return -1;

	write_cmd(cmd);

	if (!i8042_wait_cmd_rdy())
		return -1;

	return 0;
}

/** Send command to keyboard controller.
 * @param cmd: The command to be send.
 * returns: Response on success, -1 on failure.
 */
static int i8042_cmd_with_response(u8 cmd)
{
	const int ret = i8042_cmd(cmd);
	if (ret != 0)
		return ret;

	if (!i8042_wait_data_rdy())
		return -1;

	return read_data();
}

/** Send additional data to keyboard controller.
 * @param data The data to be send.
 */
void i8042_write_data(u8 data)
{
	if (!initialized)
		return;

	if (!i8042_wait_cmd_rdy())
		return;

	write_data(data);

	if (!i8042_wait_cmd_rdy())
		return;
}

/**
 * Send command & data to keyboard controller.
 *
 * @param cmd: The command to be sent.
 * @param data: The data to be sent.
 * Returns 0 on success, -1 on failure.
 */
static int i8042_cmd_with_data(const u8 cmd, const u8 data)
{
	const int ret = i8042_cmd(cmd);
	if (ret != 0)
		return ret;

	i8042_write_data(data);

	return ret;
}

/**
 * Probe for keyboard controller data and queue it.
 */
static void i8042_data_poll(void)
{
	u8 c;

	if (!initialized)
		return;

	c = read_status();
	while ((c != 0xFF) && (c & OBF)) {
		const u8 in = read_data();
		/* Assume "second PS/2 port output buffer full" flag works */
		struct fifo *const fifo = (c & 0x20) ? aux_fifo : ps2_fifo;
		if (fifo)
			fifo_push(fifo, in);

		c = read_status();
	}
}

/** Keyboard controller data ready status.
 * Signals that keyboard data is ready for reading.
 */
u8 i8042_data_ready_ps2(void)
{
	if (!initialized)
		return 0;
	i8042_data_poll();
	return !fifo_is_empty(ps2_fifo);
}

/** Keyboard controller data ready status.
 * Signals that mouse data is ready for reading.
 */
u8 i8042_data_ready_aux(void)
{
	if (!initialized)
		return 0;
	i8042_data_poll();
	return !fifo_is_empty(aux_fifo);
}

/**
 * Returns available keyboard data, if any.
 */
u8 i8042_read_data_ps2(void)
{
	i8042_data_poll();
	return fifo_pop(ps2_fifo);
}

/**
 * Returns available keyboard data without advancing the queue.
 */
u8 i8042_peek_data_ps2(void)
{
	return fifo_peek(ps2_fifo);
}

/**
 * Returns available mouse data, if any.
 */
u8 i8042_read_data_aux(void)
{
	i8042_data_poll();
	return fifo_pop(aux_fifo);
}

/**
 * Waits for keyboard data.
 * Waits for up to 500msec to receive data.
 * Returns: -1 on timeout, data received otherwise
 */
int i8042_wait_read_ps2(void)
{
	int retries = 10000;

	while (retries-- && !i8042_data_ready_ps2())
		udelay(50);

	return (retries <= 0) ? -1 : i8042_read_data_ps2();
}

/** Waits for mouse data.
 * Waits for up to 500msec to receive data.
 * Returns: -1 on timeout, data received otherwise
 */
int i8042_wait_read_aux(void)
{
	int retries = 10000;

	while (retries-- && !i8042_data_ready_aux())
		udelay(50);

	return (retries <= 0) ? -1 : i8042_read_data_aux();
}

/**
 * Get the keyboard scancode translation state.
 *
 * Returns: -1 on timeout, 1 if the controller translates
 *          scancode set #2 to #1, and 0 if not.
 */
int i8042_get_kbd_translation(void)
{
	const int cfg = i8042_cmd_with_response(I8042_CMD_RD_CMD_BYTE);
	if (cfg < 0)
		return cfg;

	return !!(cfg & I8042_CMD_BYTE_XLATE);
}

/**
 * Sets the keyboard scancode translation state.
 *
 * Returns: -1 on timeout, 0 otherwise.
 */
int i8042_set_kbd_translation(const bool xlate)
{
	int cfg = i8042_cmd_with_response(I8042_CMD_RD_CMD_BYTE);
	if (cfg < 0)
		return cfg;

	if (xlate)
		cfg |= I8042_CMD_BYTE_XLATE;
	else
		cfg &= ~I8042_CMD_BYTE_XLATE;
	return i8042_cmd_with_data(I8042_CMD_WR_CMD_BYTE, cfg);
}
