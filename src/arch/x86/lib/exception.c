#include <console/console.h>
#include <string.h>

#if CONFIG_GDB_STUB

/* BUFMAX defines the maximum number of characters in inbound/outbound buffers.
 * At least NUM_REGBYTES*2 are needed for register packets
 */
#define BUFMAX 400
enum regnames {
	EAX = 0, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
	PC /* also known as eip */,
	PS /* also known as eflags */,
	CS, SS, DS, ES, FS, GS,
	NUM_REGS /* Number of registers. */
};

static uint32_t gdb_stub_registers[NUM_REGS];

#define GDB_SIG0         0     /* Signal 0 */
#define GDB_SIGHUP       1     /* Hangup */
#define GDB_SIGINT       2     /* Interrupt */
#define GDB_SIGQUIT      3     /* Quit */
#define GDB_SIGILL       4     /* Illegal instruction */
#define GDB_SIGTRAP      5     /* Trace/breakpoint trap */
#define GDB_SIGABRT      6     /* Aborted */
#define GDB_SIGEMT       7     /* Emulation trap */
#define GDB_SIGFPE       8     /* Arithmetic exception */
#define GDB_SIGKILL      9     /* Killed */
#define GDB_SIGBUS       10    /* Bus error */
#define GDB_SIGSEGV      11    /* Segmentation fault */
#define GDB_SIGSYS       12    /* Bad system call */
#define GDB_SIGPIPE      13    /* Broken pipe */
#define GDB_SIGALRM      14    /* Alarm clock */
#define GDB_SIGTERM      15    /* Terminated */
#define GDB_SIGURG       16    /* Urgent I/O condition */
#define GDB_SIGSTOP      17    /* Stopped (signal) */
#define GDB_SIGTSTP      18    /* Stopped (user) */
#define GDB_SIGCONT      19    /* Continued */
#define GDB_SIGCHLD      20    /* Child status changed */
#define GDB_SIGTTIN      21    /* Stopped (tty input) */
#define GDB_SIGTTOU      22    /* Stopped (tty output) */
#define GDB_SIGIO        23    /* I/O possible */
#define GDB_SIGXCPU      24    /* CPU time limit exceeded */
#define GDB_SIGXFSZ      25    /* File size limit exceeded */
#define GDB_SIGVTALRM    26    /* Virtual timer expired */
#define GDB_SIGPROF      27    /* Profiling timer expired */
#define GDB_SIGWINCH     28    /* Window size changed */
#define GDB_SIGLOST      29    /* Resource lost */
#define GDB_SIGUSR1      30    /* User defined signal 1 */
#define GDB_SUGUSR2      31    /* User defined signal 2 */
#define GDB_SIGPWR       32    /* Power fail/restart */
#define GDB_SIGPOLL      33    /* Pollable event occurred */
#define GDB_SIGWIND      34    /* SIGWIND */
#define GDB_SIGPHONE     35    /* SIGPHONE */
#define GDB_SIGWAITING   36    /* Process's LWPs are blocked */
#define GDB_SIGLWP       37    /* Signal LWP */
#define GDB_SIGDANGER    38    /* Swap space dangerously low */
#define GDB_SIGGRANT     39    /* Monitor mode granted */
#define GDB_SIGRETRACT   40    /* Need to relinquish monitor mode */
#define GDB_SIGMSG       41    /* Monitor mode data available */
#define GDB_SIGSOUND     42    /* Sound completed */
#define GDB_SIGSAK       43    /* Secure attention */
#define GDB_SIGPRIO      44    /* SIGPRIO */

