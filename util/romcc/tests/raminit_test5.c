






typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned int uint32_t;
typedef signed int int32_t;







typedef unsigned char uint_least8_t;
typedef signed char int_least8_t;

typedef unsigned short uint_least16_t;
typedef signed short int_least16_t;

typedef unsigned int uint_least32_t;
typedef signed int int_least32_t;







typedef unsigned char uint_fast8_t;
typedef signed char int_fast8_t;

typedef unsigned int uint_fast16_t;
typedef signed int int_fast16_t;

typedef unsigned int uint_fast32_t;
typedef signed int int_fast32_t;







typedef int intptr_t;
typedef unsigned int uintptr_t;






typedef long int intmax_t;
typedef unsigned long int uintmax_t;




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

static void hlt(void)
{
        __builtin_hlt();
}

int log2(int value)
{






        return __builtin_bsr(value);
}


typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
        return __builtin_rdmsr(index);
}

static void wrmsr(unsigned long index, msr_t msr)
{
        __builtin_wrmsr(index, msr.lo, msr.hi);
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
        for(; dev <= ( (((255) & 0xFF) << 16) | (((31) & 0x1f) << 11) | (((7) & 0x7) << 8)); dev += ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((1) & 0x7) << 8))) {
                unsigned int id;
                id = pci_read_config32(dev, 0);
                if (id == pci_id) {
                        return dev;
                }
        }
        return (0xffffffffU);
}





static int uart_can_tx_byte(void)
{
        return inb(0x3f8 + 0x05) & 0x20;
}

static void uart_wait_to_tx_byte(void)
{
        while(!uart_can_tx_byte())
                ;
}

static void uart_wait_until_sent(void)
{
        while(!(inb(0x3f8 + 0x05) & 0x40))
                ;
}

static void uart_tx_byte(unsigned char data)
{
        uart_wait_to_tx_byte();
        outb(data, 0x3f8 + 0x00);

        uart_wait_until_sent();
}

static void uart_init(void)
{

        outb(0x0, 0x3f8 + 0x01);

        outb(0x01, 0x3f8 + 0x02);

        outb(0x80 | 0x3, 0x3f8 + 0x03);

        outb((115200/115200) & 0xFF, 0x3f8 + 0x00);
        outb(((115200/115200) >> 8) & 0xFF, 0x3f8 + 0x01);

        outb(0x3, 0x3f8 + 0x03);
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
        if (8 > loglevel) {
                uart_tx_byte(byte);
        }
}

static void __console_tx_hex8(int loglevel, unsigned char value)
{
        if (8 > loglevel) {
                __console_tx_nibble((value >> 4U) & 0x0fU);
                __console_tx_nibble(value & 0x0fU);
        }
}

static void __console_tx_hex16(int loglevel, unsigned short value)
{
        if (8 > loglevel) {
                __console_tx_nibble((value >> 12U) & 0x0fU);
                __console_tx_nibble((value >> 8U) & 0x0fU);
                __console_tx_nibble((value >> 4U) & 0x0fU);
                __console_tx_nibble(value & 0x0fU);
        }
}

static void __console_tx_hex32(int loglevel, unsigned int value)
{
        if (8 > loglevel) {
                __console_tx_nibble((value >> 28U) & 0x0fU);
                __console_tx_nibble((value >> 24U) & 0x0fU);
                __console_tx_nibble((value >> 20U) & 0x0fU);
                __console_tx_nibble((value >> 16U) & 0x0fU);
                __console_tx_nibble((value >> 12U) & 0x0fU);
                __console_tx_nibble((value >> 8U) & 0x0fU);
                __console_tx_nibble((value >> 4U) & 0x0fU);
                __console_tx_nibble(value & 0x0fU);
        }
}

static void __console_tx_string(int loglevel, const char *str)
{
        if (8 > loglevel) {
                unsigned char ch;
                while((ch = *str++) != '\0') {
                        __console_tx_byte(ch);
                }
        }
}

static void print_emerg_char(unsigned char byte) { __console_tx_char(0, byte); }
static void print_emerg_hex8(unsigned char value){ __console_tx_hex8(0, value); }
static void print_emerg_hex16(unsigned short value){ __console_tx_hex16(0, value); }
static void print_emerg_hex32(unsigned int value) { __console_tx_hex32(0, value); }
static void print_emerg(const char *str) { __console_tx_string(0, str); }

