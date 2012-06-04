#include <stdint.h>
#include <termios.h>
extern  int _call_linux(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);

void __stack_chk_init(void);
void *ulinux_mmap(unsigned long addr, unsigned int length);
int ulinux_write(int fd, const void *buf, unsigned int count);
int ulinux_close(int fd);
int ulinux_read(int fd, void *buf, unsigned int count);
int ulinux_open(const char *pathname, int flags);

#define EINTR 4
extern int ulinux_errno;

#define O_RDWR		00000002
#define O_NOCTTY	   0400	/* not fcntl */
#define O_NONBLOCK	  04000
#define O_NDELAY	O_NONBLOCK
#define F_SETFL		4	/* Set file status flags.  */
#define EOF -1

#define perror(x) printk(BIOS_ERR, x)

int ulinux_ioctl(int d, int request);

int ulinux_tcgetattr(int fd, struct termios *termios_p);
int ulinux_cfsetispeed(struct termios *termios_p, speed_t speed);
int ulinux_cfsetospeed(struct termios *termios_p, speed_t speed);
int ulinux_tcsetattr(int fd, int optional_actions,
                     const struct termios *termios_p);
void ulinux_exit(int status);
int ulinux_fcntl(int fd, int cmd, int a);
int ulinux_tcflush(int fd, int queue_selector);
void ulinux_init(void);

#define TIOCEXCL        0x540C
