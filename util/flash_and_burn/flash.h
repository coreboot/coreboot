#include <sys/io.h>
#include <unistd.h>

struct flashchip {
	char * name;
	int manufacture_id;
	int model_id;

	volatile char * virt_addr;
	int total_size;
	int page_size;

	int (*probe) (struct flashchip * flash);
	int (*erase) (struct flashchip * flash);
	int (*write) (struct flashchip * flash, char * buf);
};

#define AMD_ID            0x01
#define AM_29F040B        0xA4

#define ATMEL_ID          0x1F    /* Winbond Manufacture ID code	*/
#define AT_29C040A        0xA4    /* Winbond w29c020c device code*/

#define MX_ID             0xC2
#define MX_29F002         0xB0

#define SST_ID            0xBF	/* SST Manufacturer ID[B code	*/
#define SST_29EE020A      0x10	/* SST 29EE020 device code	*/
#define SST_28SF040       0x04    /* SST 29EE040 device code	*/

#define WINBOND_ID        0xDA	/* Winbond Manufacture ID code	*/
#define W_29C020C         0x45	/* Winbond w29c020c device code*/

extern int probe_28sf040 (struct flashchip * flash);
extern int erase_28sf040 (struct flashchip * flash);
extern int write_28sf040 (struct flashchip * flash, char * buf);

extern int probe_29f040b (struct flashchip * flash);
extern int erase_29f040b (struct flashchip * flash);
extern int write_29f040b (struct flashchip * flash, char * buf);

extern int probe_29f002 (struct flashchip * flash);
extern int erase_29f002 (struct flashchip * flash);
extern int write_29f002 (struct flashchip * flash, char * buf);