#define GDB_SIG33        45    /* Real-time event 33 */
#define GDB_SIG34        46    /* Real-time event 34 */
#define GDB_SIG35        47    /* Real-time event 35 */
#define GDB_SIG36        48    /* Real-time event 36 */
#define GDB_SIG37        49    /* Real-time event 37 */
#define GDB_SIG38        50    /* Real-time event 38 */
#define GDB_SIG39        51    /* Real-time event 39 */
#define GDB_SIG40        52    /* Real-time event 40 */
#define GDB_SIG41        53    /* Real-time event 41 */
#define GDB_SIG42        54    /* Real-time event 42 */
#define GDB_SIG43        55    /* Real-time event 43 */
#define GDB_SIG44        56    /* Real-time event 44 */
#define GDB_SIG45        57    /* Real-time event 45 */
#define GDB_SIG46        58    /* Real-time event 46 */
#define GDB_SIG47        59    /* Real-time event 47 */
#define GDB_SIG48        60    /* Real-time event 48 */
#define GDB_SIG49        61    /* Real-time event 49 */
#define GDB_SIG50        62    /* Real-time event 50 */
#define GDB_SIG51        63    /* Real-time event 51 */
#define GDB_SIG52        64    /* Real-time event 52 */
#define GDB_SIG53        65    /* Real-time event 53 */
#define GDB_SIG54        66    /* Real-time event 54 */
#define GDB_SIG55        67    /* Real-time event 55 */
#define GDB_SIG56        68    /* Real-time event 56 */
#define GDB_SIG57        69    /* Real-time event 57 */
#define GDB_SIG58        70    /* Real-time event 58 */
#define GDB_SIG59        71    /* Real-time event 59 */
#define GDB_SIG60        72    /* Real-time event 60 */
#define GDB_SIG61        73    /* Real-time event 61 */
#define GDB_SIG62        74    /* Real-time event 62 */
#define GDB_SIG63        75    /* Real-time event 63 */
#define GDB_SIGCANCEL    76    /* LWP internal signal */
#define GDB_SIG32        77    /* Real-time event 32 */
#define GDB_SIG64        78    /* Real-time event 64 */
#define GDB_SIG65        79    /* Real-time event 65 */
#define GDB_SIG66        80    /* Real-time event 66 */
#define GDB_SIG67        81    /* Real-time event 67 */
#define GDB_SIG68        82    /* Real-time event 68 */
#define GDB_SIG69        83    /* Real-time event 69 */
#define GDB_SIG70        84    /* Real-time event 70 */
#define GDB_SIG71        85    /* Real-time event 71 */
#define GDB_SIG72        86    /* Real-time event 72 */
#define GDB_SIG73        87    /* Real-time event 73 */
#define GDB_SIG74        88    /* Real-time event 74 */
#define GDB_SIG75        89    /* Real-time event 75 */
#define GDB_SIG76        90    /* Real-time event 76 */
#define GDB_SIG77        91    /* Real-time event 77 */
#define GDB_SIG78        92    /* Real-time event 78 */
#define GDB_SIG79        93    /* Real-time event 79 */
#define GDB_SIG80        94    /* Real-time event 80 */
#define GDB_SIG81        95    /* Real-time event 81 */
#define GDB_SIG82        96    /* Real-time event 82 */
#define GDB_SIG83        97    /* Real-time event 83 */
#define GDB_SIG84        98    /* Real-time event 84 */
#define GDB_SIG85        99    /* Real-time event 85 */
#define GDB_SIG86       100    /* Real-time event 86 */
#define GDB_SIG87       101    /* Real-time event 87 */
#define GDB_SIG88       102    /* Real-time event 88 */
#define GDB_SIG89       103    /* Real-time event 89 */
#define GDB_SIG90       104    /* Real-time event 90 */
#define GDB_SIG91       105    /* Real-time event 91 */
#define GDB_SIG92       106    /* Real-time event 92 */
#define GDB_SIG93       107    /* Real-time event 93 */
#define GDB_SIG94       108    /* Real-time event 94 */
#define GDB_SIG95       109    /* Real-time event 95 */
#define GDB_SIG96       110    /* Real-time event 96 */
#define GDB_SIG97       111    /* Real-time event 97 */
#define GDB_SIG98       112    /* Real-time event 98 */
#define GDB_SIG99       113    /* Real-time event 99 */
#define GDB_SIG100      114    /* Real-time event 100 */
#define GDB_SIG101      115    /* Real-time event 101 */
#define GDB_SIG102      116    /* Real-time event 102 */
#define GDB_SIG103      117    /* Real-time event 103 */
#define GDB_SIG104      118    /* Real-time event 104 */
#define GDB_SIG105      119    /* Real-time event 105 */
#define GDB_SIG106      120    /* Real-time event 106 */
#define GDB_SIG107      121    /* Real-time event 107 */
#define GDB_SIG108      122    /* Real-time event 108 */
#define GDB_SIG109      123    /* Real-time event 109 */
#define GDB_SIG110      124    /* Real-time event 110 */
#define GDB_SIG111      125    /* Real-time event 111 */
#define GDB_SIG112      126    /* Real-time event 112 */
#define GDB_SIG113      127    /* Real-time event 113 */
#define GDB_SIG114      128    /* Real-time event 114 */
#define GDB_SIG115      129    /* Real-time event 115 */
#define GDB_SIG116      130    /* Real-time event 116 */
#define GDB_SIG117      131    /* Real-time event 117 */
#define GDB_SIG118      132    /* Real-time event 118 */
#define GDB_SIG119      133    /* Real-time event 119 */
#define GDB_SIG120      134    /* Real-time event 120 */
#define GDB_SIG121      135    /* Real-time event 121 */
#define GDB_SIG122      136    /* Real-time event 122 */
#define GDB_SIG123      137    /* Real-time event 123 */
#define GDB_SIG124      138    /* Real-time event 124 */
#define GDB_SIG125      139    /* Real-time event 125 */
#define GDB_SIG126      140    /* Real-time event 126 */
#define GDB_SIG127      141    /* Real-time event 127 */
#define GDB_SIGINFO     142    /* Information request */
#define GDB_UNKNOWN     143    /* Unknown signal */
#define GDB_DEFAULT     144    /* error: default signal */
/* Mach exceptions */
#define GDB_EXC_BAD_ACCESS     145 /* Could not access memory */
#define GDB_EXC_BAD_INSTRCTION 146 /* Illegal instruction/operand */
#define GDB_EXC_ARITHMETIC     147 /* Arithmetic exception */
#define GDB_EXC_EMULATION      148 /* Emulation instruction */
#define GDB_EXC_SOFTWARE       149 /* Software generated exception */
#define GDB_EXC_BREAKPOINT     150 /* Breakpoint */



