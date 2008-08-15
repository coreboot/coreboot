#include <device/smbus_def.h>

#define SMBGSTATUS 0xe0
#define SMBGCTL    0xe2
#define SMBHSTADDR 0xe4
#define SMBHSTDAT  0xe6
#define SMBHSTCMD  0xe8
#define SMBHSTFIFO 0xe9

#define SMBUS_TIMEOUT (100*1000*10)
#define SMBUS_STATUS_MASK 0xfbff

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

