/*
 * Copyright (c) 2009 coresystems GmbH
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* System includes */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#endif

#include <console/console.h>
#include <serialice.h>
#include <ulinux.h>

#define SERIALICE_DEBUG 3
#define BUFFER_SIZE 1024

typedef struct {
#ifdef WIN32
    HANDLE fd;
#else
    int fd;
#endif
    char *buffer;
    char *command;
} SerialICEState;

static SerialICEState *s;
static const SerialICE_target serialice_protocol;
const char *serialice_mainboard = NULL;

const SerialICE_target *target;
const char *serialice_device = NULL;

#ifndef WIN32
static struct termios options;
#endif

// **************************************************************************
// low level communication with the SerialICE shell (serial communication)
static int handshake_mode = 0;

static void *mallocz(unsigned int size)
{
	void *mem = malloc(size);
	if (mem) {
		memset(mem, 0, size);
	}
	return mem;
}

static int serialice_read(SerialICEState * state, void *buf, size_t nbyte)
{
    int bytes_read = 0;

    while (1) {
#ifdef WIN32
        int ret = 0;
        ReadFile(state->fd, buf, nbyte - bytes_read, &ret, NULL);
        if (!ret) {
            break;
        }
#else
        int ret = ulinux_read(state->fd, buf, nbyte - bytes_read);

        if (ret == -1 && ulinux_errno == EINTR) {
            continue;
        }

        if (ret == -1) {
            break;
        }
#endif

        bytes_read += ret;
        buf += ret;

        if (bytes_read >= (int)nbyte) {
            break;
        }
    }

    return bytes_read;
}

static int serialice_write(SerialICEState * state, const void *buf,
                           size_t nbyte)
{
    char *buffer = (char *)buf;
    char c;
    int i;

    for (i = 0; i < (int)nbyte; i++) {
#ifdef WIN32
        int ret = 0;
        while (ret == 0) {
            WriteFile(state->fd, buffer + i, 1, &ret, NULL);
        }
        ret = 0;
        while (ret == 0) {
            ReadFile(state->fd, &c, 1, &ret, NULL);
        }
#else
        while (ulinux_write(state->fd, buffer + i, 1) != 1) ;
        while (ulinux_read(state->fd, &c, 1) != 1) ;
#endif
        if (c != buffer[i] && !handshake_mode) {
            printk(BIOS_ERR, "Readback error! %x/%x\n", c, buffer[i]);
        }
    }

    return nbyte;
}

static int serialice_wait_prompt(void)
{
    char buf[3];
    int l;

    l = serialice_read(s, buf, 3);

    if (l == -1) {
        printk(BIOS_ERR, "SerialICE: Could not read from target");
        ulinux_exit(1);
    }

    while (buf[0] != '\n' || buf[1] != '>' || buf[2] != ' ') {
        buf[0] = buf[1];
        buf[1] = buf[2];
        l = serialice_read(s, buf + 2, 1);
        if (l == -1) {
            printk(BIOS_ERR, "SerialICE: Could not read from target");
            ulinux_exit(1);
        }
    }

    return 0;
}

const SerialICE_target *serialice_serial_init(void)
{
    s = mallocz(sizeof(SerialICEState));

    if (serialice_device == NULL) {
        printk(BIOS_ERR, "You need to specify a serial device to use SerialICE.\n");
        ulinux_exit(1);
    }
#ifdef WIN32
    s->fd = CreateFile(serialice_device, GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, 0, NULL);

    if (s->fd == INVALID_HANDLE_VALUE) {
        printk(BIOS_ERR, "SerialICE: Could not connect to target TTY");
        ulinux_exit(1);
    }

    DCB dcb;
    if (!GetCommState(s->fd, &dcb)) {
        printk(BIOS_ERR, "SerialICE: Could not load config for target TTY");
        ulinux_exit(1);
    }

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(s->fd, &dcb)) {
        printk(BIOS_ERR, "SerialICE: Could not store config for target TTY");
        ulinux_exit(1);
    }
