#ifndef UHCI_H
#define UHCI_H

/*
 * The link pointer is multi use. Some fields are valid only for some uses. 
 * In other cases, they must be 0
 *
 */

typedef unsigned char uchar;

#ifndef KERNEL
typedef unsigned short ushort;
#endif

#define MAX_POLLDEV 10

#define MAX_TRANSACTIONS 10
#define MAX_QUEUEHEAD 255
#define MAX_TD 1024

#ifndef __KERNEL__
#define virt_to_bus
#define bus_to_virt
#endif

#ifndef __KERNEL__
#define LINK_ADDR(x) ((unsigned int)x >> 4)
#define MEM_ADDR(x) (void *) ( ((unsigned int) (x))<<4)
#else
#define LINK_ADDR(x) ( virt_to_bus(x) >> 4)
#define MEM_ADDR(x) (void *) (  bus_to_virt( ((unsigned int) (x)) <<4) )
#endif

typedef struct link_pointer {
	unsigned long terminate:1;
	unsigned long queue:1;
	unsigned long depth:1;
	unsigned long reserved:1;
	unsigned long link:28;
} __attribute__ ((packed)) link_pointer_t;

#define SETUP_TOKEN 0x2d
#define IN_TOKEN 0x69
#define OUT_TOKEN 0xe1

#define CTRL_RETRIES 3
#define CONTROL_STS_RETRIES 0

// Some control message bmRequestType defines
#define CTRL_DEVICE 0
#define CONTROL_INTERFACE 1
#define CONTROL_ENDPOINT 2
#define CONTROL_OTHER 3
#define CONTROL_RECIPIENT_MASK 0x1f

#define CONTROL_TYPE_STD 0
#define CONTROL_TYPE_CLASS 0x20
#define CONTROL_CLASS_VENDOR 0x40
#define CONTROL_CLASS_MASK 0x60

#define CONTROL_OUT 0
#define CONTROL_IN 0x80
#define CONTROL_DIR_MASK 0x80

// bRequest values
#define GET_STATUS 0
#define CLEAR_FEATURE 1
#define SET_FEATURE 3
#define SET_ADDRESS 5

#define GET_DESCRIPTOR 6
#define SET_DESCRIPTOR 7

#define GET_CONFIGURATION 8
#define SET_CONFIGURATION 9

#define GET_INTERFACE 10
#define SET_INTERFACE 11

#define SYNC_FRAME 12

// some port features
#define PORT_CONNECTION 0
#define PORT_ENABLE 1
#define PORT_SUSPEND 2
#define PORT_OVER_CURRENT 3
#define PORT_RESET 4
#define PORT_POWER 8
#define PORT_LOW_SPEED 9
#define C_PORT_CONNECTION 16
#define C_PORT_ENABLE 17
#define C_PORT_SUSPEND 18
#define C_PORT_OVER_CURRENT 19
#define C_PORT_RESET 20

// descriptor types
#define DEVICE_DESC 1
#define CONFIGURATION_DESC 2
#define STRING_DESC 3
#define INTERFACE_DESC 4
#define ENDPOINT_DESC 5
#define OTHERSPEED_DESC 7
#define POWER_DESC 8

// features
#define FEATURE_HALT 0

typedef struct td {
	
	link_pointer_t link;

	unsigned long actual:11;	// actual length
	unsigned long reserved2:5;

// status/error flags
	unsigned long res1:1;
	unsigned long bitstuff:1;
	unsigned long crc:1;
	unsigned long nak:1;
	unsigned long babble:1;
	unsigned long buffer_error:1;
	unsigned long stall:1;
	unsigned long active:1;

	unsigned long interrupt:1;	// interrupt on complete
	unsigned long isochronous:1;
	unsigned long lowspeed:1;
	unsigned long retrys:2;
	unsigned long detect_short:1;
	unsigned long reserved3:2;

	unsigned long packet_type:8;	// one of in (0x69), out (0xe1) or setup (0x2d)
	unsigned long device_addr:7;
	unsigned long endpoint:4;
	unsigned long data_toggle:1;
	unsigned long reserved:1;
	unsigned long max_transfer:11;	// misnamed. Desired length might be better

	void *buffer;
	unsigned long data[4];	// free use by driver
} __attribute__ ((packed)) td_t;

typedef struct queue_head {
	link_pointer_t bredth;	// depth must = 0
	link_pointer_t depth;	// depth may vary randomly, ignore
	unsigned long int udata[2];
} __attribute__ ((packed)) queue_head_t;

typedef struct transaction {
	queue_head_t	*qh;
	td_t		*td_list;
	struct transaction *next;
} transaction_t;

#define MAX_USB_DEV 127
#define MAX_EP 8

typedef struct usbdev {
	unsigned short	port;
	uchar		address;
	uchar		controller;
	uchar		class;
	uchar		subclass;
	uchar		protocol;
	uchar		bulk_in;
	uchar		bulk_out;
	uchar		interrupt;
	uchar		lowspeed;
	uchar		toggle[MAX_EP];
	unsigned short	max_packet[MAX_EP];
	void		*private;
} usbdev_t;