static void print_alert_char(unsigned char byte) { __console_tx_char(1, byte); }
static void print_alert_hex8(unsigned char value) { __console_tx_hex8(1, value); }
static void print_alert_hex16(unsigned short value){ __console_tx_hex16(1, value); }
static void print_alert_hex32(unsigned int value) { __console_tx_hex32(1, value); }
static void print_alert(const char *str) { __console_tx_string(1, str); }

static void print_crit_char(unsigned char byte) { __console_tx_char(2, byte); }
static void print_crit_hex8(unsigned char value) { __console_tx_hex8(2, value); }
static void print_crit_hex16(unsigned short value){ __console_tx_hex16(2, value); }
static void print_crit_hex32(unsigned int value) { __console_tx_hex32(2, value); }
static void print_crit(const char *str) { __console_tx_string(2, str); }

static void print_err_char(unsigned char byte) { __console_tx_char(3, byte); }
static void print_err_hex8(unsigned char value) { __console_tx_hex8(3, value); }
static void print_err_hex16(unsigned short value){ __console_tx_hex16(3, value); }
static void print_err_hex32(unsigned int value) { __console_tx_hex32(3, value); }
static void print_err(const char *str) { __console_tx_string(3, str); }

static void print_warning_char(unsigned char byte) { __console_tx_char(4, byte); }
static void print_warning_hex8(unsigned char value) { __console_tx_hex8(4, value); }
static void print_warning_hex16(unsigned short value){ __console_tx_hex16(4, value); }
static void print_warning_hex32(unsigned int value) { __console_tx_hex32(4, value); }
static void print_warning(const char *str) { __console_tx_string(4, str); }

static void print_notice_char(unsigned char byte) { __console_tx_char(5, byte); }
static void print_notice_hex8(unsigned char value) { __console_tx_hex8(5, value); }
static void print_notice_hex16(unsigned short value){ __console_tx_hex16(5, value); }
static void print_notice_hex32(unsigned int value) { __console_tx_hex32(5, value); }
static void print_notice(const char *str) { __console_tx_string(5, str); }

static void print_info_char(unsigned char byte) { __console_tx_char(6, byte); }
static void print_info_hex8(unsigned char value) { __console_tx_hex8(6, value); }
static void print_info_hex16(unsigned short value){ __console_tx_hex16(6, value); }
static void print_info_hex32(unsigned int value) { __console_tx_hex32(6, value); }
static void print_info(const char *str) { __console_tx_string(6, str); }

static void print_debug_char(unsigned char byte) { __console_tx_char(7, byte); }
static void print_debug_hex8(unsigned char value) { __console_tx_hex8(7, value); }
static void print_debug_hex16(unsigned short value){ __console_tx_hex16(7, value); }
static void print_debug_hex32(unsigned int value) { __console_tx_hex32(7, value); }
static void print_debug(const char *str) { __console_tx_string(7, str); }

static void print_spew_char(unsigned char byte) { __console_tx_char(8, byte); }
static void print_spew_hex8(unsigned char value) { __console_tx_hex8(8, value); }
static void print_spew_hex16(unsigned short value){ __console_tx_hex16(8, value); }
static void print_spew_hex32(unsigned int value) { __console_tx_hex32(8, value); }
static void print_spew(const char *str) { __console_tx_string(8, str); }

static void console_init(void)
{
        static const char console_test[] =
                "\r\n\r\nLinuxBIOS-"
                "1.1.0"
                ".0Fallback"
                " "
                "Thu Jun 19 05:42:16 MDT 2003"
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
        print_debug("Done.\n");
}


static void enumerate_ht_chain(void)
{





        unsigned next_unitid, last_unitid;;
        next_unitid = 1;
        do {
                uint32_t id;
                uint8_t hdr_type, pos;
                last_unitid = next_unitid;

                id = pci_read_config32(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), 0x00);

                if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0x0000)) {
                        break;
                }
                hdr_type = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), 0x0e);
                pos = 0;
                hdr_type &= 0x7f;

                if ((hdr_type == 0) ||
                        (hdr_type == 1)) {
                        pos = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), 0x34);
                }
                while(pos != 0) {
                        uint8_t cap;
                        cap = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), pos + 0);
                        if (cap == 0x08) {
                                uint16_t flags;
                                flags = pci_read_config16(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), pos + 2);
                                if ((flags >> 13) == 0) {
                                        unsigned count;
                                        flags &= ~0x1f;
                                        flags |= next_unitid & 0x1f;
                                        count = (flags >> 5) & 0x1f;
                                        pci_write_config16(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), pos + 2, flags);
                                        next_unitid += count;
                                        break;
                                }
                        }
                        pos = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8)), pos + 1);
                }
        } while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
}