#else
    s->fd = ulinux_open(serialice_device, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (s->fd == -1) {
        printk(BIOS_ERR, "SerialICE: Could not connect to target TTY");
        ulinux_exit(1);
    }

    if (ulinux_ioctl(s->fd, TIOCEXCL) == -1) {
        printk(BIOS_ERR, "SerialICE: TTY not exclusively available");
        ulinux_exit(1);
    }

    if (ulinux_fcntl(s->fd, F_SETFL, 0) == -1) {
        printk(BIOS_ERR, "SerialICE: Could not switch to blocking I/O");
        ulinux_exit(1);
    }

    if (ulinux_tcgetattr(s->fd, &options) == -1) {
        printk(BIOS_ERR, "SerialICE: Could not get TTY attributes");
        ulinux_exit(1);
    }

    ulinux_cfsetispeed(&options, B115200);
    ulinux_cfsetospeed(&options, B115200);

    /* set raw input, 1 second timeout */
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_iflag |= IGNCR;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 100;

    ulinux_tcsetattr(s->fd, TCSANOW, &options);

    ulinux_tcflush(s->fd, TCIOFLUSH);
#endif

    s->buffer = mallocz(BUFFER_SIZE);
    s->command = mallocz(BUFFER_SIZE);

    printk(BIOS_ERR, "SerialICE: Waiting for handshake with target... ");

    handshake_mode = 1;         // Readback errors are to be expected in this phase.

    /* Trigger a prompt */
    serialice_write(s, "@", 1);

    /* ... and wait for it to appear */
    if (serialice_wait_prompt() == 0) {
        printk(BIOS_ERR, "target alive!\n");
    } else {
        printk(BIOS_ERR, "target not ok!\n");
        ulinux_exit(1);
    }

    /* Each serialice_command() waits for a prompt, so trigger one for the
     * first command, as we consumed the last one for the handshake
     */
    serialice_write(s, "@", 1);

    handshake_mode = 0;         // from now on, warn about readback errors.
    return &serialice_protocol;
}

void serialice_serial_exit(void)
{
#if 0
    free(s->command);
    free(s->buffer);
    free(s);
#endif
}

static void serialice_command(const char *command, int reply_len)
{
#if SERIALICE_DEBUG > 5
    int i;
#endif
    int l;

    serialice_wait_prompt();

    serialice_write(s, command, strlen(command));

    memset(s->buffer, 0, reply_len + 1);        // clear enough of the buffer

    l = serialice_read(s, s->buffer, reply_len);

    if (l == -1) {
        printk(BIOS_ERR, "SerialICE: Could not read from target");
        ulinux_exit(1);
    }
    // compensate for CR on the wire. Needed on Win32
    if (s->buffer[0] == '\r') {
        memmove(s->buffer, s->buffer + 1, reply_len);
        serialice_read(s, s->buffer + reply_len - 1, 1);
    }

    if (l != reply_len) {
        printk(BIOS_ERR, "SerialICE: command was not answered sufficiently: "
               "(%d/%d bytes)\n'%s'\n", l, reply_len, s->buffer);
        ulinux_exit(1);
    }
#if SERIALICE_DEBUG > 5
    for (i = 0; i < reply_len; i++) {
        printk(BIOS_ERR, "%02x ", s->buffer[i]);
    }
    printk(BIOS_ERR, "\n");
#endif
}

// **************************************************************************
// high level communication with the SerialICE shell

static void msg_version(void)
{
    int len = 0;
    printk(BIOS_ERR, "SerialICE: Version.....: ");
    serialice_command("*vi", 0);

    memset(s->buffer, 0, BUFFER_SIZE);
    serialice_read(s, s->buffer, 1);
    serialice_read(s, s->buffer, 1);
    while (s->buffer[len++] != '\n') {
        serialice_read(s, s->buffer + len, 1);
    }
    s->buffer[len - 1] = '\0';

    printk(BIOS_ERR, "%s\n", s->buffer);
}

static void msg_mainboard(void)
{
    int len = 31;

    printk(BIOS_ERR, "SerialICE: Mainboard...: ");
    serialice_command("*mb", 32);
    while (len && s->buffer[len] == ' ') {
        s->buffer[len--] = '\0';
    }
    serialice_mainboard = strdup(s->buffer + 1);
    printk(BIOS_ERR, "%s\n", serialice_mainboard);
}