typedef struct device_descriptor {
	uchar bLength;
	uchar type;
	
	uchar bcdVersion[2];
	uchar Class;
	uchar SubClass;
	uchar protocol;
	uchar max_packet;
	
	unsigned short idVendor;
	unsigned short idProduct;
	
	uchar bcdDevice[2];
	uchar iManufacturor;
	uchar iProduct;
	uchar iSerial;
	uchar bNumConfig;
} __attribute__ ((packed)) device_descriptor_t;
	
#define GET_DESCRIPTOR 6

typedef struct config_descriptor {
	uchar bLength;
	uchar type;
	
	unsigned short wTotalLength;
	uchar bNumInterfaces;
	uchar bConfigurationValue;
	uchar iConfiguration;

	uchar bmAttributes;
	uchar bMaxPower;
} __attribute__ ((packed)) config_descriptor_t;

typedef struct interface_descriptor {
	uchar bLength;
	uchar type;
	
	uchar bInterfaceNumber;
	uchar bAlternateSetting;

	uchar bNumEndpoints;
	uchar bInterfaceClass;
	uchar bInterfaceSubClass;
	uchar bInterfaceProtocol;
	uchar iInterface;
} __attribute__ ((packed)) interface_descriptor_t;

typedef struct endpoint_descriptor {
	uchar bLength;
	uchar type;

	uchar bEndpointAddress;
	uchar bmAttributes;
	unsigned short wMaxPacketSize;
	uchar bInterval;
} __attribute__ ((packed)) endpoint_descriptor_t;

typedef struct ctrl_msg {
	uchar bmRequestType;
	uchar bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
} __attribute__ ((packed)) ctrl_msg_t;

// Some descriptors for hubs, will be moved later
typedef struct hub_descriptor {
	uchar bLength;
	uchar type;

	uchar bNbrPorts;
	ushort wHubCharacteristics;
	uchar bPwrOn2PwrGood;
	uchar bHubCntrCurrent;

	uchar DeviceRemovable;	// assume bNbrPorts <=8
	uchar PortPwrCntrMask;
} __attribute__ ((packed)) hub_descriptor_t;

//#####################################################
int wait_head( queue_head_t *head, int count);

extern queue_head_t *free_qh;
extern queue_head_t *queue_heads;

queue_head_t *new_queue_head(void);
void free_queue_head( queue_head_t *qh);
void init_qh(void);

extern td_t *free_td_list;
extern td_t *tds;

void init_td(void);
td_t *new_td(void);
td_t *find_last_td(td_t *td);
void free_td( td_t *td);
link_pointer_t *queue_end( queue_head_t *queue);
void add_td( queue_head_t *head, td_t *td);

extern transaction_t transactions[MAX_TRANSACTIONS];
extern transaction_t *free_transactions;

void init_transactions(void);
void free_transaction( transaction_t *trans );
transaction_t *new_transaction(td_t *td);
transaction_t *add_transaction( transaction_t *trans, td_t *td);

extern link_pointer_t *frame_list[];

void init_framelist(uchar dev);

extern unsigned int hc_base[];
#define USBCMD hc_base
#define USBSTS (hc_base + 0x02)
#define USBINTR (hc_base + 0x04)
#define FRNUM ( hc_base + 0x06)
#define FLBASE ( hc_base + 0x08)
#define SOFMOD ( hc_base + 0x0c)
#define PORTSC1 ( hc_base + 0x10)
#define PORTSC2 ( hc_base + 0x12)

#define USBCMDRUN 0x01

#define USBSTSHALTED 0x20

void hc_reset(uchar dev);
int hc_stop(void);
int hc_start(uchar dev);
int hc_init(uchar dev, uchar function);

extern queue_head_t *sched_queue[];

void init_sched(uchar dev);
void uhci_init(void);
int poll_queue_head( queue_head_t *qh);
int wait_queue_complete( queue_head_t *qh);

extern int next_usb_dev;
usbdev_t usb_device[MAX_USB_DEV];


extern int num_polls;
extern int (*devpoll[MAX_POLLDEV])(uchar);
extern uchar parm[MAX_POLLDEV];

void init_devices(void);
transaction_t *_bulk_transfer( uchar devnum, uchar ep, unsigned int len, uchar *data);
transaction_t *ctrl_msg(uchar devnum, uchar request_type, uchar request, unsigned short wValue, unsigned short wIndex, unsigned short wLength, uchar *data);
int schedule_transaction( uchar dev, transaction_t *trans);
int wait_transaction( transaction_t *trans);
void unlink_transaction( uchar dev, transaction_t *trans);
int bulk_transfer( uchar devnum, uchar ep, unsigned int len, uchar *data);
int usb_control_msg( uchar devnum, uchar request_type, uchar request, unsigned short wValue, unsigned short wIndex, unsigned short wLength, void *data);
inline int set_address(uchar address);
inline int clear_stall(uchar device, uchar endpoint);
int configure_device(unsigned short  port, uchar controller, unsigned int lowspeed);
#endif