static void enable_smbus(void)
{
        device_t dev;
        dev = pci_locate_device(((((0x746b) & 0xFFFF) << 16) | ((0x1022) & 0xFFFF)), 0);
        if (dev == (0xffffffffU)) {
                die("SMBUS controller not found\r\n");
        }
        uint8_t enable;
        print_debug("SMBus controller enabled\r\n");
        pci_write_config32(dev, 0x58, 0x1000 | 1);
        enable = pci_read_config8(dev, 0x41);
        pci_write_config8(dev, 0x41, enable | (1 << 7));
}


static inline void smbus_delay(void)
{
        outb(0x80, 0x80);
}

static int smbus_wait_until_ready(void)
{
        unsigned long loops;
        loops = (100*1000*10);
        do {
                unsigned short val;
                smbus_delay();
                val = inw(0x1000 + 0xe0);
                if ((val & 0x800) == 0) {
                        break;
                }
        } while(--loops);
        return loops?0:-1;
}

static int smbus_wait_until_done(void)
{
        unsigned long loops;
        loops = (100*1000*10);
        do {
                unsigned short val;
                smbus_delay();

                val = inw(0x1000 + 0xe0);
                if (((val & 0x8) == 0) | ((val & 0x437) != 0)) {
                        break;
                }
        } while(--loops);
        return loops?0:-1;
}

static int smbus_read_byte(unsigned device, unsigned address)
{
        unsigned char global_control_register;
        unsigned char global_status_register;
        unsigned char byte;

        if (smbus_wait_until_ready() < 0) {
                return -1;
        }



        outw(inw(0x1000 + 0xe2) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), 0x1000 + 0xe2);

        outw(((device & 0x7f) << 1) | 1, 0x1000 + 0xe4);

        outb(address & 0xFF, 0x1000 + 0xe8);

        outw((inw(0x1000 + 0xe2) & ~7) | (0x2), 0x1000 + 0xe2);



        outw(inw(0x1000 + 0xe0), 0x1000 + 0xe0);


        outw(0, 0x1000 + 0xe6);


        outw((inw(0x1000 + 0xe2) | (1 << 3)), 0x1000 + 0xe2);



        if (smbus_wait_until_done() < 0) {
                return -1;
        }

        global_status_register = inw(0x1000 + 0xe0);


        byte = inw(0x1000 + 0xe6) & 0xff;

        if (global_status_register != (1 << 4)) {
                return -1;
        }
        return byte;
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

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x44) & 0xFF)), 0x0000f8f8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x0000f8f8, 0x00000001,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x54) & 0xFF)), 0x0000f8f8, 0x00000002,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000f8f8, 0x00000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x64) & 0xFF)), 0x0000f8f8, 0x00000004,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x6C) & 0xFF)), 0x0000f8f8, 0x00000005,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x74) & 0xFF)), 0x0000f8f8, 0x00000006,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x7C) & 0xFF)), 0x0000f8f8, 0x00000007,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x40) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x48) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x50) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x58) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x60) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x68) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x70) & 0xFF)), 0x0000f8fc, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x78) & 0xFF)), 0x0000f8fc, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x8C) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x94) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x9C) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA4) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xAC) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB4) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xBC) & 0xFF)), 0x00000048, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x80) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x88) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x90) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x98) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA0) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA8) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB0) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB8) & 0xFF)), 0x000000f0, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC4) & 0xFF)), 0xFE000FC8, 0x01fff000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xCC) & 0xFF)), 0xFE000FC8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD4) & 0xFF)), 0xFE000FC8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xDC) & 0xFF)), 0xFE000FC8, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC0) & 0xFF)), 0xFE000FCC, 0x00000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC8) & 0xFF)), 0xFE000FCC, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD0) & 0xFF)), 0xFE000FCC, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD8) & 0xFF)), 0xFE000FCC, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE0) & 0xFF)), 0x0000FC88, 0xff000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE4) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE8) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xEC) & 0xFF)), 0x0000FC88, 0x00000000,
        };
        int max;
        max = sizeof(register_values)/sizeof(register_values[0]);
        setup_resource_map(register_values, max);
}

