# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"

# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/include/stdint.h" 1
# 11 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/include/stdint.h"
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
# 3 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/include/device/pci_def.h" 1
# 4 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/include/arch/romcc_io.h" 1
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

typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
        return __builtin_rdmsr(index);
}

static void wrmsr(unsigned long index, msr_t msr)
{
        __builtin_wrmsr(index, msr.lo, msr.hi);
}







static unsigned char pci_read_config8(unsigned addr)
{
        outl(0x80000000 | (addr & ~3), 0xCF8);
        return inb(0xCFC + (addr & 3));
}

static unsigned short pci_read_config16(unsigned addr)
{
        outl(0x80000000 | (addr & ~3), 0xCF8);
        return inw(0xCFC + (addr & 2));
}

static unsigned int pci_read_config32(unsigned addr)
{
        outl(0x80000000 | (addr & ~3), 0xCF8);
        return inl(0xCFC);
}

static void pci_write_config8(unsigned addr, unsigned char value)
{
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outb(value, 0xCFC + (addr & 3));
}

static void pci_write_config16(unsigned addr, unsigned short value)
{
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outw(value, 0xCFC + (addr & 2));
}

static void pci_write_config32(unsigned addr, unsigned int value)
{
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outl(value, 0xCFC);
}
# 5 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/pc80/serial.c" 1
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/include/part/fallback_boot.h" 1
# 2 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/pc80/serial.c" 2
# 44 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/pc80/serial.c"
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
# 89 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/pc80/serial.c"
                outb((115200/9600) & 0xFF, 0x3f8 + 0x00);
                outb(((115200/9600) >> 8) & 0xFF, 0x3f8 + 0x01);

        outb(0x3, 0x3f8 + 0x03);
}
# 6 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/lib/console.c" 1
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/include/console/loglevel.h" 1
# 2 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/lib/console.c" 2

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
# 128 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/arch/i386/lib/console.c"
static void console_init(void)
{
        static const char console_test[] =
                "\r\n\r\nLinuxBIOS-"
                "1.1.0"
                ".0Fallback"
                " "
                "Mon Jun 9 18:15:20 MDT 2003"
                " starting...\r\n";
        print_info(console_test);
}
# 7 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2
# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/ram/ramtest.c" 1
static void write_phys(unsigned long addr, unsigned long value)
{
        volatile unsigned long *ptr;
        ptr = (void *)addr;
        *ptr = value;
}

static unsigned long read_phys(unsigned long addr)
{
        volatile unsigned long *ptr;
        ptr = (void *)addr;
        return *ptr;
}

void ram_fill(unsigned long start, unsigned long stop)
{
        unsigned long addr;



        print_debug("DRAM fill: ");
        print_debug_hex32(start);
        print_debug("-");
        print_debug_hex32(stop);
        print_debug("\r\n");
        for(addr = start; addr < stop ; addr += 4) {

                if ((addr & 0xffff) == 0) {
                        print_debug_hex32(addr);
                        print_debug("\r");
                }
                write_phys(addr, addr);
        };

        print_debug_hex32(addr);
        print_debug("\r\nDRAM filled\r\n");
}

void ram_verify(unsigned long start, unsigned long stop)
{
        unsigned long addr;



        print_debug("DRAM verify: ");
        print_debug_hex32(start);
        print_debug_char('-');
        print_debug_hex32(stop);
        print_debug("\r\n");
        for(addr = start; addr < stop ; addr += 4) {
                unsigned long value;

                if ((addr & 0xffff) == 0) {
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


void ramcheck(unsigned long start, unsigned long stop)
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
# 8 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2

static void die(const char *str)
{
        print_emerg(str);
        do {
                hlt();
        } while(1);
}




static void sdram_set_registers(void)
{
        static const unsigned int register_values[] = {
# 51 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x44) & 0xFF)), 0x0000f8f8, 0x003f0000,




        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x0000f8f8, 0x00000001,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x54) & 0xFF)), 0x0000f8f8, 0x00000002,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000f8f8, 0x00000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x64) & 0xFF)), 0x0000f8f8, 0x00000004,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x6C) & 0xFF)), 0x0000f8f8, 0x00000005,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x74) & 0xFF)), 0x0000f8f8, 0x00000006,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x7C) & 0xFF)), 0x0000f8f8, 0x00000007,
