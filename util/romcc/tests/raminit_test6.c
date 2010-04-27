typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;

typedef unsigned char      uint_least8_t;
typedef signed char        int_least8_t;
typedef unsigned short     uint_least16_t;
typedef signed short       int_least16_t;
typedef unsigned int       uint_least32_t;
typedef signed int         int_least32_t;

typedef unsigned char      uint_fast8_t;
typedef signed char        int_fast8_t;
typedef unsigned int       uint_fast16_t;
typedef signed int         int_fast16_t;
typedef unsigned int       uint_fast32_t;
typedef signed int         int_fast32_t;

typedef int                intptr_t;
typedef unsigned int       uintptr_t;

typedef long int           intmax_t;
typedef unsigned long int  uintmax_t;

static inline unsigned long apic_read(unsigned long reg)
{
	return *((volatile unsigned long *)(0xfee00000 +reg));
}
static inline void apic_write(unsigned long reg, unsigned long v)
{
	*((volatile unsigned long *)(0xfee00000 +reg)) = v;
}
static inline void apic_wait_icr_idle(void)
{
	do { } while ( apic_read( 0x300  ) & 0x01000  );
}

static void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}
static void outw(unsigned short value, unsigned short port)
{
	__builtin_outw(value, port);
}
static void outl(unsigned int value, unsigned short port)
{
	__builtin_outl(value, port);
}
static unsigned char inb(unsigned short port)
{
	return __builtin_inb(port);
}
static unsigned char inw(unsigned short port)
{
	return __builtin_inw(port);
}
static unsigned char inl(unsigned short port)
{
	return __builtin_inl(port);
}
static inline void outsb(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsb "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}
static inline void outsw(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsw "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}
static inline void outsl(uint16_t port, const void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; outsl "
		: "=S" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}
static inline void insb(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insb "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}
static inline void insw(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insw "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}
static inline void insl(uint16_t port, void *addr, unsigned long count)
{
	__asm__ __volatile__ (
		"cld ; rep ; insl "
		: "=D" (addr), "=c" (count)
		: "d"(port), "0"(addr), "1" (count)
		);
}
static inline void pnp_write_config(unsigned char port, unsigned char value, unsigned char reg)
{
	outb(reg, port);
	outb(value, port +1);
}
static inline unsigned char pnp_read_config(unsigned char port, unsigned char reg)
{
	outb(reg, port);
	return inb(port +1);
}
static inline void pnp_set_logical_device(unsigned char port, int device)
{
	pnp_write_config(port, device, 0x07);
}
static inline void pnp_set_enable(unsigned char port, int enable)
{
	pnp_write_config(port, enable?0x1:0x0, 0x30);
}
static inline int pnp_read_enable(unsigned char port)
{
	return !!pnp_read_config(port, 0x30);
}
static inline void pnp_set_iobase0(unsigned char port, unsigned iobase)
{
	pnp_write_config(port, (iobase >> 8) & 0xff, 0x60);
	pnp_write_config(port, iobase & 0xff, 0x61);
}
static inline void pnp_set_iobase1(unsigned char port, unsigned iobase)
{
	pnp_write_config(port, (iobase >> 8) & 0xff, 0x62);
	pnp_write_config(port, iobase & 0xff, 0x63);
}
static inline void pnp_set_irq0(unsigned char port, unsigned irq)
{
	pnp_write_config(port, irq, 0x70);
}
static inline void pnp_set_irq1(unsigned char port, unsigned irq)
{
	pnp_write_config(port, irq, 0x72);
}
static inline void pnp_set_drq(unsigned char port, unsigned drq)
{
	pnp_write_config(port, drq & 0xff, 0x74);
}
static void hlt(void)
{
	__builtin_hlt();
}
typedef __builtin_div_t div_t;
typedef __builtin_ldiv_t ldiv_t;
typedef __builtin_udiv_t udiv_t;
typedef __builtin_uldiv_t uldiv_t;
static div_t div(int numer, int denom)
{
	return __builtin_div(numer, denom);
}
static ldiv_t ldiv(long numer, long denom)
{
	return __builtin_ldiv(numer, denom);
}
static udiv_t udiv(unsigned numer, unsigned denom)
{
	return __builtin_udiv(numer, denom);
}
static uldiv_t uldiv(unsigned long numer, unsigned long denom)
{
	return __builtin_uldiv(numer, denom);
}
int log2(int value)
{

	return __builtin_bsr(value);
}
typedef unsigned device_t;
static unsigned char pci_read_config8(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inb(0xCFC + (addr & 3));
}
static unsigned short pci_read_config16(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inw(0xCFC + (addr & 2));
}
static unsigned int pci_read_config32(device_t dev, unsigned where)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	return inl(0xCFC);
}
static void pci_write_config8(device_t dev, unsigned where, unsigned char value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outb(value, 0xCFC + (addr & 3));
}
static void pci_write_config16(device_t dev, unsigned where, unsigned short value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outw(value, 0xCFC + (addr & 2));
}
static void pci_write_config32(device_t dev, unsigned where, unsigned int value)
{
	unsigned addr;
	addr = dev | where;
	outl(0x80000000 | (addr & ~3), 0xCF8);
	outl(value, 0xCFC);
}
static device_t pci_locate_device(unsigned pci_id, device_t dev)
{
	for(; dev <= ( ((( 255 ) & 0xFF) << 16) | (((  31 ) & 0x1f) << 11) | (((  7 )  & 0x7) << 8)) ; dev += ( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 1 )  & 0x7) << 8)) ) {
		unsigned int id;
		id = pci_read_config32(dev, 0);
		if (id == pci_id) {
			return dev;
		}
	}
	return (0xffffffffU) ;
}





static int uart_can_tx_byte(void)
{
	return inb(1016  + 0x05 ) & 0x20;
}
static void uart_wait_to_tx_byte(void)
{
	while(!uart_can_tx_byte())
		;
}
static void uart_wait_until_sent(void)
{
	while(!(inb(1016  + 0x05 ) & 0x40))
		;
}
static void uart_tx_byte(unsigned char data)
{
	uart_wait_to_tx_byte();
	outb(data, 1016  + 0x00 );

	uart_wait_until_sent();
}
static void uart_init(void)
{

	outb(0x0, 1016  + 0x01 );

	outb(0x01, 1016  + 0x02 );

	outb(0x80 | 3  , 1016  + 0x03 );
	outb((115200/ 115200 )  & 0xFF,   1016  + 0x00 );
	outb(((115200/ 115200 )  >> 8) & 0xFF,    1016  + 0x01 );
	outb(3  , 1016  + 0x03 );
}

