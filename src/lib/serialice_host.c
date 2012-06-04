/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2009 coresystems GmbH
 * Copyright (c) 2012 Rudolf Marek - imported it from SerialICE/QEMU
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

/* Indented with:
 * indent -npro -kr -i4 -nut -bap -sob -l80 -ss -ncs serialice*
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
#include <serialice_host.h>
#include <ulinux.h>

const char *serialice_device = CONFIG_SERIALICE_HOST_DEV;

/* Check that a character is in the valid range for the
   given base
*/

//#warning "move this to string.h? got it from libpayload"

#define UINT32_MAX 0xffffffffU
static int _valid(char ch, int base)
{
    char end = (base > 9) ? '9' : '0' + (base - 1);

    /* all bases will be some subset of the 0-9 range */

    if (ch >= '0' && ch <= end)
        return 1;

    /* Bases > 11 will also have to match in the a-z range */

    if (base > 11) {
        if (tolower(ch) >= 'a' && tolower(ch) <= 'a' + (base - 11))
            return 1;
    }

    return 0;
}

/* Return the "value" of the character in the given base */

static int _offset(char ch, int base)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else
        return 10 + tolower(ch) - 'a';
}

/**
 * Convert the initial portion of a string into an unsigned int
 * @param ptr A pointer to the string to convert
 * @param endptr A pointer to the unconverted part of the string
 * @param base The base of the number to convert, or 0 for auto
 * @return An unsigned integer representation of the string
 */

static unsigned long long strtoull(const char *ptr, char **endptr,
                                   unsigned int base)
{
    unsigned long long ret = 0;

    if (endptr != NULL)
        *endptr = (char *)ptr;

    /* Purge whitespace */

    for (; *ptr && isspace(*ptr); ptr++) ;

    if (!*ptr)
        return 0;

    /* Determine the base */

    if (base == 0) {
        if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
            base = 16;
        else if (ptr[0] == '0') {
            base = 8;
            ptr++;
        } else
            base = 10;
    }

    /* Base 16 allows the 0x on front - so skip over it */

    if (base == 16) {
        if (ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
            ptr += 2;
    }

    /* If the first character isn't valid, then don't
     * bother */

    if (!*ptr || !_valid(*ptr, base))
        return 0;

    for (; *ptr && _valid(*ptr, base); ptr++)
        ret = (ret * base) + _offset(*ptr, base);

    if (endptr != NULL)
        *endptr = (char *)ptr;

    return ret;
}

static unsigned long strtoul(const char *ptr, char **endptr, unsigned int base)
{
    unsigned long long val = strtoull(ptr, endptr, base);
    if (val > UINT32_MAX)
        return UINT32_MAX;
    return val;
}

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

/* Make it static, so it works from start */
static char bb[BUFFER_SIZE];
static char cc[BUFFER_SIZE];

static SerialICEState ss = {
    .buffer = bb,
    .command = cc,
};

static SerialICEState *s = &ss;
int serialice_active = 0;

#ifndef WIN32
static struct termios options;
#endif

/* SerialICE output loggers */

#define LOG_IO		0
#define LOG_MEMORY	1
#define LOG_READ	0
#define LOG_WRITE	2
// these two are separate
#define LOG_QEMU	4
#define LOG_TARGET	8

static void serialice_log(int flags, uint32_t data, uint32_t addr, int size)
{
    if ((flags & LOG_WRITE) && (flags & LOG_MEMORY)) {
        printk(BIOS_INFO, "MEM WRITE: [%08x] %08x size %d\n", addr, data, size);
    } else if (!(flags & LOG_WRITE) && (flags & LOG_MEMORY)) {
        printk(BIOS_INFO, "MEM READ: [%08x] %08x size %d\n", addr, data, size);
    } else if ((flags & LOG_WRITE) && !(flags & LOG_MEMORY)) {
        printk(BIOS_INFO, "IO WRITE: [%08x] %08x size %d\n", addr, data, size);
    } else {                    // if (!(flags & LOG_WRITE) && !(flags & LOG_MEMORY))
        printk(BIOS_INFO, "IO READ: [%08x] %08x size %d\n", addr, data, size);
    }

}

static void serialice_msr_log(int flags, uint32_t addr, uint32_t hi,
                              uint32_t lo, int filtered)
{

    if (flags & LOG_WRITE) {
        printk(BIOS_INFO, "MSR READ: [%08x] %08x%08x\n", addr, hi, lo);
    } else {                    // if (!(flags & LOG_WRITE))
        printk(BIOS_INFO, "MSR WRITE: [%08x] %08x%08x\n", addr, hi, lo);
    }

}

static void serialice_cpuid_log(uint32_t eax, uint32_t ecx, cpuid_regs_t res,
                                int filtered)
{

    printk(BIOS_INFO, "CPUID todo\n");
}

// **************************************************************************
// low level communication with the SerialICE shell (serial communication)

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

static int handshake_mode = 0;

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
        perror("SerialICE: Could not read from target");
        ulinux_exit(1);
    }

    while (buf[0] != '\n' || buf[1] != '>' || buf[2] != ' ') {
        buf[0] = buf[1];
        buf[1] = buf[2];
        l = serialice_read(s, buf + 2, 1);
        if (l == -1) {
            perror("SerialICE: Could not read from target");
            ulinux_exit(1);
        }
    }

    return 0;
}