# 93 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x40) & 0xFF)), 0x0000f8fc, 0x00000003,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x48) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x50) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x58) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x60) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x68) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x70) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x78) & 0xFF)), 0x0000f8fc, 0x00400000,
# 145 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00000048, 0x00e1ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x8C) & 0xFF)), 0x00000048, 0x00dfff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x94) & 0xFF)), 0x00000048, 0x00e3ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x9C) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA4) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xAC) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB4) & 0xFF)), 0x00000048, 0x00000b00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xBC) & 0xFF)), 0x00000048, 0x00fe0b00,
# 180 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x80) & 0xFF)), 0x000000f0, 0x00e00003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x88) & 0xFF)), 0x000000f0, 0x00d80003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x90) & 0xFF)), 0x000000f0, 0x00e20003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x98) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA0) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA8) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB0) & 0xFF)), 0x000000f0, 0x00000a03,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB8) & 0xFF)), 0x000000f0, 0x00400003,
# 219 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC4) & 0xFF)), 0xFE000FC8, 0x0000d000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xCC) & 0xFF)), 0xFE000FC8, 0x000ff000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD4) & 0xFF)), 0xFE000FC8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xDC) & 0xFF)), 0xFE000FC8, 0x00000000,
# 249 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC0) & 0xFF)), 0xFE000FCC, 0x0000d003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC8) & 0xFF)), 0xFE000FCC, 0x00001013,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD0) & 0xFF)), 0xFE000FCC, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD8) & 0xFF)), 0xFE000FCC, 0x00000000,
# 290 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE0) & 0xFF)), 0x0000FC88, 0xff000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE4) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE8) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xEC) & 0xFF)), 0x0000FC88, 0x00000000,
# 316 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x40) & 0xFF)), 0x001f01fe, 0x00000001,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x44) & 0xFF)), 0x001f01fe, 0x01000001,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x48) & 0xFF)), 0x001f01fe, 0x02000001,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x001f01fe, 0x03000001,






        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x50) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x54) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x58) & 0xFF)), 0x001f01fe, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x001f01fe, 0x00000000,
# 351 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x60) & 0xFF)), 0xC01f01ff, 0x00e0fe00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x64) & 0xFF)), 0xC01f01ff, 0x00e0fe00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x68) & 0xFF)), 0xC01f01ff, 0x00e0fe00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x6C) & 0xFF)), 0xC01f01ff, 0x00e0fe00,







        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x70) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x74) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x78) & 0xFF)), 0xC01f01ff, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x7C) & 0xFF)), 0xC01f01ff, 0x00000000,
# 387 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x80) & 0xFF)), 0xffff8888, 0x00000033,
# 456 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x88) & 0xFF)), 0xe8088008, 0x03623125,
# 487 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x8c) & 0xFF)), 0xff8fe08e, 0x00000930,
# 563 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)), 0xf0000000,
        (4 << 25)|(0 << 24)|
        (0 << 23)|(0 << 22)|(0 << 21)|(0 << 20)|
        (1 << 19)|(1 << 18)|(0 << 17)|(0 << 16)|
        (2 << 14)|(0 << 13)|(0 << 12)|
        (0 << 11)|(0 << 10)|(0 << 9)|(0 << 8)|
        (0 << 3) |(0 << 1) |(0 << 0),
# 635 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x94) & 0xFF)), 0xc180f0f0, 0x0e2b0a05,
# 655 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x98) & 0xFF)), 0xfc00ffff, 0x00000000,
# 689 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((3) & 0x07) << 8) | ((0x58) & 0xFF)), 0xffe0e0e0, 0x00000000,
# 698 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((3) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000003e, 0x00000000,





        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((3) & 0x07) << 8) | ((0x60) & 0xFF)), 0xffffff00, 0x00000000,
        };
        int i;
        int max;
        print_debug("setting up CPU0 northbridge registers\r\n");
        max = sizeof(register_values)/sizeof(register_values[0]);
        for(i = 0; i < max; i += 3) {
                unsigned long reg;






                reg = pci_read_config32(register_values[i]);
                reg &= register_values[i+1];
                reg |= register_values[i+2];
                pci_write_config32(register_values[i], reg);
        }
        print_debug("done.\r\n");
}
# 743 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
static void sdram_set_spd_registers(void)
{
        unsigned long dcl;
        dcl = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)));

        dcl &= ~(1<<17);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)), dcl);
}


static void sdram_enable(void)
{
        unsigned long dcl;


        dcl = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)));
        print_debug("dcl: ");
        print_debug_hex32(dcl);
        print_debug("\r\n");
        dcl |= (1<<3);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)), dcl);
        dcl &= ~(1<<3);
        dcl &= ~(1<<0);
        dcl &= ~(1<<1);
        dcl &= ~(1<<2);
        dcl |= (1<<8);
        pci_write_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)), dcl);

        print_debug("Initializing memory: ");
        int loops = 0;
        do {
                dcl = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((2) & 0x07) << 8) | ((0x90) & 0xFF)));
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
# 803 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
}

static void sdram_first_normal_reference(void) {}
static void sdram_enable_refresh(void) {}
static void sdram_special_finishup(void) {}

# 1 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/sdram/generic_sdram.c" 1
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
# 810 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c" 2