static void sdram_set_registers(void)
{
        static const unsigned int register_values[] = {

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x44) & 0xFF)), 0x0000f8f8, 0x003f0000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x0000f8f8, 0x00000001,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x54) & 0xFF)), 0x0000f8f8, 0x00000002,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000f8f8, 0x00000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x64) & 0xFF)), 0x0000f8f8, 0x00000004,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x6C) & 0xFF)), 0x0000f8f8, 0x00000005,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x74) & 0xFF)), 0x0000f8f8, 0x00000006,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x7C) & 0xFF)), 0x0000f8f8, 0x00000007,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x40) & 0xFF)), 0x0000f8fc, 0x00000003,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x48) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x50) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x58) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x60) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x68) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x70) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x78) & 0xFF)), 0x0000f8fc, 0x00400000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00000048, 0x00e1ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x8C) & 0xFF)), 0x00000048, 0x00dfff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x94) & 0xFF)), 0x00000048, 0x00e3ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x9C) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA4) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xAC) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB4) & 0xFF)), 0x00000048, 0x00000b00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xBC) & 0xFF)), 0x00000048, 0x00fe0b00,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x80) & 0xFF)), 0x000000f0, 0x00e00003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x88) & 0xFF)), 0x000000f0, 0x00d80003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x90) & 0xFF)), 0x000000f0, 0x00e20003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x98) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA0) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA8) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB0) & 0xFF)), 0x000000f0, 0x00000a03,


        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB8) & 0xFF)), 0x000000f0, 0x00400003,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC4) & 0xFF)), 0xFE000FC8, 0x0000d000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xCC) & 0xFF)), 0xFE000FC8, 0x000ff000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD4) & 0xFF)), 0xFE000FC8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xDC) & 0xFF)), 0xFE000FC8, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC0) & 0xFF)), 0xFE000FCC, 0x0000d003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC8) & 0xFF)), 0xFE000FCC, 0x00001013,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD0) & 0xFF)), 0xFE000FCC, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD8) & 0xFF)), 0xFE000FCC, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE0) & 0xFF)), 0x0000FC88, 0xff000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE4) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE8) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xEC) & 0xFF)), 0x0000FC88, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x40) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x44) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x48) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x001f01fe, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x50) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x54) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x58) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x001f01fe, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x60) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x64) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x68) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x6C) & 0xFF)), 0xC01f01ff, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x70) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x74) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x78) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x7C) & 0xFF)), 0xC01f01ff, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x80) & 0xFF)), 0xffff8888, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x88) & 0xFF)), 0xe8088008, 0x03623125,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x8c) & 0xFF)), 0xff8fe08e, 0x00000930,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)), 0xf0000000,
        (4 << 25)|(0 << 24)|
        (0 << 23)|(0 << 22)|(0 << 21)|(0 << 20)|
        (1 << 19)|(1 << 18)|(0 << 17)|(0 << 16)|
        (2 << 14)|(0 << 13)|(0 << 12)|
        (0 << 11)|(0 << 10)|(0 << 9)|(0 << 8)|
        (0 << 3) |(0 << 1) |(0 << 0),

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x94) & 0xFF)), 0xc180f0f0, 0x0e2b0a05,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x98) & 0xFF)), 0xfc00ffff, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((3) & 0x07) << 8) | ((0x58) & 0xFF)), 0xffe0e0e0, 0x00000000,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((3) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000003e, 0x00000000,





        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((3) & 0x07) << 8) | ((0x60) & 0xFF)), 0xffffff00, 0x00000000,
        };
        int i;
        int max;
        print_debug("setting up CPU0 northbridge registers\r\n");
        max = sizeof(register_values)/sizeof(register_values[0]);
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





        value = smbus_read_byte(device, 3);
        if (value < 0) return sz;
        sz.side1 += value & 0xf;

        value = smbus_read_byte(device, 4);
        if (value < 0) return sz;
        sz.side1 += value & 0xf;

        value = smbus_read_byte(device, 17);
        if (value < 0) return sz;
        sz.side1 += log2(value & 0xff);


        value = smbus_read_byte(device, 7);
        if (value < 0) return sz;
        value &= 0xff;
        value <<= 8;

        low = smbus_read_byte(device, 6);
        if (low < 0) return sz;
        value = value | (low & 0xff);
        sz.side1 += log2(value);


        value = smbus_read_byte(device, 5);
        if (value <= 1) return sz;


        sz.side2 = sz.side1;

        value = smbus_read_byte(device, 3);
        if (value < 0) return sz;
        if ((value & 0xf0) == 0) return sz;
        sz.side2 -= (value & 0x0f);
        sz.side2 += ((value >> 4) & 0x0f);

        value = smbus_read_byte(device, 4);
        if (value < 0) return sz;
        sz.side2 -= (value & 0x0f);
        sz.side2 += ((value >> 4) & 0x0f);
        return sz;
}