static void serialice_command(const char *command, int reply_len)
{
#if SERIALICE_DEBUG > 5
    int i;
#endif
    int l;

    if (!serialice_active) {
        printk(BIOS_ERR, "SerialICE not ready yet (ignoring)\n");
        return;
    }

    serialice_wait_prompt();

    serialice_write(s, command, strlen(command));

    memset(s->buffer, 0, reply_len + 1);        // clear enough of the buffer

    l = serialice_read(s, s->buffer, reply_len);

    if (l == -1) {
        perror("SerialICE: Could not read from target");
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

static void serialice_get_version(void)
{
    int len = 0;
    printk(BIOS_INFO, "SerialICE: Version.....: ");
    serialice_command("*vi", 0);

    memset(s->buffer, 0, BUFFER_SIZE);
    serialice_read(s, s->buffer, 1);
    serialice_read(s, s->buffer, 1);
    while (s->buffer[len++] != '\n') {
        serialice_read(s, s->buffer + len, 1);
    }
    s->buffer[len - 1] = '\0';

    printk(BIOS_INFO, "%s\n", s->buffer);
}

static void serialice_get_mainboard(void)
{
}

uint8_t serialice_inb(uint16_t port)
{
    uint8_t ret;

    sprintf(s->command, "*ri%04x.b", port);
    // command read back: "\n00" (3 characters)
    serialice_command(s->command, 3);
    ret = (uint8_t) strtoul(s->buffer + 1, (char **)NULL, 16);

    serialice_log(LOG_READ | LOG_IO, ret, port, 1);

    return ret;
}

uint16_t serialice_inw(uint16_t port)
{
    uint16_t ret;

    sprintf(s->command, "*ri%04x.w", port);
    // command read back: "\n0000" (5 characters)
    serialice_command(s->command, 5);
    ret = (uint16_t) strtoul(s->buffer + 1, (char **)NULL, 16);

    serialice_log(LOG_READ | LOG_IO, ret, port, 2);

    return ret;
}

uint32_t serialice_inl(uint16_t port)
{
    uint32_t ret;

    sprintf(s->command, "*ri%04x.l", port);
    // command read back: "\n00000000" (9 characters)
    serialice_command(s->command, 9);
    ret = (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);

    serialice_log(LOG_READ | LOG_IO, ret, port, 4);

    return ret;
}

void serialice_outb(uint8_t data, uint16_t port)
{
    serialice_log(LOG_WRITE | LOG_IO, data, port, 1);

    sprintf(s->command, "*wi%04x.b=%02x", port, data);
    serialice_command(s->command, 0);
}

void serialice_outw(uint16_t data, uint16_t port)
{
    serialice_log(LOG_WRITE | LOG_IO, data, port, 2);

    sprintf(s->command, "*wi%04x.w=%04x", port, data);
    serialice_command(s->command, 0);
}

void serialice_outl(uint32_t data, uint16_t port)
{
    serialice_log(LOG_WRITE | LOG_IO, data, port, 4);

    sprintf(s->command, "*wi%04x.l=%08x", port, data);
    serialice_command(s->command, 0);
}

uint8_t serialice_readb(uint32_t addr)
{
    uint8_t ret;
    sprintf(s->command, "*rm%08x.b", addr);
    // command read back: "\n00" (3 characters)
    serialice_command(s->command, 3);
    ret = (uint8_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    serialice_log(LOG_READ | LOG_MEMORY, ret, addr, 1);

    return ret;
}

uint16_t serialice_readw(uint32_t addr)
{
    uint16_t ret;
    sprintf(s->command, "*rm%08x.w", addr);
    // command read back: "\n0000" (5 characters)
    serialice_command(s->command, 5);
    ret = (uint16_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    serialice_log(LOG_READ | LOG_MEMORY, ret, addr, 2);

    return ret;
}

uint32_t serialice_readl(uint32_t addr)
{
    uint32_t ret;
    sprintf(s->command, "*rm%08x.l", addr);
    // command read back: "\n00000000" (9 characters)
    serialice_command(s->command, 9);
    ret = (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    serialice_log(LOG_READ | LOG_MEMORY, ret, addr, 4);

    return ret;
}

void serialice_writeb(uint8_t data, uint32_t addr)
{
    serialice_log(LOG_WRITE | LOG_MEMORY, data, addr, 1);
    sprintf(s->command, "*wm%08x.b=%02x", addr, data);
    serialice_command(s->command, 0);
}

void serialice_writew(uint16_t data, uint32_t addr)
{
    serialice_log(LOG_WRITE | LOG_MEMORY, data, addr, 2);
    sprintf(s->command, "*wm%08x.w=%04x", addr, data);
    serialice_command(s->command, 0);
}

void serialice_writel(uint32_t data, uint32_t addr)
{
    serialice_log(LOG_WRITE | LOG_MEMORY, data, addr, 4);
    sprintf(s->command, "*wm%08x.l=%08x", addr, data);
    serialice_command(s->command, 0);
}

uint64_t serialice_rdmsr(uint32_t addr, uint32_t key)
{
    uint32_t hi, lo;
    uint64_t ret;
    int filtered = 0;

    sprintf(s->command, "*rc%08x.%08x", addr, key);

    // command read back: "\n00000000.00000000" (18 characters)
    serialice_command(s->command, 18);

    s->buffer[9] = 0;           // . -> \0
    hi = (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    lo = (uint32_t) strtoul(s->buffer + 10, (char **)NULL, 16);

    ret = hi;
    ret <<= 32;
    ret |= lo;

    serialice_msr_log(LOG_READ, addr, hi, lo, filtered);

    return ret;
}

void serialice_wrmsr(uint64_t data, uint32_t addr, uint32_t key)
{
    uint32_t hi, lo;
    int filtered = 0;

    hi = (data >> 32);
    lo = (data & 0xffffffff);

    if (!filtered) {
        sprintf(s->command, "*wc%08x.%08x=%08x.%08x", addr, key, hi, lo);
        serialice_command(s->command, 0);
    }

    serialice_msr_log(LOG_WRITE, addr, hi, lo, filtered);
}

cpuid_regs_t serialice_cpuid(uint32_t eax, uint32_t ecx)
{
    cpuid_regs_t ret;

    ret.eax = eax;
    ret.ebx = 0;                // either set by filter or by target
    ret.ecx = ecx;
    ret.edx = 0;                // either set by filter or by target

    sprintf(s->command, "*ci%08x.%08x", eax, ecx);

    // command read back: "\n000006f2.00000000.00001234.12340324"
    // (36 characters)
    serialice_command(s->command, 36);

    s->buffer[9] = 0;           // . -> \0
    s->buffer[18] = 0;          // . -> \0
    s->buffer[27] = 0;          // . -> \0
    ret.eax = (uint32_t) strtoul(s->buffer + 1, (char **)NULL, 16);
    ret.ebx = (uint32_t) strtoul(s->buffer + 10, (char **)NULL, 16);
    ret.ecx = (uint32_t) strtoul(s->buffer + 19, (char **)NULL, 16);
    ret.edx = (uint32_t) strtoul(s->buffer + 28, (char **)NULL, 16);

    serialice_cpuid_log(eax, ecx, ret, 0);

    return ret;
}

// **************************************************************************
// initialization and exit

void serialice_init(void)
{
    printk(BIOS_INFO, "SerialICE: Open connection to target hardware...\n");

    if (serialice_device == NULL) {
        printk(BIOS_INFO,
               "You need to specify a serial device to use SerialICE.\n");
        ulinux_exit(1);
    }
#ifdef WIN32
    s->fd = CreateFile(serialice_device, GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, 0, NULL);

    if (s->fd == INVALID_HANDLE_VALUE) {
        perror("SerialICE: Could not connect to target TTY");
        ulinux_exit(1);
    }

    DCB dcb;
    if (!GetCommState(s->fd, &dcb)) {
        perror("SerialICE: Could not load config for target TTY");
        ulinux_exit(1);
    }

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(s->fd, &dcb)) {
        perror("SerialICE: Could not store config for target TTY");
        ulinux_exit(1);
    }
#else
    s->fd = ulinux_open(serialice_device, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (s->fd == -1) {
        perror("SerialICE: Could not connect to target TTY");
        ulinux_exit(1);
    }

    if (ulinux_ioctl(s->fd, TIOCEXCL) == -1) {
        perror("SerialICE: TTY not exclusively available");
        ulinux_exit(1);
    }

    if (ulinux_fcntl(s->fd, F_SETFL, 0) == -1) {
        perror("SerialICE: Could not switch to blocking I/O");
        ulinux_exit(1);
    }

    if (ulinux_tcgetattr(s->fd, &options) == -1) {
        perror("SerialICE: Could not get TTY attributes");
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

    s->buffer = bb;
    s->command = cc;

    printk(BIOS_INFO, "SerialICE: Waiting for handshake with target... ");

    handshake_mode = 1;         // Readback errors are to be expected in this phase.

    /* Trigger a prompt */
    serialice_write(s, "\n\n\n\n", 4);
    serialice_write(s, "\n\n\n\n", 4);

    /* ... and wait for it to appear */
    if (serialice_wait_prompt() == 0) {
        printk(BIOS_INFO, "target alive!\n");
        serialice_active = 1;
    } else {
        printk(BIOS_INFO, "target not ok!\n");
        ulinux_exit(1);
    }

    /* Each serialice_command() waits for a prompt, so trigger one for the
     * first command, as we consumed the last one for the handshake
     */
    serialice_write(s, "@", 1);

    handshake_mode = 0;         // from now on, warn about readback errors.

    serialice_get_version();

    serialice_get_mainboard();

    /* Let the rest of Qemu know we're alive */
}
