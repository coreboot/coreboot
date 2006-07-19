#ifndef __FLASH_H__
#define __FLASH_H__ 1

#include <sys/io.h>
#include <unistd.h>
#include <stdint.h>

struct flashchip {
	char *name;
	int manufacture_id;
	int model_id;

	volatile uint8_t *virt_addr;
	int total_size;
	int page_size;

	int (*probe) (struct flashchip * flash);
	int (*erase) (struct flashchip * flash);
	int (*write) (struct flashchip * flash, uint8_t *buf);
	int (*read) (struct flashchip * flash, uint8_t *buf);

	int fd_mem;
	volatile uint8_t *virt_addr_2;
};

extern struct flashchip flashchips[];

#define AMD_ID            0x01
#define AM_29F040B        0xA4
#define AM_29F016D        0xAD

#define ATMEL_ID          0x1F	/* Winbond Manufacture ID code        */
#define AT_29C040A        0xA4	/* Winbond w29c020c device code */

#define MX_ID             0xC2
#define MX_29F002         0xB0

#define SHARP_ID            0xB0
#define SHARP_LHF00L04      0xCF

#define SST_ID            0xBF	/* SST Manufacturer ID code   */
#define SST_29EE020A      0x10	/* SST 29EE020 device code      */
#define SST_28SF040       0x04	/* SST 29EE040 device code    */
#define SST_39SF020       0xB6	/* SST 39SF020  device        */
#define SST_39VF020       0xD6	/* SST 39VF020  device        */
#define SST_49LF040B	  0x50	/* SST 49LF040B device */
#define SST_49LF040	  0x51	/* SST 49LF040 device */
#define SST_49LF080A	  0x5B	/* SST 48LF080A device */
#define SST_49LF002A	  0x57	/* SST 49LF002A device */
#define SST_49LF003A	  0x1B	/* SST 49LF003A device */
#define SST_49LF004A	  0x60	/* SST 49LF004A device */
#define SST_49LF008A	  0x5A	/* SST 49LF008A device */

#define PMC_ID            0x9D	/* PMC Manufacturer ID[B code   */
#define PMC_49FL002       0x6D	/* PMC 49FL002 device code      */
#define PMC_49FL004       0x6E	/* PMC 49FL004 device code      */

#define WINBOND_ID        0xDA	/* Winbond Manufacture ID code  */
#define W_29C011          0xC1	/* Winbond w29c011 device code */
#define W_29C020C         0x45	/* Winbond w29c020c device code */
#define W_39V040A         0x3D	/* Winbond w39v040a device code */
#define W_49F002U         0x0B	/* Winbond w49F002u device code */
#define W_49V002A         0xB0  /* Winbond W49V002A device code */
#define W_49V002FA        0x32  /* Winbond W49V002FA device code */

#define ST_ID       	  0x20
#define ST_M29F040B       0xE2
#define ST_M29F400BT      0xD5

#define EMST_ID           0x8c /* EMST - Elite Flash Storage Inc. Manufacturer ID code */
#define EMST_F49B002UA    0x00 /* EMST F49B002UA device code */

#define MSYSTEMS_ID       0x156f
#define MSYSTEMS_MD2200   0xdb	/* ? */
#define MSYSTEMS_MD2800   0x30	/* hmm -- both 0x30 */
#define MSYSTEMS_MD2802   0x30	/* hmm -- both 0x30 */

extern void myusec_delay(int time);
extern void myusec_calibrate_delay();
extern int enable_flash_write(void);
#endif				/* !__FLASH_H__ */