static unsigned spd_to_dimm(unsigned device)
{
        return (device - (0xa << 3));
}

static void set_dimm_size(struct dimm_size sz, unsigned index)
{
        uint32_t base0, base1, map;


        print_debug("set_dimm_size: (");
        print_debug_hex32(sz.side1);
        print_debug_char(',');
        print_debug_hex32(sz.side2);
        print_debug_char(',');
        print_debug_hex32(index);
        print_debug(")\r\n");

        if (sz.side1 != sz.side2) {
                sz.side2 = 0;
        }
        map = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x80);
        map &= ~(0xf << (index + 4));







        base0 = base1 = 0;


        if (sz.side1 >= (25 + 3)) {
                base0 = (1 << ((sz.side1 - (25 + 3)) + 21)) | 1;
                map |= (sz.side1 - (25 + 3)) << (index *4);
        }


        if (sz.side2 >= (25 + 3)) {
                base1 = (1 << ((sz.side2 - (25 + 3)) + 21)) | 1;
        }


        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x40 + (((index << 1)+0)<<2), base0);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x40 + (((index << 1)+1)<<2), base1);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x80, map);
}

static void spd_set_ram_size(void)
{
        unsigned device;
        for(device = (0xa << 3);
                device <= ((0xa << 3) +1);
                device += 1)
        {
                struct dimm_size sz;
                sz = spd_get_dimm_size(device);
                set_dimm_size(sz, spd_to_dimm(device));
        }
}

static void set_top_mem(unsigned tom_k)
{

        if (!tom_k) {
                die("No memory");
        }

        msr_t msr;
        msr.lo = (tom_k & 0x003fffff) << 10;
        msr.hi = (tom_k & 0xffc00000) >> 22;
        wrmsr(0xC001001A, msr);







}

static void order_dimms(void)
{
        unsigned long tom;
        unsigned mask;
        unsigned index;


        tom = 0;
        for(;;) {

                unsigned canidate;
                uint32_t csbase, csmask;
                unsigned size;
                csbase = 0;
                canidate = 0;
                for(index = 0; index < 8; index++) {
                        uint32_t value;
                        value = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x40 + (index << 2));


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


                tom |= (1 << (canidate + 24));


                size = csbase >> 21;


                csbase = (tom << 21) | 1;


                tom += size;


                csmask = ((size -1) << 21);
                csmask |= 0xfe00;


                pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x40 + (canidate << 2), csbase);

                pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x60 + (canidate << 2), csmask);

        }
        set_top_mem((tom & ~0xff000000) << 15);
}

static void spd_set_dram_timing(void)
{

}

static void spd_set_ecc_mode(void)
{
        unsigned long dcl;
        dcl = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x90);

        dcl &= ~(1<<17);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x90, dcl);

}
static void sdram_set_spd_registers(void)
{
        spd_set_ram_size();
        spd_set_dram_timing();
        spd_set_ecc_mode();
        order_dimms();
}


static void sdram_enable(void)
{
        unsigned long dcl;


        dcl = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x90);
        print_debug("dcl: ");
        print_debug_hex32(dcl);
        print_debug("\r\n");
        dcl |= (1<<3);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x90, dcl);
        dcl &= ~(1<<3);
        dcl &= ~(1<<0);
        dcl &= ~(1<<1);
        dcl &= ~(1<<2);
        dcl |= (1<<8);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x90, dcl);

        print_debug("Initializing memory: ");
        int loops = 0;
        do {
                dcl = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)), 0x90);
                loops += 1;
                if ((loops & 1023) == 0) {
                        print_debug(".");
                }
        } while(((dcl & (1<<8)) != 0) && (loops < 300000));
        if (loops >= 300000) {
                print_debug(" failed\r\n");
        } else {
                print_debug(" done\r\n");
        }

}