static int boot_cpu(void)
{
        volatile unsigned long *local_apic;
        unsigned long apic_id;
        int bsp;
        msr_t msr;
        msr = rdmsr(0x1b);
        bsp = !!(msr.lo & (1 << 8));
        if (bsp) {
                print_debug("Bootstrap cpu\r\n");
        }

        return bsp;
}

static int cpu_init_detected(void)
{
        unsigned long dcl;
        int cpu_init;

        unsigned long htic;

        htic = pci_read_config32(( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x6c) & 0xFF)));
# 849 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        cpu_init = (htic & (1<<6));
        if (cpu_init) {
                print_debug("CPU INIT Detected.\r\n");
        }
        return cpu_init;
}

static void setup_coherent_ht_domain(void)
{
        static const unsigned int register_values[] = {
# 884 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x40) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x44) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x48) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x4c) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x50) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x54) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x58) & 0xFF)), 0xfff0f0f0, 0x00010101,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x5c) & 0xFF)), 0xfff0f0f0, 0x00010101,
# 983 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x68) & 0xFF)), 0x00800000, 0x0f00840f,
# 1005 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x6C) & 0xFF)), 0xffffff8c, 0x00000000 | (1 << 6) |(1 << 5)| (1 << 4),
# 1082 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00009c05, 0x11110020,
# 1127 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x88) & 0xFF)), 0xfffff0ff, 0x00000200,
# 1148 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x94) & 0xFF)), 0xff000000, 0x00ff0000,
# 1182 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x44) & 0xFF)), 0x0000f8f8, 0x003f0000,




        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x4C) & 0xFF)), 0x0000f8f8, 0x00000001,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x54) & 0xFF)), 0x0000f8f8, 0x00000002,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x5C) & 0xFF)), 0x0000f8f8, 0x00000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x64) & 0xFF)), 0x0000f8f8, 0x00000004,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x6C) & 0xFF)), 0x0000f8f8, 0x00000005,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x74) & 0xFF)), 0x0000f8f8, 0x00000006,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x7C) & 0xFF)), 0x0000f8f8, 0x00000007,
# 1224 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x40) & 0xFF)), 0x0000f8fc, 0x00000003,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x48) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x50) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x58) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x60) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x68) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x70) & 0xFF)), 0x0000f8fc, 0x00400000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x78) & 0xFF)), 0x0000f8fc, 0x00400000,
# 1276 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x84) & 0xFF)), 0x00000048, 0x00e1ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x8C) & 0xFF)), 0x00000048, 0x00dfff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x94) & 0xFF)), 0x00000048, 0x00e3ff00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x9C) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA4) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xAC) & 0xFF)), 0x00000048, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB4) & 0xFF)), 0x00000048, 0x00000b00,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xBC) & 0xFF)), 0x00000048, 0x00fe0b00,
# 1311 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x80) & 0xFF)), 0x000000f0, 0x00e00003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x88) & 0xFF)), 0x000000f0, 0x00d80003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x90) & 0xFF)), 0x000000f0, 0x00e20003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0x98) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA0) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xA8) & 0xFF)), 0x000000f0, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB0) & 0xFF)), 0x000000f0, 0x00000a03,

        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xB8) & 0xFF)), 0x000000f0, 0x00400003,
# 1350 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC4) & 0xFF)), 0xFE000FC8, 0x0000d000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xCC) & 0xFF)), 0xFE000FC8, 0x000ff000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD4) & 0xFF)), 0xFE000FC8, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xDC) & 0xFF)), 0xFE000FC8, 0x00000000,
# 1380 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC0) & 0xFF)), 0xFE000FCC, 0x0000d003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xC8) & 0xFF)), 0xFE000FCC, 0x00001013,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD0) & 0xFF)), 0xFE000FCC, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xD8) & 0xFF)), 0xFE000FCC, 0x00000000,
# 1421 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE0) & 0xFF)), 0x0000FC88, 0xff000003,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE4) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xE8) & 0xFF)), 0x0000FC88, 0x00000000,
        ( (((0) & 0xFF) << 16) | (((0x18) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0xEC) & 0xFF)), 0x0000FC88, 0x00000000,

        };
        int i;
        int max;
        print_debug("setting up coherent ht domain....\r\n");
        max = sizeof(register_values)/sizeof(register_values[0]);
        for(i = 0; i < max; i += 3) {
                unsigned long reg;






                reg = pci_read_config32(register_values[i]);
                reg &= register_values[i+1];
                reg |= register_values[i+2] & ~register_values[i+1];
                pci_write_config32(register_values[i], reg);
        }
        print_debug("done.\r\n");
}