static unsigned char exception_to_signal[] =
{
	[0]  = GDB_SIGFPE,  /* divide by zero */
	[1]  = GDB_SIGTRAP, /* debug exception */
	[2]  = GDB_SIGSEGV, /* NMI Interrupt */
	[3]  = GDB_SIGTRAP, /* Breakpoint */
	[4]  = GDB_SIGSEGV, /* into instruction (overflow) */
	[5]  = GDB_SIGSEGV, /* bound instruction */
	[6]  = GDB_SIGILL,  /* Invalid opcode */
	[7]  = GDB_SIGSEGV, /* coprocessor not available */
	[8]  = GDB_SIGSEGV, /* double fault */
	[9]  = GDB_SIGFPE,  /* coprocessor segment overrun */
	[10] = GDB_SIGSEGV, /* Invalid TSS */
	[11] = GDB_SIGBUS,  /* Segment not present */
	[12] = GDB_SIGBUS,  /* stack exception */
	[13] = GDB_SIGSEGV, /* general protection */
	[14] = GDB_SIGSEGV, /* page fault */
	[15] = GDB_UNKNOWN, /* reserved */
	[16] = GDB_SIGEMT,  /* coprocessor error */
	[17] = GDB_SIGBUS,  /* alignment check */
	[18] = GDB_SIGSEGV, /* machine check */
	[19] = GDB_SIGFPE,  /* simd floating point exception */
	[20] = GDB_UNKNOWN,
	[21] = GDB_UNKNOWN,
	[22] = GDB_UNKNOWN,
	[23] = GDB_UNKNOWN,
	[24] = GDB_UNKNOWN,
	[25] = GDB_UNKNOWN,
	[26] = GDB_UNKNOWN,
	[27] = GDB_UNKNOWN,
	[28] = GDB_UNKNOWN,
	[29] = GDB_UNKNOWN,
	[30] = GDB_UNKNOWN,
	[31] = GDB_UNKNOWN,
	[32] = GDB_SIGINT,  /* User interrupt */
};

static const char hexchars[] = "0123456789abcdef";
static char in_buffer[BUFMAX];
static char out_buffer[BUFMAX];


static inline void stub_putc(int ch)
{
	console_tx_byte(ch);
}

static inline int stub_getc(void)
{
	return console_rx_byte();
}

static int hex(char ch)
{
	if ((ch >= 'a') && (ch <= 'f'))
		return (ch - 'a' + 10);
	if ((ch >= '0') && (ch <= '9'))
		return (ch - '0');
	if ((ch >= 'A') && (ch <= 'F'))
		return (ch - 'A' + 10);
	return (-1);
}