static void sdram_first_normal_reference(void) {}
static void sdram_enable_refresh(void) {}
static void sdram_special_finishup(void) {}


static void setup_coherent_ht_domain(void)
{
        static const unsigned int register_values[] = {

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x40) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x44) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x48) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x4c) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x50) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x54) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x58) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x5c) & 0xFF)), 0xfff0f0f0, 0x00010101,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x68) & 0xFF)), 0x00800000, 0x0f00840f,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x6C) & 0xFF)), 0xffffff8c, 0x00000000 | (1 << 6) |(1 << 5)| (1 << 4),

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00009c05, 0x11110020,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x88) & 0xFF)), 0xfffff0ff, 0x00000200,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x94) & 0xFF)), 0xff000000, 0x00ff0000,





        };
        int i;
        int max;
        print_debug("setting up coherent ht domain....\r\n");
        max = sizeof(register_values)/sizeof(register_values[0]);
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


void sdram_no_memory(void)
{
        print_err("No memory!!\r\n");
        while(1) {
                hlt();
        }
}


void sdram_initialize(void)
{
        print_debug("Ram1\r\n");

        sdram_set_registers();

        print_debug("Ram2\r\n");

        sdram_set_spd_registers();

        print_debug("Ram3\r\n");




        sdram_enable();

        print_debug("Ram4\r\n");
        sdram_first_normal_reference();

        print_debug("Ram5\r\n");
        sdram_enable_refresh();
        sdram_special_finishup();

        print_debug("Ram6\r\n");
}


static int boot_cpu(void)
{
        volatile unsigned long *local_apic;
        unsigned long apic_id;
        int bsp;
        msr_t msr;
        msr = rdmsr(0x1b);
        bsp = !!(msr.lo & (1 << 8));
        if (bsp) {
                print_debug("Bootstrap processor\r\n");
        } else {
                print_debug("Application processor\r\n");
        }

        return bsp;
}

static int cpu_init_detected(void)
{
        unsigned long dcl;
        int cpu_init;

        unsigned long htic;

        htic = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x7) << 8)), 0x6c);

        cpu_init = (htic & (1<<6));
        if (cpu_init) {
                print_debug("CPU INIT Detected.\r\n");
        }
        return cpu_init;
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
        for(dev = ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x7) << 8));
                dev <= ( (((0) & 0xFF) << 16) | (((0x1f) & 0x1f) << 11) | (((0x7) & 0x7) << 8));
                dev += ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((1) & 0x7) << 8))) {
                uint32_t id;
                id = pci_read_config32(dev, 0x00);
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

static void dump_spd_registers(void)
{
        unsigned device;
        device = (0xa << 3);
        print_debug("\r\n");
        while(device <= ((0xa << 3) +1)) {
                int i;
                print_debug("dimm: ");
                print_debug_hex8(device);
                for(i = 0; i < 256; i++) {
                        int status;
                        unsigned char byte;
                        if ((i & 0xf) == 0) {
                                print_debug("\r\n");
                                print_debug_hex8(i);
                                print_debug(": ");
                        }
                        status = smbus_read_byte(device, i);
                        if (status < 0) {
                                print_debug("bad device\r\n");
                                break;
                        }
                        byte = status & 0xff;
                        print_debug_hex8(byte);
                        print_debug_char(' ');
                }
                device += 1;
                print_debug("\r\n");
        }
}


static void main(void)
{
        uart_init();
        console_init();







        if (boot_cpu() && !cpu_init_detected()) {
                setup_default_resource_map();
                setup_coherent_ht_domain();
                enumerate_ht_chain();
                print_pci_devices();
                enable_smbus();
                sdram_initialize();

                dump_spd_registers();
                dump_pci_device(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x7) << 8)));


                msr_t msr;
                msr = rdmsr(0xC001001A);
                print_debug("TOP_MEM: ");
                print_debug_hex32(msr.hi);
                print_debug_hex32(msr.lo);
                print_debug("\r\n");
                ram_check(0x00000000, msr.lo);
        }
}
