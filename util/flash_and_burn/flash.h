#ifndef __FLASH_H__
#define __FLASH_H__ 1

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
	int (*write) (struct flashchip * flash, unsigned char * buf);
	int (*read)  (struct flashchip * flash, unsigned char * buf);

	int fd_mem;
	volatile char *virt_addr_2;
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
#define SST_39SF020       0xB6    /* SST 39SF020  device        */
#define SST_39VF020       0xD6    /* SST 39VF020  device        */
#define SST_49LF040	  0x51	/* SST 49LF040 device */

#define PMC_ID            0x9D  /* PMC Manufacturer ID[B code   */
#define PMC_49FL004       0x6E  /* PMC 49FL004 device code      */

#define WINBOND_ID        0xDA	/* Winbond Manufacture ID code	*/
#define W_29C011          0xC1	/* Winbond w29c011 device code*/
#define W_29C020C         0x45	/* Winbond w29c020c device code*/
#define W_49F002U         0x0B	/* Winbond w29c020c device code*/

#define ST_ID       	  0x20	
#define ST_M29F400BT      0xD5

#define MSYSTEMS_ID       0x156f
#define MSYSTEMS_MD2200   0xdb /* ? */
#define MSYSTEMS_MD2800   0x30 /* hmm -- both 0x30 */
#define MSYSTEMS_MD2802   0x30 /* hmm -- both 0x30 */

extern void myusec_delay(int time);
extern void myusec_calibrate_delay();
extern int enable_flash_write(void);
#endif /* !__FLASH_H__ */