/*
 * While we find hexadecimal digits, build an int.
 * Fals is returned if nothing is parsed true otherwise.
 */
static int parse_ulong(char **ptr, unsigned long *value)
{
	int digit;
	char *start;

	start = *ptr;
	*value = 0;

	while((digit = hex(**ptr)) >= 0) {
		*value = ((*value) << 4) | digit;
		(*ptr)++;
	}
	return start != *ptr;
}

/* convert the memory pointed to by mem into hex, placing result in buf */
/* return a pointer to the last char put in buf (null) */
static void copy_to_hex(char *buf, void *addr, unsigned long count)
{
	unsigned char ch;
	char *mem = addr;

	while(count--) {
		ch = *mem++;
		*buf++ = hexchars[ch >> 4];
		*buf++ = hexchars[ch & 0x0f];
	}
	*buf = 0;
	return;
}


/* convert the hex array pointed to by buf into binary to be placed in mem */
/* return a pointer to the character AFTER the last byte written */
static void copy_from_hex(void *addr, char *buf, unsigned long count)
{
	unsigned char ch;
	char *mem = addr;

	while(count--) {
		ch = hex (*buf++) << 4;
		ch = ch + hex (*buf++);
		*mem++ = ch;
	}
}


/* scan for the sequence $<data>#<checksum>	*/

static int get_packet(char *buffer)
{
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	char ch;

	/* Wishlit implement a timeout in get_packet */
	do {
		/* wait around for the start character, ignore all other characters */
		while ((ch = (stub_getc() & 0x7f)) != '$');
		checksum = 0;
		xmitcsum = -1;

		count = 0;

		/* now, read until a # or end of buffer is found */
		while (count < BUFMAX) {
			ch = stub_getc() & 0x7f;
			if (ch == '#')
				break;
			checksum = checksum + ch;
			buffer[count] = ch;
			count = count + 1;
		}
		buffer[count] = 0;

		if (ch == '#') {
			xmitcsum = hex(stub_getc() & 0x7f) << 4;
			xmitcsum += hex(stub_getc() & 0x7f);

			if (checksum != xmitcsum) {
				stub_putc('-');	/* failed checksum */
			}
			else {
				stub_putc('+');	/* successful transfer */
			}
		}
	} while(checksum != xmitcsum);
	return 1;
}

/* send the packet in buffer.*/
static void put_packet(char *buffer)
{
	unsigned char checksum;
	int count;
	char ch;

	/*  $<packet info>#<checksum>. */
	do {
		stub_putc('$');
		checksum = 0;
		count = 0;

		while ((ch = buffer[count])) {
			stub_putc(ch);
			checksum += ch;
			count += 1;
		}

		stub_putc('#');
		stub_putc(hexchars[checksum >> 4]);
		stub_putc(hexchars[checksum % 16]);

	} while ((stub_getc() & 0x7f) != '+');

}
#endif /* CONFIG_GDB_STUB */

#include <arch/registers.h>

void x86_exception(struct eregs *info);