static void __console_tx_byte(unsigned char byte)
{
	uart_tx_byte(byte);
}
static void __console_tx_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	__console_tx_byte(digit);
}
static void __console_tx_char(int loglevel, unsigned char byte)
{
	if (8   > loglevel) {
		uart_tx_byte(byte);
	}
}
static void __console_tx_hex8(int loglevel, unsigned char value)
{
	if (8   > loglevel) {
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
}
static void __console_tx_hex16(int loglevel, unsigned short value)
{
	if (8   > loglevel) {
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
}
static void __console_tx_hex32(int loglevel, unsigned int value)
{
	if (8   > loglevel) {
		__console_tx_nibble((value >> 28U) & 0x0fU);
		__console_tx_nibble((value >> 24U) & 0x0fU);
		__console_tx_nibble((value >> 20U) & 0x0fU);
		__console_tx_nibble((value >> 16U) & 0x0fU);
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
}
static void __console_tx_string(int loglevel, const char *str)
{
	if (8   > loglevel) {
		unsigned char ch;
		while((ch = *str++) != '\0') {
			__console_tx_byte(ch);
		}
	}
}
static void print_emerg_char(unsigned char byte) { __console_tx_char(0 , byte); }
static void print_emerg_hex8(unsigned char value){ __console_tx_hex8(0 , value); }
static void print_emerg_hex16(unsigned short value){ __console_tx_hex16(0 , value); }
static void print_emerg_hex32(unsigned int value) { __console_tx_hex32(0 , value); }
static void print_emerg(const char *str) { __console_tx_string(0 , str); }
static void print_alert_char(unsigned char byte) { __console_tx_char(1 , byte); }
static void print_alert_hex8(unsigned char value) { __console_tx_hex8(1 , value); }
static void print_alert_hex16(unsigned short value){ __console_tx_hex16(1 , value); }
static void print_alert_hex32(unsigned int value) { __console_tx_hex32(1 , value); }
static void print_alert(const char *str) { __console_tx_string(1 , str); }
static void print_crit_char(unsigned char byte) { __console_tx_char(2 , byte); }
static void print_crit_hex8(unsigned char value) { __console_tx_hex8(2 , value); }
static void print_crit_hex16(unsigned short value){ __console_tx_hex16(2 , value); }
static void print_crit_hex32(unsigned int value) { __console_tx_hex32(2 , value); }
static void print_crit(const char *str) { __console_tx_string(2 , str); }
static void print_err_char(unsigned char byte) { __console_tx_char(3 , byte); }
static void print_err_hex8(unsigned char value) { __console_tx_hex8(3 , value); }
static void print_err_hex16(unsigned short value){ __console_tx_hex16(3 , value); }
static void print_err_hex32(unsigned int value) { __console_tx_hex32(3 , value); }
static void print_err(const char *str) { __console_tx_string(3 , str); }
static void print_warning_char(unsigned char byte) { __console_tx_char(4 , byte); }
static void print_warning_hex8(unsigned char value) { __console_tx_hex8(4 , value); }
static void print_warning_hex16(unsigned short value){ __console_tx_hex16(4 , value); }
static void print_warning_hex32(unsigned int value) { __console_tx_hex32(4 , value); }
static void print_warning(const char *str) { __console_tx_string(4 , str); }
static void print_notice_char(unsigned char byte) { __console_tx_char(5 , byte); }
static void print_notice_hex8(unsigned char value) { __console_tx_hex8(5 , value); }
static void print_notice_hex16(unsigned short value){ __console_tx_hex16(5 , value); }
static void print_notice_hex32(unsigned int value) { __console_tx_hex32(5 , value); }
static void print_notice(const char *str) { __console_tx_string(5 , str); }
static void print_info_char(unsigned char byte) { __console_tx_char(6 , byte); }
static void print_info_hex8(unsigned char value) { __console_tx_hex8(6 , value); }
static void print_info_hex16(unsigned short value){ __console_tx_hex16(6 , value); }
static void print_info_hex32(unsigned int value) { __console_tx_hex32(6 , value); }
static void print_info(const char *str) { __console_tx_string(6 , str); }
static void print_debug_char(unsigned char byte) { __console_tx_char(7 , byte); }
static void print_debug_hex8(unsigned char value) { __console_tx_hex8(7 , value); }
static void print_debug_hex16(unsigned short value){ __console_tx_hex16(7 , value); }
static void print_debug_hex32(unsigned int value) { __console_tx_hex32(7 , value); }
static void print_debug(const char *str) { __console_tx_string(7 , str); }
static void print_spew_char(unsigned char byte) { __console_tx_char(8 , byte); }
static void print_spew_hex8(unsigned char value) { __console_tx_hex8(8 , value); }
static void print_spew_hex16(unsigned short value){ __console_tx_hex16(8 , value); }
static void print_spew_hex32(unsigned int value) { __console_tx_hex32(8 , value); }
static void print_spew(const char *str) { __console_tx_string(8 , str); }
static void console_init(void)
{
	static const char console_test[] =
		"\r\n\r\nLinuxBIOS-"
		"1.1.4"
		".0Fallback"
		" "
		"Thu Oct 9 20:29:48 MDT 2003"
		" starting...\r\n";
	print_info(console_test);
}
static void die(const char *str)
{
	print_emerg(str);
	do {
		hlt();
	} while(1);
}
static void write_phys(unsigned long addr, unsigned long value)
{
	asm volatile(
		"movnti %1, (%0)"
		:
		: "r" (addr), "r" (value)
		:
		);
}
static unsigned long read_phys(unsigned long addr)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	return *ptr;
}
static void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;

	print_debug("DRAM fill: ");
	print_debug_hex32(start);
	print_debug("-");
	print_debug_hex32(stop);
	print_debug("\r\n");
	for(addr = start; addr < stop ; addr += 4) {

		if (!(addr & 0xffff)) {
			print_debug_hex32(addr);
			print_debug("\r");
		}
		write_phys(addr, addr);
	};

	print_debug_hex32(addr);
	print_debug("\r\nDRAM filled\r\n");
}
static void ram_verify(unsigned long start, unsigned long stop)
{
	unsigned long addr;

	print_debug("DRAM verify: ");
	print_debug_hex32(start);
	print_debug_char('-');
	print_debug_hex32(stop);
	print_debug("\r\n");
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;

		if (!(addr & 0xffff)) {
			print_debug_hex32(addr);
			print_debug("\r");
		}
		value = read_phys(addr);
		if (value != addr) {

			print_err_hex32(addr);
			print_err_char(':');
			print_err_hex32(value);
			print_err("\r\n");
		}
	}

	print_debug_hex32(addr);
	print_debug("\r\nDRAM verified\r\n");
}
void ram_check(unsigned long start, unsigned long stop)
{
	int result;

	print_debug("Testing DRAM : ");
	print_debug_hex32(start);
	print_debug("-");
	print_debug_hex32(stop);
	print_debug("\r\n");
	ram_fill(start, stop);
	ram_verify(start, stop);
	print_debug("Done.\r\n");
}
static int enumerate_ht_chain(unsigned link)
{

	unsigned next_unitid, last_unitid;
	int reset_needed = 0;
	next_unitid = 1;
	do {
		uint32_t id;
		uint8_t hdr_type, pos;
		last_unitid = next_unitid;
		id = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8)) , 0x00 );

		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			break;
		}
		hdr_type = pci_read_config8(( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8)) , 0x0e );
		pos = 0;
		hdr_type &= 0x7f;
		if ((hdr_type == 0 ) ||
			(hdr_type == 1 )) {
			pos = pci_read_config8(( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8)) , 0x34 );
		}
		while(pos != 0) {
			uint8_t cap;
			cap = pci_read_config8(( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8)) , pos + 0 );
			if (cap == 0x08 ) {
				uint16_t flags;
				flags = pci_read_config16(( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8)) , pos + 2 );
				if ((flags >> 13) == 0) {
					unsigned count;
					flags &= ~0x1f;
					flags |= next_unitid & 0x1f;
					count = (flags >> 5) & 0x1f;
					pci_write_config16(( ((( 0 ) & 0xFF) << 16) | (((  0 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , pos + 2 , flags);
					next_unitid += count;
					break;
				}
			}
			pos = pci_read_config8(( ((( 0 ) & 0xFF) << 16) | (((  0 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , pos + 1 );
		}
	} while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
	return reset_needed;
}
static void enable_smbus(void)
{
	device_t dev;
	dev = pci_locate_device(((((  0x746b ) & 0xFFFF) << 16) | (( 0x1022 ) & 0xFFFF)) , 0);
	if (dev == (0xffffffffU) ) {
		die("SMBUS controller not found\r\n");
	}
	uint8_t enable;
	print_debug("SMBus controller enabled\r\n");
	pci_write_config32(dev, 0x58, 0x0f00  | 1);
	enable = pci_read_config8(dev, 0x41);
	pci_write_config8(dev, 0x41, enable | (1 << 7));

	outw(inw(0x0f00  + 0xe0 ), 0x0f00  + 0xe0 );
}
static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}
static int smbus_wait_until_ready(void)
{
	unsigned long loops;
	loops = (100*1000*10) ;
	do {
		unsigned short val;
		smbus_delay();
		val = inw(0x0f00  + 0xe0 );
		if ((val & 0x800) == 0) {
			break;
		}
		if(loops == ((100*1000*10)  / 2)) {
			outw(inw(0x0f00  + 0xe0 ),
				0x0f00  + 0xe0 );
		}
	} while(--loops);
	return loops?0:-2;
}
static int smbus_wait_until_done(void)
{
	unsigned long loops;
	loops = (100*1000*10) ;
	do {
		unsigned short val;
		smbus_delay();

		val = inw(0x0f00  + 0xe0 );
		if (((val & 0x8) == 0) | ((val & 0x437) != 0)) {
			break;
		}
	} while(--loops);
	return loops?0:-3;
}
static int smbus_read_byte(unsigned device, unsigned address)
{
	unsigned char global_control_register;
	unsigned char global_status_register;
	unsigned char byte;
	if (smbus_wait_until_ready() < 0) {
		return -2;
	}



	outw(inw(0x0f00  + 0xe2 ) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), 0x0f00  + 0xe2 );

	outw(((device & 0x7f) << 1) | 1, 0x0f00  + 0xe4 );

	outb(address & 0xFF, 0x0f00  + 0xe8 );

	outw((inw(0x0f00  + 0xe2 ) & ~7) | (0x2), 0x0f00  + 0xe2 );


	outw(inw(0x0f00  + 0xe0 ), 0x0f00  + 0xe0 );

	outw(0, 0x0f00  + 0xe6 );

	outw((inw(0x0f00  + 0xe2 ) | (1 << 3)), 0x0f00  + 0xe2 );

	if (smbus_wait_until_done() < 0) {
		return -3;
	}
	global_status_register = inw(0x0f00  + 0xe0 );

	byte = inw(0x0f00  + 0xe6 ) & 0xff;
	if (global_status_register != (1 << 4)) {
		return -1;
	}
	return byte;
}
static void smbus_write_byte(unsigned device, unsigned address, unsigned char val)
{
	return;
}
struct mem_controller {
	unsigned node_id;
	device_t f0, f1, f2, f3;
	uint8_t channel0[4];
	uint8_t channel1[4];
};
typedef __builtin_msr_t msr_t;
static msr_t rdmsr(unsigned long index)
{
	return __builtin_rdmsr(index);
}
static void wrmsr(unsigned long index, msr_t msr)
{
	__builtin_wrmsr(index, msr.lo, msr.hi);
}
struct tsc_struct {
	unsigned lo;
	unsigned hi;
};
typedef struct tsc_struct tsc_t;
static tsc_t rdtsc(void)
{
	tsc_t res;
	asm ("rdtsc"
		: "=a" (res.lo), "=d"(res.hi)
		:
		:
		);
	return res;
}
void init_timer(void)
{

	apic_write(0x320 , (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0));

	apic_write(0x3E0 , 0xB );

	apic_write(0x380 , 0xffffffff);
}
void udelay(unsigned usecs)
{
	uint32_t start, value, ticks;

	ticks = usecs * 200;
	start = apic_read(0x390 );
	do {
		value = apic_read(0x390 );
	} while((start - value) < ticks);

}
void mdelay(unsigned msecs)
{
	unsigned i;
	for(i = 0; i < msecs; i++) {
		udelay(1000);
	}
}
void delay(unsigned secs)
{
	unsigned i;
	for(i = 0; i < secs; i++) {
		mdelay(1000);
	}
}
int boot_cpu(void)
{
	volatile unsigned long *local_apic;
	unsigned long apic_id;
	int bsp;
	msr_t msr;
	msr = rdmsr(0x1b);
	bsp = !!(msr.lo & (1 << 8));
	return bsp;
}
static int cpu_init_detected(void)
{
	unsigned long htic;
	htic = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , 0x6c );
	return !!(htic & (1<<6) );
}
static int bios_reset_detected(void)
{
	unsigned long htic;
	htic = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , 0x6c );
	return (htic & (1<<4) ) && !(htic & (1<<5) );
}
static int cold_reset_detected(void)
{
	unsigned long htic;
	htic = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , 0x6c );
	return !(htic & (1<<4) );
}
static void distinguish_cpu_resets(unsigned node_id)
{
	uint32_t htic;
	device_t device;
	device = ( ((( 0 ) & 0xFF) << 16) | (((  0x18 + node_id ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) ;
	htic = pci_read_config32(device, 0x6c );
	htic |= (1<<4)  | (1<<5)  | (1<<6) ;
	pci_write_config32(device, 0x6c , htic);
}
static void set_bios_reset(void)
{
	unsigned long htic;
	htic = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , 0x6c );
	htic &= ~(1<<5) ;
	pci_write_config32(( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) , 0x6c , htic);
}
static void print_debug_pci_dev(unsigned dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 16) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 11) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 8) & 7);
}
static void print_pci_devices(void)
{
	device_t dev;
	for(dev = ( ((( 0 ) & 0xFF) << 16) | (((  0 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) ;
		dev <= ( ((( 0 ) & 0xFF) << 16) | (((  0x1f ) & 0x1f) << 11) | (((  0x7 )  & 0x7) << 8)) ;
		dev += ( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 1 )  & 0x7) << 8)) ) {
		uint32_t id;
		id = pci_read_config32(dev, 0x00 );
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		print_debug("\r\n");
	}
}
static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);
	print_debug("\r\n");

	for(i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\r\n");
		}
	}
}
static void dump_pci_devices(void)
{
	device_t dev;
	for(dev = ( ((( 0 ) & 0xFF) << 16) | (((  0 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) ;
		dev <= ( ((( 0 ) & 0xFF) << 16) | (((  0x1f ) & 0x1f) << 11) | (((  0x7 )  & 0x7) << 8)) ;
		dev += ( ((( 0 ) & 0xFF) << 16) | ((( 0 ) & 0x1f) << 11) | ((( 1 )  & 0x7) << 8)) ) {
		uint32_t id;
		id = pci_read_config32(dev, 0x00 );
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);
	}
}
static void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	print_debug("\r\n");
	for(i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".0: ");
			print_debug_hex8(device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					print_debug("\r\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\r\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\r\n");
		}
		device = ctrl->channel1[i];
		if (device) {
			int j;
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".1: ");
			print_debug_hex8(device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					print_debug("\r\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\r\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\r\n");
		}
	}
}