static uint32_t msg_io_read(uint16_t port, unsigned int size)
{
    switch (size) {
    case 1:
        sprintf(s->command, "*ri%04x.b", port);
        // command read back: "\n00" (3 characters)
        serialice_command(s->command, 3);
        return (uint8_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    case 2:
        sprintf(s->command, "*ri%04x.w", port);
        // command read back: "\n0000" (5 characters)
        serialice_command(s->command, 5);
        return (uint16_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    case 4:
        sprintf(s->command, "*ri%04x.l", port);
        // command read back: "\n00000000" (9 characters)
        serialice_command(s->command, 9);
        return strtoul(s->buffer + 1, (char **)NULL, 16);
    default:
        printk(BIOS_ERR, "WARNING: unknown read access size %d @%08x\n", size, port);
        return -1;
    }
}

static void msg_io_write(uint16_t port, unsigned int size, uint32_t data)
{
    switch (size) {
    case 1:
        sprintf(s->command, "*wi%04x.b=%02x", port, (uint8_t) data);
        serialice_command(s->command, 0);
        return;
    case 2:
        sprintf(s->command, "*wi%04x.w=%04x", port, (uint16_t) data);
        serialice_command(s->command, 0);
        return;
    case 4:
        sprintf(s->command, "*wi%04x.l=%08x", port, data);
        serialice_command(s->command, 0);
        return;
    default:
        printk(BIOS_ERR, "WARNING: unknown write access size %d @%08x\n", size, port);
    }
    return;
}

static uint32_t msg_load(uint32_t addr, unsigned int size)
{
    switch (size) {
    case 1:
        sprintf(s->command, "*rm%08x.b", addr);
        // command read back: "\n00" (3 characters)
        serialice_command(s->command, 3);
        return (uint8_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    case 2:
        sprintf(s->command, "*rm%08x.w", addr);
        // command read back: "\n0000" (5 characters)
        serialice_command(s->command, 5);
        return (uint16_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    case 4:
        sprintf(s->command, "*rm%08x.l", addr);
        // command read back: "\n00000000" (9 characters)
        serialice_command(s->command, 9);
        return (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    default:
        printk(BIOS_ERR, "WARNING: unknown read access size %d @%08x\n", size, addr);
    }
    return 0;
}

static void msg_store(uint32_t addr, unsigned int size, uint32_t data)
{
    switch (size) {
    case 1:
        sprintf(s->command, "*wm%08x.b=%02x", addr, (uint8_t) data);
        serialice_command(s->command, 0);
        break;
    case 2:
        sprintf(s->command, "*wm%08x.w=%04x", addr, (uint16_t) data);
        serialice_command(s->command, 0);
        break;
    case 4:
        sprintf(s->command, "*wm%08x.l=%08x", addr, data);
        serialice_command(s->command, 0);
        break;
    default:
        printk(BIOS_ERR, "WARNING: unknown write access size %d @%08x\n", size, addr);
    }
}

static void msg_rdmsr(uint32_t addr, uint32_t key, uint32_t * hi, uint32_t * lo)
{
    sprintf(s->command, "*rc%08x.%08x", addr, key);
    // command read back: "\n00000000.00000000" (18 characters)
    serialice_command(s->command, 18);
    s->buffer[9] = 0;           // . -> \0
    *hi = (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    *lo = (uint32_t) strtoul(s->buffer + 10, (char **)NULL, 16);
}

static void msg_wrmsr(uint32_t addr, uint32_t key, uint32_t hi, uint32_t lo)
{
    sprintf(s->command, "*wc%08x.%08x=%08x.%08x", addr, key, hi, lo);
    serialice_command(s->command, 0);
}

static void msg_cpuid(uint32_t eax, uint32_t ecx, cpuid_regs_t * ret)
{
    sprintf(s->command, "*ci%08x.%08x", eax, ecx);
    // command read back: "\n000006f2.00000000.00001234.12340324"
    // (36 characters)
    serialice_command(s->command, 36);
    s->buffer[9] = 0;           // . -> \0
    s->buffer[18] = 0;          // . -> \0
    s->buffer[27] = 0;          // . -> \0
    ret->eax = (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    ret->ebx = (uint32_t) strtoul(s->buffer + 10, (char **)NULL, 16);
    ret->ecx = (uint32_t) strtoul(s->buffer + 19, (char **)NULL, 16);
    ret->edx = (uint32_t) strtoul(s->buffer + 28, (char **)NULL, 16);
}

static const SerialICE_target serialice_protocol = {
    .version = msg_version,
    .mainboard = msg_mainboard,
    .io_read = msg_io_read,
    .io_write = msg_io_write,
    .load = msg_load,
    .store = msg_store,
    .rdmsr = msg_rdmsr,
    .wrmsr = msg_wrmsr,
    .cpuid = msg_cpuid,
};

#define mask_data(val,bytes) (val & (((uint64_t)1<<(bytes*8))-1))

uint32_t serialice_io_read(uint16_t port, unsigned int size)
{
    uint32_t data = 0;

    data = target->io_read(port, size);

    data = mask_data(data, size);
    return data;
}

void serialice_io_write(uint16_t port, unsigned int size, uint32_t data)
{
    uint32_t filtered_data = mask_data(data, size);


    data = mask_data(filtered_data, size);
    target->io_write(port, size, data);

}