void x86_exception(struct eregs *info)
{
#if CONFIG_GDB_STUB
	int signo;
	memcpy(gdb_stub_registers, info, 8*sizeof(uint32_t));
	gdb_stub_registers[PC] = info->eip;
	gdb_stub_registers[CS] = info->cs;
	gdb_stub_registers[PS] = info->eflags;
	signo = GDB_UNKNOWN;
	if (info->vector < ARRAY_SIZE(exception_to_signal)) {
		signo = exception_to_signal[info->vector];
	}

	/* reply to the host that an exception has occured */
	out_buffer[0] = 'S';
	out_buffer[1] = hexchars[(signo>>4) & 0xf];
	out_buffer[2] = hexchars[signo & 0xf];
	out_buffer[3] = '\0';
	put_packet(out_buffer);

	while(1) {
		unsigned long addr, length;
		char *ptr;
		out_buffer[0] = '\0';
		out_buffer[1] = '\0';
		if (!get_packet(in_buffer)) {
			break;
		}
		switch(in_buffer[0]) {
		case '?': /* last signal */
			out_buffer[0] = 'S';
			out_buffer[1] = hexchars[(signo >> 4) & 0xf];
			out_buffer[2] = hexchars[signo & 0xf];
			out_buffer[3] = '\0';
			break;
		case 'g': /* return the value of the cpu registers */
			copy_to_hex(out_buffer, &gdb_stub_registers, sizeof(gdb_stub_registers));
			break;
		case 'G': /* set the value of the CPU registers - return OK */
			copy_from_hex(&gdb_stub_registers, in_buffer + 1, sizeof(gdb_stub_registers));
			memcpy(info, gdb_stub_registers, 8*sizeof(uint32_t));
			info->eip    = gdb_stub_registers[PC];
			info->cs     = gdb_stub_registers[CS];
			info->eflags = gdb_stub_registers[PS];
			memcpy(out_buffer, "OK",3);
			break;
		case 'm':
			/* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
			ptr = &in_buffer[1];
			if (	parse_ulong(&ptr, &addr) &&
				(*ptr++ == ',') &&
				parse_ulong(&ptr, &length)) {
				copy_to_hex(out_buffer, (void *)addr, length);
			} else {
				memcpy(out_buffer, "E01", 4);
			}
			break;
		case 'M':
			/* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
			ptr = &in_buffer[1];
			if (	parse_ulong(&ptr, &addr) &&
				(*(ptr++) == ',') &&
				parse_ulong(&ptr, &length) &&
				(*(ptr++) == ':')) {
				copy_from_hex((void *)addr, ptr, length);
				memcpy(out_buffer, "OK", 3);
			}
			else {
				memcpy(out_buffer, "E02", 4);
			}
			break;
		case 's':
		case 'c':
			/* cAA..AA    Continue at address AA..AA(optional) */
			/* sAA..AA    Step one instruction from AA..AA(optional) */
			ptr = &in_buffer[1];
			if (parse_ulong(&ptr, &addr)) {
				info->eip = addr;
			}

			/* Clear the trace bit */
			info->eflags &= ~(1 << 8);
			/* Set the trace bit if we are single stepping */
			if (in_buffer[0] == 's') {
				info->eflags |= (1 << 8);
			}
			return;
			break;
		case 'D':
			memcpy(out_buffer, "OK", 3);
			break;
		case 'k':  /* kill request? */
			break;
		case 'q':  /* query */
			break;
		case 'z':  /* z0AAAA,LLLL remove memory breakpoint */
			   /* z1AAAA,LLLL remove hardware breakpoint */
			   /* z2AAAA,LLLL remove write watchpoint */
			   /* z3AAAA,LLLL remove read watchpoint */
			   /* z4AAAA,LLLL remove access watchpoint */
		case 'Z':  /* Z0AAAA,LLLL insert memory breakpoint */
			   /* Z1AAAA,LLLL insert hardware breakpoint */
			   /* Z2AAAA,LLLL insert write watchpoint */
			   /* Z3AAAA,LLLL insert read watchpoint */
			   /* Z4AAAA,LLLL insert access watchpoint */
			break;
		default:
			break;
		}
		put_packet(out_buffer);
	}
#else /* !CONFIG_GDB_STUB */
#define MDUMP_SIZE 0x80
	printk(BIOS_EMERG,
		"Unexpected Exception: %d @ %02x:%08x - Halting\n"
		"Code: %d eflags: %08x\n"
		"eax: %08x ebx: %08x ecx: %08x edx: %08x\n"
		"edi: %08x esi: %08x ebp: %08x esp: %08x\n",
		info->vector, info->cs, info->eip,
		info->error_code, info->eflags,
		info->eax, info->ebx, info->ecx, info->edx,
		info->edi, info->esi, info->ebp, info->esp);
	u8 *code = (u8*)((u32)info->eip - (MDUMP_SIZE >>1));
	/* Align to 8-byte boundary please, and print eight bytes per row.
	 * This is done to make DRAM burst timing/reordering errors more
	 * evident from the looking at the dump */
	code = (u8*)((u32)code & ~0x7);
	int i;
	for(i = 0; i < MDUMP_SIZE; i++)
	{
		if( (i & 0x07) == 0 )
			printk(BIOS_EMERG, "\n%.8x:\t", (int)code + i );
		printk(BIOS_EMERG, "%.2x ", code[i]);
	}
	die("");
#endif
}