static unsigned int cpuid(unsigned int op)
{
	unsigned int ret;
	unsigned dummy2,dummy3,dummy4;
	asm volatile (
		"cpuid"
		: "=a" (ret), "=b" (dummy2), "=c" (dummy3), "=d" (dummy4)
		: "a" (op)
		);
	return ret;
}
static int is_cpu_rev_a0(void)
{
	return (cpuid(1) & 0xffff) == 0x0f10;
}
static int is_cpu_pre_c0(void)
{
	return (cpuid(1) & 0xffef) < 0x0f48;
}
static void memreset_setup(void)
{
	if (is_cpu_pre_c0()) {

		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), 0x0f00  + 0xc0 + 28);

		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(0<<0), 0x0f00  + 0xc0 + 29);
	}
	else {

		outb((0 << 7)|(0 << 6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), 0x0f00  + 0xc0 + 29);
	}
}
static void memreset(int controllers, const struct mem_controller *ctrl)
{
	if (is_cpu_pre_c0()) {
		udelay(800);

		outb((0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1<<0), 0x0f00  + 0xc0 + 28);
		udelay(90);
	}
}
static unsigned int generate_row(uint8_t node, uint8_t row, uint8_t maxnodes)
{

	uint32_t ret=0x00010101;
	static const unsigned int rows_2p[2][2] = {
		{ 0x00050101, 0x00010404 },
		{ 0x00010404, 0x00050101 }
	};
	if(maxnodes>2) {
		print_debug("this mainboard is only designed for 2 cpus\r\n");
		maxnodes=2;
	}
	if (!(node>=maxnodes || row>=maxnodes)) {
		ret=rows_2p[node][row];
	}
	return ret;
}
static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

static void coherent_ht_mainboard(unsigned cpus)
{
}

void cpu_ldtstop(unsigned cpus)
{
	uint32_t tmp;
	device_t dev;
	unsigned cnt;
	for(cnt=0; cnt<cpus; cnt++) {

        	pci_write_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24 ) & 0x1f) << 11) | ((( 3 )  & 0x7) << 8)) ,0x81,0x23);

		pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24 ) & 0x1f) << 11) | ((( 3 )  & 0x7) << 8)) ,0xd4,0x00000701);

		pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24 ) & 0x1f) << 11) | ((( 3 )  & 0x7) << 8)) ,0xd8,0x00000000);

		tmp=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24 ) & 0x1f) << 11) | ((( 2 )  & 0x7) << 8)) ,0x90);
		pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24 ) & 0x1f) << 11) | ((( 2 )  & 0x7) << 8)) ,0x90, tmp | (1<<24) );
	}
}





static void setup_resource_map(const unsigned int *register_values, int max)
{
	int i;
	print_debug("setting up resource map....\r\n");
	for(i = 0; i < max; i += 3) {
		device_t dev;
		unsigned where;
		unsigned long reg;
		dev = register_values[i] & ~0xff;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
	print_debug("done.\r\n");
}
static void setup_default_resource_map(void)
{
	static const unsigned int register_values[] = {


	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x44 ) & 0xFF)) , 0x0000f8f8, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x4C ) & 0xFF)) , 0x0000f8f8, 0x00000001,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x54 ) & 0xFF)) , 0x0000f8f8, 0x00000002,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x5C ) & 0xFF)) , 0x0000f8f8, 0x00000003,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x64 ) & 0xFF)) , 0x0000f8f8, 0x00000004,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x6C ) & 0xFF)) , 0x0000f8f8, 0x00000005,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x74 ) & 0xFF)) , 0x0000f8f8, 0x00000006,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x7C ) & 0xFF)) , 0x0000f8f8, 0x00000007,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x40 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x48 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x50 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x58 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x60 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x68 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x70 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x78 ) & 0xFF)) , 0x0000f8fc, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x84 ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x8C ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x94 ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x9C ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xA4 ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xAC ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xB4 ) & 0xFF)) , 0x00000048, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xBC ) & 0xFF)) , 0x00000048, 0x00ffff00,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x80 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x88 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x90 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x98 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xA0 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xA8 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xB0 ) & 0xFF)) , 0x000000f0, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xB8 ) & 0xFF)) , 0x000000f0, 0x00fc0003,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xC4 ) & 0xFF)) , 0xFE000FC8, 0x01fff000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xCC ) & 0xFF)) , 0xFE000FC8, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xD4 ) & 0xFF)) , 0xFE000FC8, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xDC ) & 0xFF)) , 0xFE000FC8, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xC0 ) & 0xFF)) , 0xFE000FCC, 0x00000003,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xC8 ) & 0xFF)) , 0xFE000FCC, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xD0 ) & 0xFF)) , 0xFE000FCC, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xD8 ) & 0xFF)) , 0xFE000FCC, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xE0 ) & 0xFF)) , 0x0000FC88, 0xff000003,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xE4 ) & 0xFF)) , 0x0000FC88, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xE8 ) & 0xFF)) , 0x0000FC88, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0xEC ) & 0xFF)) , 0x0000FC88, 0x00000000,
	};
	int max;
	max = sizeof(register_values)/sizeof(register_values[0]);
	setup_resource_map(register_values, max);
}
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	static const unsigned int register_values[] = {


	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x44 ) & 0xFF)) , 0x0000f8f8, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x4C ) & 0xFF)) , 0x0000f8f8, 0x00000001,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x54 ) & 0xFF)) , 0x0000f8f8, 0x00000002,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x5C ) & 0xFF)) , 0x0000f8f8, 0x00000003,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x64 ) & 0xFF)) , 0x0000f8f8, 0x00000004,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x6C ) & 0xFF)) , 0x0000f8f8, 0x00000005,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x74 ) & 0xFF)) , 0x0000f8f8, 0x00000006,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x7C ) & 0xFF)) , 0x0000f8f8, 0x00000007,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x40 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x48 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x50 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x58 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x60 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x68 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x70 ) & 0xFF)) , 0x0000f8fc, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 ) & 0x07) << 8) | ((  0x78 ) & 0xFF)) , 0x0000f8fc, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x40 ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x44 ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x48 ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x4C ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x50 ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x54 ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x58 ) & 0xFF)) , 0x001f01fe, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x5C ) & 0xFF)) , 0x001f01fe, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x60 ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x64 ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x68 ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x6C ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x70 ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x74 ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x78 ) & 0xFF)) , 0xC01f01ff, 0x00000000,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x7C ) & 0xFF)) , 0xC01f01ff, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x80 ) & 0xFF)) , 0xffff8888, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x88 ) & 0xFF)) , 0xe8088008, 0x02522001   ,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x8c ) & 0xFF)) , 0xff8fe08e, (0 << 20)|(0 << 8)|(0 << 4)|(0 << 0),

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x90 ) & 0xFF)) , 0xf0000000,
	(4 << 25)|(0 << 24)|
	(0 << 23)|(0 << 22)|(0 << 21)|(0 << 20)|
	(1 << 19)|(0 << 18)|(1 << 17)|(0 << 16)|
	(2 << 14)|(0 << 13)|(0 << 12)|
	(0 << 11)|(0 << 10)|(0 << 9)|(0 << 8)|
	(0 << 3) |(0 << 1) |(0 << 0),

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x94 ) & 0xFF)) , 0xc180f0f0,
	(0 << 29)|(0 << 28)|(0 << 27)|(0 << 26)|(0 << 25)|
	(0 << 20)|(0 << 19)|(3  << 16)|(0 << 8)|(0 << 0),

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 ) & 0x07) << 8) | ((  0x98 ) & 0xFF)) , 0xfc00ffff, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 ) & 0x07) << 8) | ((  0x58 ) & 0xFF)) , 0xffe0e0e0, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 ) & 0x07) << 8) | ((  0x5C ) & 0xFF)) , 0x0000003e, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 ) & 0x07) << 8) | ((  0x60 ) & 0xFF)) , 0xffffff00, 0x00000000,

	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 ) & 0x07) << 8) | ((  0x94 ) & 0xFF)) , 0xffff8000, 0x00000f70,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 ) & 0x07) << 8) | ((  0x90 ) & 0xFF)) , 0xffffff80, 0x00000002,
	( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 ) & 0x07) << 8) | ((  0x98 ) & 0xFF)) , 0x0000000f, 0x00068300,
	};
	int i;
	int max;
	print_debug("setting up CPU");
	print_debug_hex8(ctrl->node_id);
	print_debug(" northbridge registers\r\n");
	max = sizeof(register_values)/sizeof(register_values[0]);
	for(i = 0; i < max; i += 3) {
		device_t dev;
		unsigned where;
		unsigned long reg;
		dev = (register_values[i] & ~0xff) - ( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8))  + ctrl->f0;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
	print_debug("done.\r\n");
}
static int is_dual_channel(const struct mem_controller *ctrl)
{
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, 0x90 );
	return dcl & (1<<16) ;
}
static int is_opteron(const struct mem_controller *ctrl)
{

	uint32_t nbcap;
	nbcap = pci_read_config32(ctrl->f3, 0xE8 );
	return !!(nbcap & 0x0001 );
}
static int is_registered(const struct mem_controller *ctrl)
{

	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, 0x90 );
	return !(dcl & (1<<18) );
}
struct dimm_size {
	unsigned long side1;
	unsigned long side2;
};
static struct dimm_size spd_get_dimm_size(unsigned device)
{

