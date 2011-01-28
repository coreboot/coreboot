#include <assert.h>
#include "cmos_lowlevel.h"

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <machine/cpufunc.h>
#define OUTB(x, y) do { u_int tmp = (y); outb(tmp, (x)); } while (0)
#define OUTW(x, y) do { u_int tmp = (y); outw(tmp, (x)); } while (0)
#define OUTL(x, y) do { u_int tmp = (y); outl(tmp, (x)); } while (0)
#define INB(x) __extension__ ({ u_int tmp = (x); inb(tmp); })
#define INW(x) __extension__ ({ u_int tmp = (x); inw(tmp); })
#define INL(x) __extension__ ({ u_int tmp = (x); inl(tmp); })
#else
#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
#include <DirectIO/darwinio.h>
#endif
#define OUTB outb
#define OUTW outw
#define OUTL outl
#define INB  inb
#define INW  inw
#define INL  inl
#endif

static void cmos_hal_init(void* data);
static unsigned char cmos_hal_read(unsigned addr);
static void cmos_hal_write(unsigned addr, unsigned char value);
static void cmos_set_iopl(int level);

/* no need to initialize anything */
static void cmos_hal_init(__attribute__((unused)) void *data)
{
}

static unsigned char cmos_hal_read(unsigned index)
{
	unsigned short port_0, port_1;

	assert(!verify_cmos_byte_index(index));

	if (index < 128) {
		port_0 = 0x70;
		port_1 = 0x71;
	} else {
		port_0 = 0x72;
		port_1 = 0x73;
	}

	OUTB(index, port_0);
	return INB(port_1);
}

static void cmos_hal_write(unsigned index, unsigned char value)
{
	unsigned short port_0, port_1;

	assert(!verify_cmos_byte_index(index));

	if (index < 128) {
		port_0 = 0x70;
		port_1 = 0x71;
	} else {
		port_0 = 0x72;
		port_1 = 0x73;
	}

	OUTB(index, port_0);
	OUTB(value, port_1);
}


/****************************************************************************
 * cmos_set_iopl
 *
 * Set the I/O privilege level of the executing process.  Root privileges are
 * required for performing this action.  A sufficient I/O privilege level
 * allows the process to access x86 I/O address space and to disable/reenable
 * interrupts while executing in user space.  Messing with the I/O privilege
 * level is therefore somewhat dangerous.
 ****************************************************************************/
static void cmos_set_iopl(int level)
{
#if defined(__FreeBSD__)
	static int io_fd = -1;
#endif

	assert((level >= 0) && (level <= 3));

#if defined(__FreeBSD__)
	if (level == 0) {
		if (io_fd != -1) {
			close(io_fd);
			io_fd = -1;
		}
	} else {
		if (io_fd == -1) {
			io_fd = open("/dev/io", O_RDWR);
			if (io_fd < 0) {
				perror("/dev/io");
				exit(1);
			}
		}
	}
#else
	if (iopl(level)) {
		fprintf(stderr, "%s: iopl() system call failed.  "
			"You must be root to do this.\n", prog_name);
		exit(1);
	}
#endif
}

cmos_access_t cmos_hal = {
	.init = cmos_hal_init,
	.read = cmos_hal_read,
	.write = cmos_hal_write,
	.set_iopl = cmos_set_iopl,
};