static void enumerate_ht_chain(void)
{
        unsigned next_unitid, last_unitid;;
        next_unitid = 1;
        do {
                uint32_t id;
                uint8_t hdr_type, pos;
                last_unitid = next_unitid;

                id = pci_read_config32(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x00) & 0xFF)));

                if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0x0000)) {
                        break;
                }
                hdr_type = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x0e) & 0xFF)));
                pos = 0;
                hdr_type &= 0x7f;

                if ((hdr_type == 0) ||
                        (hdr_type == 1)) {
                        pos = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0x34) & 0xFF)));
                }
                while(pos != 0) {
                        uint8_t cap;
                        cap = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((pos + 0) & 0xFF)));
                        if (cap == 0x08) {
                                uint16_t flags;
                                flags = pci_read_config16(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((pos + 2) & 0xFF)));
                                if ((flags >> 13) == 0) {
                                        unsigned count;
                                        flags &= ~0x1f;
                                        flags |= next_unitid & 0x1f;
                                        count = (flags >> 5) & 0x1f;
                                        pci_write_config16(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((pos + 2) & 0xFF)), flags);
                                        next_unitid += count;
                                        break;
                                }
                        }
                        pos = pci_read_config8(( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((pos + 1) & 0xFF)));
                }
        } while((last_unitid != next_unitid) && (next_unitid <= 0x1f));
}

static void print_pci_devices(void)
{
        uint32_t addr;
        for(addr = ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0) & 0xFF));
                addr <= ( (((0) & 0xFF) << 16) | (((0x1f) & 0x1f) << 11) | (((0x7) & 0x07) << 8) | ((0) & 0xFF));
                addr += ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0) & 0xFF))) {
                uint32_t id;
                id = pci_read_config32(addr + 0x00);
                if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0x0000)) {
                        continue;
                }
                print_debug("PCI: 00:");
                print_debug_hex8(addr >> 11);
                print_debug_char('.');
                print_debug_hex8((addr >> 8) & 7);
                print_debug("\r\n");
        }
}
# 1525 "/home/eric/projects/linuxbios/checkin/solo/freebios2/src/mainboard/amd/solo/auto.c"
static void enable_smbus(void)
{
        uint32_t addr;
        for(addr = ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((0) & 0x07) << 8) | ((0) & 0xFF));
                addr <= ( (((0) & 0xFF) << 16) | (((0x1f) & 0x1f) << 11) | (((0x7) & 0x07) << 8) | ((0) & 0xFF));
                addr += ( (((0) & 0xFF) << 16) | (((0) & 0x1f) << 11) | (((1) & 0x07) << 8) | ((0) & 0xFF))) {
                uint32_t id;
                id = pci_read_config32(addr);
                if (id == ((0x746b << 16) | (0x1022))) {
                        break;
                }
        }
        if (addr > ( (((0) & 0xFF) << 16) | (((0x1f) & 0x1f) << 11) | (((0x7) & 0x07) << 8) | ((0) & 0xFF))) {
                die("SMBUS controller not found\r\n");
        }
        uint8_t enable;
        print_debug("SMBus controller enabled\r\n");
        pci_write_config32(addr + 0x58, 0x1000 | 1);
        enable = pci_read_config8(addr + 0x41);
        pci_write_config8(addr + 0x41, enable | (1 << 7));
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
                if (((val & 0x8) == 0) || ((val & 0x437) != 0)) {
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
                                continue;
                        }
                        byte = status & 0xff;
                        print_debug_hex8(byte);
                        print_debug_char(' ');
                }
                device += 1;
                print_debug("\r\n");
        }
}

static void dump_spd_registers1(void)
{
        int i;
        print_debug("dimm: ");
        print_debug_hex8((0xa << 3));
        for(i = 0; i < 256; i++) {
                int status;
                unsigned char byte;
                if ((i & 0xf) == 0) {
                        print_debug("\r\n");
                        print_debug_hex8(i);
                        print_debug(": ");
                }
                status = smbus_read_byte((0xa << 3), i);
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



static void dump_spd_registers2(void)
{
        unsigned dev;
        print_debug("\r\n");
        for(dev = (0xa << 3); dev <= ((0xa << 3) +1); dev += 1) {
                print_debug("dimm: ");
                print_debug_hex8(dev);
                int status;
                unsigned char byte;
                status = smbus_read_byte(dev, 0);
                if (status < 0) {
                        print_debug("bad device\r\n");
                        continue;
                }
                byte = status & 0xff;
                print_debug_hex8(byte);
                print_debug("\r\n");
        }
}

static void main(void)
{
        uart_init();
        console_init();
        if (boot_cpu() && !cpu_init_detected()) {
                setup_coherent_ht_domain();
                enumerate_ht_chain();
                print_pci_devices();
                enable_smbus();
                sdram_initialize();



                dump_spd_registers1();
                dump_spd_registers2();





                ram_fill( 0x00000000, 0x00001000);
                ram_verify(0x00000000, 0x00001000);





        }
}