	struct dimm_size sz;
	int value, low;
	sz.side1 = 0;
	sz.side2 = 0;

	value = spd_read_byte(device, 3);
	if (value < 0) goto out;
	sz.side1 += value & 0xf;
	value = spd_read_byte(device, 4);
	if (value < 0) goto out;
	sz.side1 += value & 0xf;
	value = spd_read_byte(device, 17);
	if (value < 0) goto out;
	sz.side1 += log2(value & 0xff);

	value = spd_read_byte(device, 7);
	if (value < 0) goto out;
	value &= 0xff;
	value <<= 8;

	low = spd_read_byte(device, 6);
	if (low < 0) goto out;
	value = value | (low & 0xff);
	sz.side1 += log2(value);

	value = spd_read_byte(device, 5);
	if (value <= 1) goto out;

	sz.side2 = sz.side1;
	value = spd_read_byte(device, 3);
	if (value < 0) goto out;
	if ((value & 0xf0) == 0) goto out;
	sz.side2 -= (value & 0x0f);
	sz.side2 += ((value >> 4) & 0x0f);
	value = spd_read_byte(device, 4);
	if (value < 0) goto out;
	sz.side2 -= (value & 0x0f);
	sz.side2 += ((value >> 4) & 0x0f);
 out:
	return sz;
}
static void set_dimm_size(const struct mem_controller *ctrl, struct dimm_size sz, unsigned index)
{
	uint32_t base0, base1, map;
	uint32_t dch;
	if (sz.side1 != sz.side2) {
		sz.side2 = 0;
	}
	map = pci_read_config32(ctrl->f2, 0x80 );
	map &= ~(0xf << (index + 4));


	base0 = base1 = 0;

	if (sz.side1 >= (25 +3)) {
		map |= (sz.side1 - (25 + 3)) << (index *4);
		base0 = (1 << ((sz.side1 - (25 + 3)) + 21)) | 1;
	}

	if (sz.side2 >= (25 + 3)) {
		base1 = (1 << ((sz.side2 - (25 + 3)) + 21)) | 1;
	}

	if (is_dual_channel(ctrl)) {
		base0 = (base0 << 1) | (base0 & 1);
		base1 = (base1 << 1) | (base1 & 1);
	}

	base0 &= ~0x001ffffe;
	base1 &= ~0x001ffffe;

	pci_write_config32(ctrl->f2, 0x40  + (((index << 1)+0)<<2), base0);
	pci_write_config32(ctrl->f2, 0x40  + (((index << 1)+1)<<2), base1);
	pci_write_config32(ctrl->f2, 0x80 , map);


	if (base0) {
		dch = pci_read_config32(ctrl->f2, 0x94 );
		dch |= (1 << 26)  << index;
		pci_write_config32(ctrl->f2, 0x94 , dch);
	}
}
static void spd_set_ram_size(const struct mem_controller *ctrl)
{
	int i;

	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		struct dimm_size sz;
		sz = spd_get_dimm_size(ctrl->channel0[i]);
		set_dimm_size(ctrl, sz, i);
	}
}
static void route_dram_accesses(const struct mem_controller *ctrl,
	unsigned long base_k, unsigned long limit_k)
{

	unsigned node_id;
	unsigned limit;
	unsigned base;
	unsigned index;
	unsigned limit_reg, base_reg;
	device_t device;
	node_id = ctrl->node_id;
	index = (node_id << 3);
	limit = (limit_k << 2);
	limit &= 0xffff0000;
	limit -= 0x00010000;
	limit |= ( 0 << 8) | (node_id << 0);
	base = (base_k << 2);
	base &= 0xffff0000;
	base |= (0 << 8) | (1<<1) | (1<<0);
	limit_reg = 0x44 + index;
	base_reg = 0x40 + index;
	for(device = ( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 )  & 0x7) << 8)) ; device <= ( ((( 0 ) & 0xFF) << 16) | (((  0x1f ) & 0x1f) << 11) | (((  1 )  & 0x7) << 8)) ; device += ( ((( 0 ) & 0xFF) << 16) | (((  1 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) ) {
		pci_write_config32(device, limit_reg, limit);
		pci_write_config32(device, base_reg, base);
	}
}
static void set_top_mem(unsigned tom_k)
{

	if (!tom_k) {
		set_bios_reset();
		print_debug("No memory - reset");

		pci_write_config8(( ((( 0 ) & 0xFF) << 16) | (((  0x04 ) & 0x1f) << 11) | (((  3 )  & 0x7) << 8)) , 0x41, 0xf1);

		outb(0x0e, 0x0cf9);
	}

	print_debug("RAM: 0x");
	print_debug_hex32(tom_k);
	print_debug(" KB\r\n");

	msr_t msr;
	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(0xC001001D , msr);

	if (tom_k >= 0x003f0000) {
		tom_k = 0x3f0000;
	}
	msr.lo = (tom_k & 0x003fffff) << 10;
	msr.hi = (tom_k & 0xffc00000) >> 22;
	wrmsr(0xC001001A , msr);
}
static unsigned long interleave_chip_selects(const struct mem_controller *ctrl)
{

	static const uint32_t csbase_low[] = {
	 	(1 << (13 - 4)),
	 	(1 << (14 - 4)),
	 	(1 << (14 - 4)),
	 	(1 << (15 - 4)),
	 	(1 << (15 - 4)),
	 	(1 << (16 - 4)),
	 	(1 << (16 - 4)),
	};
	uint32_t csbase_inc;
	int chip_selects, index;
	int bits;
	int dual_channel;
	unsigned common_size;
	uint32_t csbase, csmask;

	chip_selects = 0;
	common_size = 0;
	for(index = 0; index < 8; index++) {
		unsigned size;
		uint32_t value;

		value = pci_read_config32(ctrl->f2, 0x40  + (index << 2));


		if (!(value & 1)) {
			continue;
		}
		chip_selects++;
		size = value >> 21;
		if (common_size == 0) {
			common_size = size;
		}

		if (common_size != size) {
			return 0;
		}
	}

	bits = log2(chip_selects);
	if (((1 << bits) != chip_selects) || (bits < 1) || (bits > 3)) {
		return 0;

	}

	if ((bits == 3) && (common_size == (1 << (32 - 3)))) {
		print_debug("8 4GB chip selects cannot be interleaved\r\n");
		return 0;
	}

	if (is_dual_channel(ctrl)) {
		csbase_inc = csbase_low[log2(common_size) - 1] << 1;
	} else {
		csbase_inc = csbase_low[log2(common_size)];
	}

	csbase = 0 | 1;
	csmask = (((common_size  << bits) - 1) << 21);
	csmask |= 0xfe00 & ~((csbase_inc << bits) - csbase_inc);
	for(index = 0; index < 8; index++) {
		uint32_t value;
		value = pci_read_config32(ctrl->f2, 0x40  + (index << 2));

		if (!(value & 1)) {
			continue;
		}
		pci_write_config32(ctrl->f2, 0x40  + (index << 2), csbase);
		pci_write_config32(ctrl->f2, 0x60  + (index << 2), csmask);
		csbase += csbase_inc;
	}

	print_debug("Interleaved\r\n");

	return common_size << (15 + bits);
}
static unsigned long order_chip_selects(const struct mem_controller *ctrl)
{
	unsigned long tom;


	tom = 0;
	for(;;) {

		unsigned index, canidate;
		uint32_t csbase, csmask;
		unsigned size;
		csbase = 0;
		canidate = 0;
		for(index = 0; index < 8; index++) {
			uint32_t value;
			value = pci_read_config32(ctrl->f2, 0x40  + (index << 2));

			if (!(value & 1)) {
				continue;
			}


			if (value <= csbase) {
				continue;
			}


			if (tom & (1 << (index + 24))) {
				continue;
			}

			csbase = value;
			canidate = index;
		}

		if (csbase == 0) {
			break;
		}

		size = csbase >> 21;

		tom |= (1 << (canidate + 24));

		csbase = (tom << 21) | 1;

		tom += size;

		csmask = ((size -1) << 21);
		csmask |= 0xfe00;

		pci_write_config32(ctrl->f2, 0x40  + (canidate << 2), csbase);

		pci_write_config32(ctrl->f2, 0x60  + (canidate << 2), csmask);

	}

	return (tom & ~0xff000000) << 15;
}
static void order_dimms(const struct mem_controller *ctrl)
{
	unsigned long tom, tom_k, base_k;
	unsigned node_id;
	tom_k = interleave_chip_selects(ctrl);
	if (!tom_k) {
		tom_k = order_chip_selects(ctrl);
	}

	base_k = 0;
	for(node_id = 0; node_id < ctrl->node_id; node_id++) {
		uint32_t limit, base;
		unsigned index;
		index = node_id << 3;
		base = pci_read_config32(ctrl->f1, 0x40 + index);

		if ((base & 3) == 3) {
			limit = pci_read_config32(ctrl->f1, 0x44 + index);
			base_k = ((limit + 0x00010000) & 0xffff0000) >> 2;
		}
	}
	tom_k += base_k;
	route_dram_accesses(ctrl, base_k, tom_k);
	set_top_mem(tom_k);
}
static void disable_dimm(const struct mem_controller *ctrl, unsigned index)
{
	print_debug("disabling dimm");
	print_debug_hex8(index);
	print_debug("\r\n");
	pci_write_config32(ctrl->f2, 0x40  + (((index << 1)+0)<<2), 0);
	pci_write_config32(ctrl->f2, 0x40  + (((index << 1)+1)<<2), 0);
}
static void spd_handle_unbuffered_dimms(const struct mem_controller *ctrl)
{
	int i;
	int registered;
	int unbuffered;
	uint32_t dcl;
	unbuffered = 0;
	registered = 0;
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		int value;
		value = spd_read_byte(ctrl->channel0[i], 21);
		if (value < 0) {
			disable_dimm(ctrl, i);
			continue;
		}

		if (value & (1 << 1)) {
			registered = 1;
		}

		else {
			unbuffered = 1;
		}
	}
	if (unbuffered && registered) {
		die("Mixed buffered and registered dimms not supported");
	}
	if (unbuffered && is_opteron(ctrl)) {
		die("Unbuffered Dimms not supported on Opteron");
	}
	dcl = pci_read_config32(ctrl->f2, 0x90 );
	dcl &= ~(1<<18) ;
	if (unbuffered) {
		dcl |= (1<<18) ;
	}
	pci_write_config32(ctrl->f2, 0x90 , dcl);
}
static void spd_enable_2channels(const struct mem_controller *ctrl)
{
	int i;
	uint32_t nbcap;


	static const unsigned addresses[] = {
		2,
		3,
		4,
		5,
		6,
		7,
		9,
		11,
		13,
		17,
		18,
		21,
		23,
		26,
		27,
		28,
		29,
		30,
		41,
		42,
	};
	nbcap = pci_read_config32(ctrl->f3, 0xE8 );
	if (!(nbcap & 0x0001 )) {
		return;
	}
	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		unsigned device0, device1;
		int value0, value1;
		int j;
		device0 = ctrl->channel0[i];
		device1 = ctrl->channel1[i];
		if (!device1)
			return;
		for(j = 0; j < sizeof(addresses)/sizeof(addresses[0]); j++) {
			unsigned addr;
			addr = addresses[j];
			value0 = spd_read_byte(device0, addr);
			if (value0 < 0) {
				break;
			}
			value1 = spd_read_byte(device1, addr);
			if (value1 < 0) {
				return;
			}
			if (value0 != value1) {
				return;
			}
		}
	}
	print_debug("Enabling dual channel memory\r\n");
	uint32_t dcl;
	dcl = pci_read_config32(ctrl->f2, 0x90 );
	dcl &= ~(1<<19) ;
	dcl |= (1<<16) ;
	pci_write_config32(ctrl->f2, 0x90 , dcl);
}
struct mem_param {
	uint8_t cycle_time;
	uint8_t divisor;
	uint8_t tRC;
	uint8_t tRFC;
	uint32_t dch_memclk;
	uint16_t dch_tref4k, dch_tref8k;
	uint8_t	 dtl_twr;
	char name[9];
};
static const struct mem_param *get_mem_param(unsigned min_cycle_time)
{
	static const struct mem_param speed[] = {
		{
			.name	    = "100Mhz\r\n",
			.cycle_time = 0xa0,
			.divisor    = (10 <<1),
			.tRC	    = 0x46,
			.tRFC	    = 0x50,
			.dch_memclk = 0  << 20 ,
			.dch_tref4k = 0x00 ,
			.dch_tref8k = 0x08 ,
			.dtl_twr    = 2,
		},
		{
			.name	    = "133Mhz\r\n",
			.cycle_time = 0x75,
			.divisor    = (7<<1)+1,
			.tRC	    = 0x41,
			.tRFC	    = 0x4B,
			.dch_memclk = 2  << 20 ,
			.dch_tref4k = 0x01 ,
			.dch_tref8k = 0x09 ,
			.dtl_twr    = 2,
		},
		{
			.name	    = "166Mhz\r\n",
			.cycle_time = 0x60,
			.divisor    = (6<<1),
			.tRC	    = 0x3C,
			.tRFC	    = 0x48,
			.dch_memclk = 5  << 20 ,
			.dch_tref4k = 0x02 ,
			.dch_tref8k = 0x0A ,
			.dtl_twr    = 3,
		},
		{
			.name	    = "200Mhz\r\n",
			.cycle_time = 0x50,
			.divisor    = (5<<1),
			.tRC	    = 0x37,
			.tRFC	    = 0x46,
			.dch_memclk = 7  << 20 ,
			.dch_tref4k = 0x03 ,
			.dch_tref8k = 0x0B ,
			.dtl_twr    = 3,
		},
		{
			.cycle_time = 0x00,
		},
	};
	const struct mem_param *param;
	for(param = &speed[0]; param->cycle_time ; param++) {
		if (min_cycle_time > (param+1)->cycle_time) {
			break;
		}
	}
	if (!param->cycle_time) {
		die("min_cycle_time to low");
	}
	print_debug(param->name);
	return param;
}
static const struct mem_param *spd_set_memclk(const struct mem_controller *ctrl)
{

	const struct mem_param *param;
	unsigned min_cycle_time, min_latency;
	int i;
	uint32_t value;
	static const int latency_indicies[] = { 26, 23, 9 };
	static const unsigned char min_cycle_times[] = {
		[0 ] = 0x50,
		[1 ] = 0x60,
		[2 ] = 0x75,
		[3 ] = 0xa0,
	};
	value = pci_read_config32(ctrl->f3, 0xE8 );
	min_cycle_time = min_cycle_times[(value >> 5 ) & 3 ];
	min_latency = 2;

	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		int new_cycle_time, new_latency;
		int index;
		int latencies;
		int latency;

		new_cycle_time = 0xa0;
		new_latency = 5;
		latencies = spd_read_byte(ctrl->channel0[i], 18);
		if (latencies <= 0) continue;

		latency = log2(latencies) -2;

		for(index = 0; index < 3; index++, latency++) {
			int value;
			if ((latency < 2) || (latency > 4) ||
				(!(latencies & (1 << latency)))) {
				continue;
			}
			value = spd_read_byte(ctrl->channel0[i], latency_indicies[index]);
			if (value < 0) {
				continue;
			}

			if ((value >= min_cycle_time) && (value < new_cycle_time)) {
				new_cycle_time = value;
				new_latency = latency;
			}
		}
		if (new_latency > 4){
			continue;
		}

		if (new_cycle_time > min_cycle_time) {
			min_cycle_time = new_cycle_time;
		}

		if (new_latency > min_latency) {
			min_latency = new_latency;
		}
	}


	for(i = 0; (i < 4) && (ctrl->channel0[i]); i++) {
		int latencies;
		int latency;
		int index;
		int value;
		int dimm;
		latencies = spd_read_byte(ctrl->channel0[i], 18);
		if (latencies <= 0) {
			goto dimm_err;
		}

		latency = log2(latencies) -2;

		for(index = 0; index < 3; index++, latency++) {
			if (!(latencies & (1 << latency))) {
				continue;
			}
			if (latency == min_latency)
				break;
		}

		if ((latency != min_latency) || (index >= 3)) {
			goto dimm_err;
		}


		value = spd_read_byte(ctrl->channel0[i], latency_indicies[index]);


		if (value <= min_cycle_time) {
			continue;
		}

	dimm_err:
		disable_dimm(ctrl, i);
	}

	param = get_mem_param(min_cycle_time);

	value = pci_read_config32(ctrl->f2, 0x94 );
	value &= ~(0x7  << 20 );
	value |= param->dch_memclk;
	pci_write_config32(ctrl->f2, 0x94 , value);
	static const unsigned latencies[] = { 1 , 5 , 2  };

	value = pci_read_config32(ctrl->f2, 0x88 );
	value &= ~(0x7  << 0 );
	value |= latencies[min_latency - 2] << 0 ;
	pci_write_config32(ctrl->f2, 0x88 , value);

	return param;
}
static int update_dimm_Trc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 41);
	if (value < 0) return -1;
	if ((value == 0) || (value == 0xff)) {
		value = param->tRC;
	}
	clocks = ((value << 1) + param->divisor - 1)/param->divisor;
	if (clocks < 7 ) {
		clocks = 7 ;
	}
	if (clocks > 22 ) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	old_clocks = ((dtl >> 4 ) & 0xf ) + 7 ;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(0xf  << 4 );
	dtl |=	((clocks - 7 ) << 4 );
	pci_write_config32(ctrl->f2, 0x88 , dtl);
	return 0;
}
static int update_dimm_Trfc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 42);
	if (value < 0) return -1;
	if ((value == 0) || (value == 0xff)) {
		value = param->tRFC;
	}
	clocks = ((value << 1) + param->divisor - 1)/param->divisor;
	if (clocks < 9 ) {
		clocks = 9 ;
	}
	if (clocks > 24 ) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	old_clocks = ((dtl >> 8 ) & 0xf ) + 9 ;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(0xf  << 8 );
	dtl |= ((clocks - 9 ) << 8 );
	pci_write_config32(ctrl->f2, 0x88 , dtl);
	return 0;
}
static int update_dimm_Trcd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 29);
	if (value < 0) return -1;
	clocks = (value + ((param->divisor & 0xff) << 1) -1)/((param->divisor & 0xff) << 1);
	if (clocks < 2 ) {
		clocks = 2 ;
	}
	if (clocks > 6 ) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	old_clocks = ((dtl >> 12 ) & 0x7 ) + 0 ;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(0x7  << 12 );
	dtl |= ((clocks - 0 ) << 12 );
	pci_write_config32(ctrl->f2, 0x88 , dtl);
	return 0;
}
static int update_dimm_Trrd(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 28);
	if (value < 0) return -1;
	clocks = (value + ((param->divisor & 0xff) << 1) -1)/((param->divisor & 0xff) << 1);
	if (clocks < 2 ) {
		clocks = 2 ;
	}
	if (clocks > 4 ) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	old_clocks = ((dtl >> 16 ) & 0x7 ) + 0 ;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(0x7  << 16 );
	dtl |= ((clocks - 0 ) << 16 );
	pci_write_config32(ctrl->f2, 0x88 , dtl);
	return 0;
}
static int update_dimm_Tras(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 30);
	if (value < 0) return -1;
	clocks = ((value << 1) + param->divisor - 1)/param->divisor;
	if (clocks < 5 ) {
		clocks = 5 ;
	}
	if (clocks > 15 ) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	old_clocks = ((dtl >> 20 ) & 0xf ) + 0 ;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(0xf  << 20 );
	dtl |= ((clocks - 0 ) << 20 );
	pci_write_config32(ctrl->f2, 0x88 , dtl);
	return 0;
}
static int update_dimm_Trp(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	unsigned clocks, old_clocks;
	uint32_t dtl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 27);
	if (value < 0) return -1;
	clocks = (value + ((param->divisor & 0xff) << 1) - 1)/((param->divisor & 0xff) << 1);
	if (clocks < 2 ) {
		clocks = 2 ;
	}
	if (clocks > 6 ) {
		return -1;
	}
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	old_clocks = ((dtl >> 24 ) & 0x7 ) + 0 ;
	if (old_clocks > clocks) {
		clocks = old_clocks;
	}
	dtl &= ~(0x7  << 24 );
	dtl |= ((clocks - 0 ) << 24 );
	pci_write_config32(ctrl->f2, 0x88 , dtl);
	return 0;
}
static void set_Twr(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dtl;
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	dtl &= ~(0x1  << 28 );
	dtl |= (param->dtl_twr - 2 ) << 28 ;
	pci_write_config32(ctrl->f2, 0x88 , dtl);
}
static void init_Tref(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth;
	dth = pci_read_config32(ctrl->f2, 0x8c );
	dth &= ~(0x1f  << 8 );
	dth |= (param->dch_tref4k << 8 );
	pci_write_config32(ctrl->f2, 0x8c , dth);
}
static int update_dimm_Tref(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dth;
	int value;
	unsigned tref, old_tref;
	value = spd_read_byte(ctrl->channel0[i], 3);
	if (value < 0) return -1;
	value &= 0xf;
	tref = param->dch_tref8k;
	if (value == 12) {
		tref = param->dch_tref4k;
	}
	dth = pci_read_config32(ctrl->f2, 0x8c );
	old_tref = (dth >> 8 ) & 0x1f ;
	if ((value == 12) && (old_tref == param->dch_tref4k)) {
		tref = param->dch_tref4k;
	} else {
		tref = param->dch_tref8k;
	}
	dth &= ~(0x1f  << 8 );
	dth |= (tref << 8 );
	pci_write_config32(ctrl->f2, 0x8c , dth);
	return 0;
}
static int update_dimm_x4(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dcl;
	int value;
	int dimm;
	value = spd_read_byte(ctrl->channel0[i], 13);
	if (value < 0) {
		return -1;
	}
	dimm = i;
	dimm += 20 ;
	dcl = pci_read_config32(ctrl->f2, 0x90 );
	dcl &= ~(1 << dimm);
	if (value == 4) {
		dcl |= (1 << dimm);
	}
	pci_write_config32(ctrl->f2, 0x90 , dcl);
	return 0;
}
static int update_dimm_ecc(const struct mem_controller *ctrl, const struct mem_param *param, int i)
{
	uint32_t dcl;
	int value;
	value = spd_read_byte(ctrl->channel0[i], 11);
	if (value < 0) {
		return -1;
	}
	if (value != 2) {
		dcl = pci_read_config32(ctrl->f2, 0x90 );
		dcl &= ~(1<<17) ;
		pci_write_config32(ctrl->f2, 0x90 , dcl);
	}
	return 0;
}
static int count_dimms(const struct mem_controller *ctrl)
{
	int dimms;
	unsigned index;
	dimms = 0;
	for(index = 0; index < 8; index += 2) {
		uint32_t csbase;
		csbase = pci_read_config32(ctrl->f2, (0x40  + index << 2));
		if (csbase & 1) {
			dimms += 1;
		}
	}
	return dimms;
}
static void set_Twtr(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth;
	unsigned clocks;
	clocks = 1;
	dth = pci_read_config32(ctrl->f2, 0x8c );
	dth &= ~(0x1  << 0 );
	dth |= ((clocks - 1 ) << 0 );
	pci_write_config32(ctrl->f2, 0x8c , dth);
}
static void set_Trwt(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dth, dtl;
	unsigned divisor;
	unsigned latency;
	unsigned clocks;
	clocks = 0;
	dtl = pci_read_config32(ctrl->f2, 0x88 );
	latency = (dtl >> 0 ) & 0x7 ;
	divisor = param->divisor;
	if (is_opteron(ctrl)) {
		if (latency == 1 ) {
			if (divisor == ((6 << 0) + 0)) {

				clocks = 3;
			}
			else if (divisor > ((6 << 0)+0)) {

				clocks = 2;
			}
		}
		else if (latency == 5 ) {
			clocks = 3;
		}
		else if (latency == 2 ) {
			if (divisor == ((6 << 0)+0)) {

				clocks = 4;
			}
			else if (divisor > ((6 << 0)+0)) {

				clocks = 3;
			}
		}
	}
	else   {
		if (is_registered(ctrl)) {
			if (latency == 1 ) {
				clocks = 2;
			}
			else if (latency == 5 ) {
				clocks = 3;
			}
			else if (latency == 2 ) {
				clocks = 3;
			}
		}
		else  {
			if (latency == 1 ) {
				clocks = 3;
			}
			else if (latency == 5 ) {
				clocks = 4;
			}
			else if (latency == 2 ) {
				clocks = 4;
			}
		}
	}
	if ((clocks < 1 ) || (clocks > 6 )) {
		die("Unknown Trwt");
	}

	dth = pci_read_config32(ctrl->f2, 0x8c );
	dth &= ~(0x7  << 4 );
	dth |= ((clocks - 1 ) << 4 );
	pci_write_config32(ctrl->f2, 0x8c , dth);
	return;
}
static void set_Twcl(const struct mem_controller *ctrl, const struct mem_param *param)
{

	uint32_t dth;
	unsigned clocks;
	if (is_registered(ctrl)) {
		clocks = 2;
	} else {
		clocks = 1;
	}
	dth = pci_read_config32(ctrl->f2, 0x8c );
	dth &= ~(0x7  << 20 );
	dth |= ((clocks - 1 ) << 20 );
	pci_write_config32(ctrl->f2, 0x8c , dth);
}
static void set_read_preamble(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;
	unsigned divisor;
	unsigned rdpreamble;
	divisor = param->divisor;
	dch = pci_read_config32(ctrl->f2, 0x94 );
	dch &= ~(0xf  << 8 );
	rdpreamble = 0;
	if (is_registered(ctrl)) {
		if (divisor == ((10 << 1)+0)) {

			rdpreamble = ((9 << 1)+ 0);
		}
		else if (divisor == ((7 << 1)+1)) {

			rdpreamble = ((8 << 1)+0);
		}
		else if (divisor == ((6 << 1)+0)) {

			rdpreamble = ((7 << 1)+1);
		}
		else if (divisor == ((5 << 1)+0)) {

			rdpreamble = ((7 << 1)+0);
		}
	}
	else {
		int slots;
		int i;
		slots = 0;
		for(i = 0; i < 4; i++) {
			if (ctrl->channel0[i]) {
				slots += 1;
			}
		}
		if (divisor == ((10 << 1)+0)) {

			if (slots <= 2) {

				rdpreamble = ((9 << 1)+0);
			} else {

				rdpreamble = ((14 << 1)+0);
			}
		}
		else if (divisor == ((7 << 1)+1)) {

			if (slots <= 2) {

				rdpreamble = ((7 << 1)+0);
			} else {

				rdpreamble = ((11 << 1)+0);
			}
		}
		else if (divisor == ((6 << 1)+0)) {

			if (slots <= 2) {

				rdpreamble = ((7 << 1)+0);
			} else {

				rdpreamble = ((9 << 1)+0);
			}
		}
		else if (divisor == ((5 << 1)+0)) {

			if (slots <= 2) {

				rdpreamble = ((5 << 1)+0);
			} else {

				rdpreamble = ((7 << 1)+0);
			}
		}
	}
	if ((rdpreamble < ((2<<1)+0) ) || (rdpreamble > ((9<<1)+1) )) {
		die("Unknown rdpreamble");
	}
	dch |= (rdpreamble - ((2<<1)+0) ) << 8 ;
	pci_write_config32(ctrl->f2, 0x94 , dch);
}
static void set_max_async_latency(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;
	int i;
	unsigned async_lat;
	int dimms;
	dimms = count_dimms(ctrl);
	dch = pci_read_config32(ctrl->f2, 0x94 );
	dch &= ~(0xf  << 0 );
	async_lat = 0;
	if (is_registered(ctrl)) {
		if (dimms == 4) {

			async_lat = 9;
		}
		else {

			async_lat = 8;
		}
	}
	else {
		if (dimms > 3) {
			die("Too many unbuffered dimms");
		}
		else if (dimms == 3) {

			async_lat = 7;
		}
		else {

			async_lat = 6;
		}
	}
	dch |= ((async_lat - 0 ) << 0 );
	pci_write_config32(ctrl->f2, 0x94 , dch);
}
static void set_idle_cycle_limit(const struct mem_controller *ctrl, const struct mem_param *param)
{
	uint32_t dch;

	dch = pci_read_config32(ctrl->f2, 0x94 );
	dch &= ~(0x7  << 16 );
	dch |= 3  << 16 ;
	dch |= (1 << 19) ;
	pci_write_config32(ctrl->f2, 0x94 , dch);
}
static void spd_set_dram_timing(const struct mem_controller *ctrl, const struct mem_param *param)
{
	int dimms;
	int i;
	int rc;

	init_Tref(ctrl, param);
	for(i = 0; (i < 4) && ctrl->channel0[i]; i++) {
		int rc;

		if (update_dimm_Trc (ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trfc(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trcd(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trrd(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Tras(ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_Trp (ctrl, param, i) < 0) goto dimm_err;

		if (update_dimm_Tref(ctrl, param, i) < 0) goto dimm_err;

		if (update_dimm_x4 (ctrl, param, i) < 0) goto dimm_err;
		if (update_dimm_ecc(ctrl, param, i) < 0) goto dimm_err;
		continue;
	dimm_err:
		disable_dimm(ctrl, i);

	}

	set_Twr(ctrl, param);

	set_Twtr(ctrl, param);
	set_Trwt(ctrl, param);
	set_Twcl(ctrl, param);

	set_read_preamble(ctrl, param);
	set_max_async_latency(ctrl, param);
	set_idle_cycle_limit(ctrl, param);
}
static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	const struct mem_param *param;
	spd_enable_2channels(ctrl);
	spd_set_ram_size(ctrl);
	spd_handle_unbuffered_dimms(ctrl);
	param = spd_set_memclk(ctrl);
	spd_set_dram_timing(ctrl, param);
	order_dimms(ctrl);
}
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;

	for(i = 0; i < controllers; i++) {
		uint32_t dch;
		dch = pci_read_config32(ctrl[i].f2, 0x94 );
		dch |= (1 << 25) ;
		pci_write_config32(ctrl[i].f2, 0x94 , dch);
	}

	memreset(controllers, ctrl);
	for(i = 0; i < controllers; i++) {
		uint32_t dcl;

		dcl = pci_read_config32(ctrl[i].f2, 0x90 );
		if (dcl & (1<<17) ) {
			uint32_t mnc;
			print_debug("ECC enabled\r\n");
			mnc = pci_read_config32(ctrl[i].f3, 0x44 );
			mnc |= (1 << 22) ;
			if (dcl & (1<<16) ) {
				mnc |= (1 << 23) ;
			}
			pci_write_config32(ctrl[i].f3, 0x44 , mnc);
		}
		dcl |= (1<<3) ;
		pci_write_config32(ctrl[i].f2, 0x90 , dcl);
		dcl &= ~(1<<3) ;
		dcl &= ~(1<<0) ;
		dcl &= ~(1<<1) ;
		dcl &= ~(1<<2) ;
		dcl |= (1<<8) ;
		pci_write_config32(ctrl[i].f2, 0x90 , dcl);
	}
	for(i = 0; i < controllers; i++) {
		uint32_t dcl;
		print_debug("Initializing memory: ");
		int loops = 0;
		do {
			dcl = pci_read_config32(ctrl[i].f2, 0x90 );
			loops += 1;
			if ((loops & 1023) == 0) {
				print_debug(".");
			}
		} while(((dcl & (1<<8) ) != 0) && (loops < 300000 ));
		if (loops >= 300000 ) {
			print_debug(" failed\r\n");
		} else {
			print_debug(" done\r\n");
		}
		if (dcl & (1<<17) ) {
			print_debug("Clearing memory: ");
			if (!is_cpu_pre_c0()) {

				dcl &= ~((1<<11)  | (1<<10) );
				pci_write_config32(ctrl[i].f2, 0x90 , dcl);
				do {
					dcl = pci_read_config32(ctrl[i].f2, 0x90 );
				} while(((dcl & (1<<11) ) == 0) || ((dcl & (1<<10) ) == 0) );
			}
			uint32_t base, last_scrub_k, scrub_k;
			uint32_t cnt,zstart,zend;
			msr_t msr,msr_201;

			pci_write_config32(ctrl[i].f3, 0x58 ,
				(0  << 16) | (0  << 8) | (0  << 0));

			msr_201 = rdmsr(0x201);
			zstart = pci_read_config32(ctrl[0].f1, 0x40 + (i*8));
			zend = pci_read_config32(ctrl[0].f1, 0x44 + (i*8));
			zstart >>= 16;
			zend >>=16;
			print_debug("addr ");
			print_debug_hex32(zstart);
			print_debug("-");
			print_debug_hex32(zend);
			print_debug("\r\n");


			msr = rdmsr(0x2ff );
			msr.lo &= ~(1<<10);
			wrmsr(0x2ff , msr);

			msr = rdmsr(0xc0010015);
			msr.lo |= (1<<17);
			wrmsr(0xc0010015,msr);
			for(;zstart<zend;zstart+=4) {

				if(zstart == 0x0fc)
					continue;


				__asm__ volatile(
					"movl  %%cr0, %0\n\t"
					"orl  $0x40000000, %0\n\t"
					"movl  %0, %%cr0\n\t"
					:"=r" (cnt)
					);


				msr.lo = 1 + ((zstart&0x0ff)<<24);
				msr.hi = (zstart&0x0ff00)>>8;
				wrmsr(0x200,msr);

				msr.hi = 0x000000ff;
				msr.lo = 0xfc000800;
				wrmsr(0x201,msr);

				__asm__ volatile(
					"movl  %%cr0, %0\n\t"
					"andl  $0x9fffffff, %0\n\t"
					"movl  %0, %%cr0\n\t"
					:"=r" (cnt)
					);

				msr.lo = (zstart&0xff) << 24;
				msr.hi = (zstart&0xff00) >> 8;
				wrmsr(0xc0000100,msr);
				print_debug_char((zstart > 0x0ff)?'+':'-');


				__asm__ volatile(
					"1: \n\t"
					"movl %0, %%fs:(%1)\n\t"
					"addl $4,%1\n\t"
					"subl $1,%2\n\t"
					"jnz 1b\n\t"
					:
					: "a" (0), "D" (0), "c" (0x01000000)
					);
			}


			__asm__ volatile(
				"movl  %%cr0, %0\n\t"
				"orl  $0x40000000, %0\n\t"
				"movl  %0, %%cr0\n\t"
				:"=r" (cnt)
				);


			msr = rdmsr(0x2ff );
			msr.lo |= 0x0400;
			wrmsr(0x2ff , msr);

			msr.lo = 6;
			msr.hi = 0;
			wrmsr(0x200,msr);
			wrmsr(0x201,msr_201);

			msr.lo = 0;
			msr.hi = 0;
			wrmsr(0xc0000100,msr);

			__asm__ volatile(
				"movl  %%cr0, %0\n\t"
				"andl  $0x9fffffff, %0\n\t"
				"movl  %0, %%cr0\n\t"
				:"=r" (cnt)
				);


			msr = rdmsr(0xc0010015);
			msr.lo &= ~(1<<17);
			wrmsr(0xc0010015,msr);

			base = pci_read_config32(ctrl[i].f1, 0x40 + (ctrl[i].node_id << 3));
			base &= 0xffff0000;

			pci_write_config32(ctrl[i].f3, 0x5C , base << 8);
			pci_write_config32(ctrl[i].f3, 0x60 , base >> 24);

			pci_write_config32(ctrl[i].f3, 0x58 ,
				(22  << 16) | (22  << 8) | (22  << 0));
			print_debug("done\r\n");
		}
	}
}





typedef uint8_t u8;
typedef uint32_t u32;
typedef int8_t bool;
static void disable_probes(void)
{


	u32 val;
	print_debug("Disabling read/write/fill probes for UP... ");
	val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ 0  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x68);
	val |= (1<<10)|(1<<9)|(1<<8)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1 << 0);
	pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ 0  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x68, val);
	print_debug("done.\r\n");
}

static void wait_ap_stop(u8 node)
{
	unsigned long reg;
	unsigned long i;
	for(i=0;i< 1000 ;i++) {
		unsigned long regx;
		regx = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0x6c);
		if((regx & (1<<4))==1) break;
        }
	reg = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0x6c);
        reg &= ~(1<<4);
        pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x6c, reg);
}
static void notify_bsp_ap_is_stopped(void)
{
	unsigned long reg;
	unsigned long apic_id;
        apic_id = *((volatile unsigned long *)(0xfee00000 + 0x020 ));
	apic_id >>= 24;

        if(apic_id != 0) {

                reg = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ apic_id  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x6C);
                reg |= 1<<4;
                pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ apic_id  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x6C, reg);
        }

}

static void enable_routing(u8 node)
{
	u32 val;


	print_debug("Enabling routing table for node ");
	print_debug_hex32(node);
	val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x6c);
	val &= ~((1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0));
	pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x6c, val);

	if(node!=0) {
		wait_ap_stop(node);
	}

	print_debug(" done.\r\n");
}
static void rename_temp_node(u8 node)
{
	uint32_t val;
	print_debug("Renaming current temp node to ");
	print_debug_hex32(node);
	val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ 7  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x60);
	val &= (~7);
        val |= node;
	pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ 7  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x60, val);
	print_debug(" done.\r\n");
}
static bool check_connection(u8 src, u8 dest, u8 link)
{

	u32 val;


	val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ src  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x98+link);
	if ( (val&0x17) != 0x03)
		return 0;

        val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ dest  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0);
	if(val != 0x11001022)
		return 0;
	return 1;
}
static void optimize_connection(u8 node1, u8 link1, u8 node2, u8 link2)
{
	static const uint8_t link_width_to_pow2[]= { 3, 4, 0, 5, 1, 2, 0, 0 };
	static const uint8_t pow2_to_link_width[] = { 0x7, 4, 5, 0, 1, 3 };
	uint16_t freq_cap1, freq_cap2, freq_cap, freq_mask;
	uint8_t width_cap1, width_cap2, width_cap, width, ln_width1, ln_width2;
	uint8_t freq;


	freq_cap1  = pci_read_config16(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node1  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x80 + link1 + 0x0a );
	freq_cap2  = pci_read_config16(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node2  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x80 + link2 + 0x0a );


	freq = log2(freq_cap1 & freq_cap2 & 0xff);

	pci_write_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node1  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x80 + link1 + 0x09 , freq);
	pci_write_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node2  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x80 + link2 + 0x09 , freq);

	width_cap1 = pci_read_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node1  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,  0x80 + link1 + 6 );
	width_cap2 = pci_read_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node2  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,  0x80 + link2 + 6 );

	ln_width1 = link_width_to_pow2[width_cap1 & 7];
	ln_width2 = link_width_to_pow2[(width_cap2 >> 4) & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width = pow2_to_link_width[ln_width1];

	ln_width1 = link_width_to_pow2[(width_cap1 >> 4) & 7];
	ln_width2 = link_width_to_pow2[width_cap2 & 7];
	if (ln_width1 > ln_width2) {
		ln_width1 = ln_width2;
	}
	width |= pow2_to_link_width[ln_width1] << 4;


	pci_write_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node1  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x80 + link1 + 6  + 1, width);

	width = ((width & 0x70) >> 4) | ((width & 0x7) << 4);
	pci_write_config8(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node2  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x80 + link2 + 6  + 1, width);
}
static void fill_row(u8 node, u8 row, u32 value)
{
	pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x40+(row<<2), value);
}
static void setup_row(u8 source, u8 dest, u8 cpus)
{
	fill_row(source,dest,generate_row(source,dest,cpus));
}
static void setup_temp_row(u8 source, u8 dest, u8 cpus)
{
	fill_row(source,7,((generate_row( source,dest,cpus )&(~0x0f0000))|0x010000) );
}
static void setup_node(u8 node, u8 cpus)
{
	u8 row;
	for(row=0; row<cpus; row++)
		setup_row(node, row, cpus);
}
static void setup_remote_row(u8 source, u8 dest, u8 cpus)
{
	fill_row(7, dest, generate_row(source, dest, cpus));
}
static void setup_remote_node(u8 node, u8 cpus)
{
	static const uint8_t pci_reg[] = {
		0x44, 0x4c, 0x54, 0x5c, 0x64, 0x6c, 0x74, 0x7c,
		0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78,
		0x84, 0x8c, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbc,
		0x80, 0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8,
		0xc4, 0xcc, 0xd4, 0xdc,
		0xc0, 0xc8, 0xd0, 0xd8,
		0xe0, 0xe4, 0xe8, 0xec,
	};
	uint8_t row;
	int i;
	print_debug("setup_remote_node\r\n");
	for(row=0; row<cpus; row++)
		setup_remote_row(node, row, cpus);

	for(i = 0; i < sizeof(pci_reg)/sizeof(pci_reg[0]); i++) {
		uint32_t value;
		uint8_t reg;
		reg = pci_reg[i];
		value = pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ 0  ) & 0x1f) << 11) | ((( 1 )  & 0x7) << 8))  , reg);
		pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ 7  ) & 0x1f) << 11) | ((( 1 )  & 0x7) << 8))  , reg, value);
	}
	print_debug("setup_remote_done\r\n");
}
static u8 setup_uniprocessor(void)
{
	print_debug("Enabling UP settings\r\n");
	disable_probes();
	return 1;
}
static u8 setup_smp(void)
{
	u8 cpus=2;
	print_debug("Enabling SMP settings\r\n");
	setup_row(0,0,cpus);

	setup_temp_row(0,1,cpus);

	if (!check_connection(0, 7, 0x20  )) {
		print_debug("No connection to Node 1.\r\n");
		fill_row( 0 ,7,0x00010101 ) ;
		setup_uniprocessor();
		return 1;
	}

	optimize_connection(0, 0x20 , 7, 0x20 );
	setup_node(0, cpus);
	setup_remote_node(1, cpus);
        rename_temp_node(1);
        enable_routing(1);

	fill_row( 0 ,7,0x00010101 ) ;

	print_debug_hex32(cpus);
	print_debug(" nodes initialized.\r\n");
	return cpus;
}
static unsigned detect_mp_capabilities(unsigned cpus)
{
	unsigned node, row, mask;
	bool mp_cap= (-1) ;
	print_debug("detect_mp_capabilities: ");
	print_debug_hex32(cpus);
	print_debug("\r\n");
	if (cpus>2)
		mask=0x06;
	else
		mask=0x02;
	for (node=0; node<cpus; node++) {
		if ((pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 3 )  & 0x7) << 8))  , 0xe8) & mask)!=mask)
			mp_cap= (0) ;
	}
	if (mp_cap)
		return cpus;

	print_debug("One of the CPUs is not MP capable. Going back to UP\r\n");
	for (node=cpus; node>0; node--)
	    for (row=cpus; row>0; row--)
		fill_row(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node-1  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , row-1, 0x00010101 );

	return setup_uniprocessor();
}
static void coherent_ht_finalize(unsigned cpus)
{
	int node;
	bool rev_a0;


	print_debug("coherent_ht_finalize\r\n");
	rev_a0= is_cpu_rev_a0();
	for (node=0; node<cpus; node++) {
		u32 val;
		val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x60);
		val &= (~0x000F0070);
		val |= ((cpus-1)<<16)|((cpus-1)<<4);
		pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0x60,val);
		val=pci_read_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  , 0x68);
		val |= 0x00008000;
		pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0x68,val);
		if (rev_a0) {
			pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0x94,0);
			pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0xb4,0);
			pci_write_config32(( ((( 0 ) & 0xFF) << 16) | ((( 24+ node  ) & 0x1f) << 11) | ((( 0 )  & 0x7) << 8))  ,0xd4,0);
		}
	}
	print_debug("done\r\n");
}
static int setup_coherent_ht_domain(void)
{
	unsigned cpus;
	int reset_needed = 0;
	enable_routing(0) ;
	cpus=setup_smp();
	cpus=detect_mp_capabilities(cpus);
	coherent_ht_finalize(cpus);

	coherent_ht_mainboard(cpus);
	return reset_needed;
}
void sdram_no_memory(void)
{
	print_err("No memory!!\r\n");
	while(1) {
		hlt();
	}
}

void sdram_initialize(int controllers, const struct mem_controller *ctrl)
{
	int i;

	for(i = 0; i < controllers; i++) {
		print_debug("Ram1.");
		print_debug_hex8(i);
		print_debug("\r\n");
		sdram_set_registers(ctrl + i);
	}

	for(i = 0; i < controllers; i++) {
		print_debug("Ram2.");
		print_debug_hex8(i);
		print_debug("\r\n");
		sdram_set_spd_registers(ctrl + i);
	}

	print_debug("Ram3\r\n");
	sdram_enable(controllers, ctrl);
	print_debug("Ram4\r\n");
}
static void enable_lapic(void)
{
	msr_t msr;
	msr = rdmsr(0x1b);
	msr.hi &= 0xffffff00;
	msr.lo &= 0x000007ff;
	msr.lo |= 0xfee00000  | (1 << 11);
	wrmsr(0x1b, msr);
}
static void stop_this_cpu(void)
{
	unsigned apicid;
	apicid = apic_read(0x020 ) >> 24;

	apic_write(0x310 , (( apicid )<<24) );
	apic_write(0x300 , 0x08000  | 0x04000  | 0x00500 );

	apic_wait_icr_idle();

	apic_write(0x310 , (( apicid )<<24) );
	apic_write(0x300 ,  0x08000  | 0x00500 );

	apic_wait_icr_idle();

	for(;;) {
		hlt();
	}
}
static void pc87360_enable_serial(void)
{
	pnp_set_logical_device(0x2e , 0x03 );
	pnp_set_enable(0x2e , 1);
	pnp_set_iobase0(0x2e , 0x3f8);
}
static void main(void)
{

	static const struct mem_controller cpu[] = {
		{
			.node_id = 0,
			.f0 = ( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) ,
			.f1 = ( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  1 )  & 0x7) << 8)) ,
			.f2 = ( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  2 )  & 0x7) << 8)) ,
			.f3 = ( ((( 0 ) & 0xFF) << 16) | (((  0x18 ) & 0x1f) << 11) | (((  3 )  & 0x7) << 8)) ,
			.channel0 = { (0xa<<3)|0, (0xa<<3)|2, 0, 0 },
			.channel1 = { (0xa<<3)|1, (0xa<<3)|3, 0, 0 },
		},
		{
			.node_id = 1,
			.f0 = ( ((( 0 ) & 0xFF) << 16) | (((  0x19 ) & 0x1f) << 11) | (((  0 )  & 0x7) << 8)) ,
			.f1 = ( ((( 0 ) & 0xFF) << 16) | (((  0x19 ) & 0x1f) << 11) | (((  1 )  & 0x7) << 8)) ,
			.f2 = ( ((( 0 ) & 0xFF) << 16) | (((  0x19 ) & 0x1f) << 11) | (((  2 )  & 0x7) << 8)) ,
			.f3 = ( ((( 0 ) & 0xFF) << 16) | (((  0x19 ) & 0x1f) << 11) | (((  3 )  & 0x7) << 8)) ,
			.channel0 = { (0xa<<3)|4, (0xa<<3)|6, 0, 0 },
			.channel1 = { (0xa<<3)|5, (0xa<<3)|7, 0, 0 },
		},
	};
	if (cpu_init_detected()) {
		asm("jmp __cpu_reset");
	}
	enable_lapic();
	init_timer();
	if (!boot_cpu()) {
		stop_this_cpu();
	}
	pc87360_enable_serial();
	uart_init();
	console_init();
	setup_default_resource_map();
	setup_coherent_ht_domain();
	enumerate_ht_chain(0);
	distinguish_cpu_resets(0);

	enable_smbus();
	memreset_setup();
	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);

}
