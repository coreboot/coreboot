/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __COMMON_H_
#define __COMMON_H_	1

#undef	_LINUX_CONFIG_H
#define _LINUX_CONFIG_H 1	/* avoid reading Linux autoconf.h file	*/

#ifndef __ASSEMBLER__		/* put C only stuff in this section */

typedef unsigned char		uchar;
typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;

#include <config.h>
#include <asm-offsets.h>
#include <linux/bitops.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/ptrace.h>
#include <stdarg.h>
#if defined(CONFIG_PCI) && (defined(CONFIG_4xx) && !defined(CONFIG_AP1000))
#include <pci.h>
#endif
#if defined(CONFIG_8xx)
#include <asm/8xx_immap.h>
#if defined(CONFIG_MPC852)	|| defined(CONFIG_MPC852T)	|| \
    defined(CONFIG_MPC859)	|| defined(CONFIG_MPC859T)	|| \
    defined(CONFIG_MPC859DSL)	|| \
    defined(CONFIG_MPC866)	|| defined(CONFIG_MPC866T)	|| \
    defined(CONFIG_MPC866P)
# define CONFIG_MPC866_FAMILY 1
#elif defined(CONFIG_MPC870) \
   || defined(CONFIG_MPC875) \
   || defined(CONFIG_MPC880) \
   || defined(CONFIG_MPC885)
# define CONFIG_MPC885_FAMILY   1
#endif
#if   defined(CONFIG_MPC860)	   \
   || defined(CONFIG_MPC860T)	   \
   || defined(CONFIG_MPC866_FAMILY) \
   || defined(CONFIG_MPC885_FAMILY)
# define CONFIG_MPC86x 1
#endif
#elif defined(CONFIG_5xx)
#include <asm/5xx_immap.h>
#elif defined(CONFIG_MPC5xxx)
#include <mpc5xxx.h>
#elif defined(CONFIG_MPC512X)
#include <asm/immap_512x.h>
#elif defined(CONFIG_MPC8220)
#include <asm/immap_8220.h>
#elif defined(CONFIG_8260)
#if   defined(CONFIG_MPC8247) \
   || defined(CONFIG_MPC8248) \
   || defined(CONFIG_MPC8271) \
   || defined(CONFIG_MPC8272)
#define CONFIG_MPC8272_FAMILY	1
#endif
#if defined(CONFIG_MPC8272_FAMILY)
#define CONFIG_MPC8260	1
#endif
#include <asm/immap_8260.h>
#endif
#ifdef CONFIG_MPC86xx
#include <mpc86xx.h>
#include <asm/immap_86xx.h>
#endif
#ifdef CONFIG_MPC85xx
#include <mpc85xx.h>
#include <asm/immap_85xx.h>
#endif
#ifdef CONFIG_MPC83xx
#include <mpc83xx.h>
#include <asm/immap_83xx.h>
#endif
#ifdef	CONFIG_4xx
#include <asm/ppc4xx.h>
#endif
#ifdef CONFIG_HYMOD
#include <board/hymod/hymod.h>
#endif
#ifdef CONFIG_ARM
#define asmlinkage	/* nothing */
#endif
#ifdef CONFIG_BLACKFIN
#include <asm/blackfin.h>
#endif
#ifdef CONFIG_SOC_DA8XX
#include <asm/arch/hardware.h>
#endif

#include <part.h>
#include <flash.h>
#include <image.h>

#ifdef	DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#define debugX(level,fmt,args...) if (DEBUG>=level) printf(fmt,##args);
#else
#define debug(fmt,args...)
#define debugX(level,fmt,args...)
#endif	/* DEBUG */

#ifdef DEBUG
# define _DEBUG 1
#else
# define _DEBUG 0
#endif

/*
 * An assertion is run-time check done in debug mode only. If DEBUG is not
 * defined then it is skipped. If DEBUG is defined and the assertion fails,
 * then it calls panic*( which may or may not reset/halt U-Boot (see
 * CONFIG_PANIC_HANG), It is hoped that all failing assertions are found
 * before release, and after release it is hoped that they don't matter. But
 * in any case these failing assertions cannot be fixed with a reset (which
 * may just do the same assertion again).
 */
void __assert_fail(const char *assertion, const char *file, unsigned line,
		   const char *function);
#define assert(x) \
	({ if (!(x) && _DEBUG) \
		__assert_fail(#x, __FILE__, __LINE__, __func__); })

#define error(fmt, args...) do {					\
		printf("ERROR: " fmt "\nat %s:%d/%s()\n",		\
			##args, __FILE__, __LINE__, __func__);		\
} while (0)

#ifndef BUG
#define BUG() do { \
	printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __FUNCTION__); \
	panic("BUG!"); \
} while (0)
#define BUG_ON(condition) do { if (unlikely((condition)!=0)) BUG(); } while(0)
#endif /* BUG */

/* Force a compilation error if condition is true */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

typedef void (interrupt_handler_t)(void *);

#include <asm/u-boot.h> /* boot information for Linux kernel */
#include <asm/global_data.h>	/* global data used for startup functions */

/*
 * enable common handling for all TQM8xxL/M boards:
 * - CONFIG_TQM8xxM will be defined for all TQM8xxM boards
 * - CONFIG_TQM8xxL will be defined for all TQM8xxL _and_ TQM8xxM boards
 *                  and for the TQM885D board
 */
#if defined(CONFIG_TQM823M) || defined(CONFIG_TQM850M) || \
    defined(CONFIG_TQM855M) || defined(CONFIG_TQM860M) || \
    defined(CONFIG_TQM862M) || defined(CONFIG_TQM866M)
# ifndef CONFIG_TQM8xxM
#  define CONFIG_TQM8xxM
# endif
#endif
#if defined(CONFIG_TQM823L) || defined(CONFIG_TQM850L) || \
    defined(CONFIG_TQM855L) || defined(CONFIG_TQM860L) || \
    defined(CONFIG_TQM862L) || defined(CONFIG_TQM8xxM) || \
    defined(CONFIG_TQM885D)
# ifndef CONFIG_TQM8xxL
#  define CONFIG_TQM8xxL
# endif
#endif

#ifndef CONFIG_SERIAL_MULTI

#if defined(CONFIG_8xx_CONS_SMC1) || defined(CONFIG_8xx_CONS_SMC2) \
 || defined(CONFIG_8xx_CONS_SCC1) || defined(CONFIG_8xx_CONS_SCC2) \
 || defined(CONFIG_8xx_CONS_SCC3) || defined(CONFIG_8xx_CONS_SCC4)

#define CONFIG_SERIAL_MULTI	1

#endif

#endif /* CONFIG_SERIAL_MULTI */

/*
 * General Purpose Utilities
 */
#define min(X, Y)				\
	({ typeof (X) __x = (X);		\
		typeof (Y) __y = (Y);		\
		(__x < __y) ? __x : __y; })

#define max(X, Y)				\
	({ typeof (X) __x = (X);		\
		typeof (Y) __y = (Y);		\
		(__x > __y) ? __x : __y; })

#define MIN(x, y)  min(x, y)
#define MAX(x, y)  max(x, y)

#if defined(CONFIG_ENV_IS_EMBEDDED)
#define TOTAL_MALLOC_LEN	CONFIG_SYS_MALLOC_LEN
#elif ( ((CONFIG_ENV_ADDR+CONFIG_ENV_SIZE) < CONFIG_SYS_MONITOR_BASE) || \
	(CONFIG_ENV_ADDR >= (CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)) ) || \
      defined(CONFIG_ENV_IS_IN_NVRAM)
#define	TOTAL_MALLOC_LEN	(CONFIG_SYS_MALLOC_LEN + CONFIG_ENV_SIZE)
#else
#define	TOTAL_MALLOC_LEN	CONFIG_SYS_MALLOC_LEN
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

/*
 * Function Prototypes
 */

void	hang		(void) __attribute__ ((noreturn));

int	timer_init(void);
int	cpu_init(void);

/* */
phys_size_t initdram (int);
int	display_options (void);
void	print_size(unsigned long long, const char *);
int	print_buffer (ulong addr, void* data, uint width, uint count, uint linelen);

/* common/main.c */
void	main_loop	(void);
int	run_command	(const char *cmd, int flag);
int	readline	(const char *const prompt);
int	readline_into_buffer	(const char *const prompt, char * buffer);
int	parse_line (char *, char *[]);
void	init_cmd_timeout(void);
void	reset_cmd_timeout(void);

/* arch/$(ARCH)/lib/board.c */
void	board_init_f  (ulong) __attribute__ ((noreturn));
void	board_init_r  (gd_t *, ulong) __attribute__ ((noreturn));
int	checkboard    (void);
int	checkflash    (void);
int	checkdram     (void);
int	last_stage_init(void);
extern ulong monitor_flash_len;
int mac_read_from_eeprom(void);

/* common/flash.c */
void flash_perror (int);

/* common/cmd_source.c */
int	source (ulong addr, const char *fit_uname);

extern ulong load_addr;		/* Default Load Address */

/* common/cmd_doc.c */
void	doc_probe(unsigned long physadr);

/* common/cmd_nvedit.c */
int	env_init     (void);
void	env_relocate (void);
int	envmatch     (uchar *, int);
char	*getenv	     (const char *);
int	getenv_f     (const char *name, char *buf, unsigned len);
int	saveenv	     (void);
#ifdef CONFIG_PPC		/* ARM version to be fixed! */
int inline setenv    (const char *, const char *);
#else
int	setenv	     (const char *, const char *);
#endif /* CONFIG_PPC */
#ifdef CONFIG_ARM
# include <asm/mach-types.h>
# include <asm/setup.h>
# include <asm/u-boot-arm.h>	/* ARM version to be fixed! */
#endif /* CONFIG_ARM */
#ifdef CONFIG_X86		/* x86 version to be fixed! */
# include <asm/u-boot-x86.h>
#endif /* CONFIG_X86 */

#ifdef CONFIG_AUTO_COMPLETE
int env_complete(char *var, int maxv, char *cmdv[], int maxsz, char *buf);
#endif
int get_env_id (void);

void	pci_init      (void);
void	pci_init_board(void);
void	pciinfo	      (int, int);

#if defined(CONFIG_PCI) && (defined(CONFIG_4xx) && !defined(CONFIG_AP1000))
    int	   pci_pre_init	       (struct pci_controller *);
    int	   is_pci_host	       (struct pci_controller *);
#endif

#if defined(CONFIG_PCI) && (defined(CONFIG_440) || defined(CONFIG_405EX))
#   if defined(CONFIG_SYS_PCI_TARGET_INIT)
	void	pci_target_init	     (struct pci_controller *);
#   endif
#   if defined(CONFIG_SYS_PCI_MASTER_INIT)
	void	pci_master_init	     (struct pci_controller *);
#   endif
#if defined(CONFIG_440SPE) || \
    defined(CONFIG_460EX) || defined(CONFIG_460GT) || \
    defined(CONFIG_405EX)
   void pcie_setup_hoses(int busno);
#endif
#endif

int	misc_init_f   (void);
int	misc_init_r   (void);

/* common/exports.c */
void	jumptable_init(void);

/* common/kallsysm.c */
const char *symbol_lookup(unsigned long addr, unsigned long *caddr);

/* api/api.c */
void	api_init (void);

/* common/memsize.c */
long	get_ram_size  (long *, long);

/* $(BOARD)/$(BOARD).c */
void	reset_phy     (void);
void	fdc_hw_init   (void);

/* $(BOARD)/eeprom.c */
void eeprom_init  (void);
#ifndef CONFIG_SPI
int  eeprom_probe (unsigned dev_addr, unsigned offset);
#endif
int  eeprom_read  (unsigned dev_addr, unsigned offset, uchar *buffer, unsigned cnt);
int  eeprom_write (unsigned dev_addr, unsigned offset, uchar *buffer, unsigned cnt);
#ifdef CONFIG_LWMON
extern uchar pic_read  (uchar reg);
extern void  pic_write (uchar reg, uchar val);
#endif

/*
 * Set this up regardless of board
 * type, to prevent errors.
 */
#if defined(CONFIG_SPI) || !defined(CONFIG_SYS_I2C_EEPROM_ADDR)
# define CONFIG_SYS_DEF_EEPROM_ADDR 0
#else
#if !defined(CONFIG_ENV_EEPROM_IS_ON_I2C)
# define CONFIG_SYS_DEF_EEPROM_ADDR CONFIG_SYS_I2C_EEPROM_ADDR
#endif
#endif /* CONFIG_SPI || !defined(CONFIG_SYS_I2C_EEPROM_ADDR) */

#if defined(CONFIG_SPI)
extern void spi_init_f (void);
extern void spi_init_r (void);
extern ssize_t spi_read	 (uchar *, int, uchar *, int);
extern ssize_t spi_write (uchar *, int, uchar *, int);
#endif

#ifdef CONFIG_RPXCLASSIC
void rpxclassic_init (void);
#endif

void rpxlite_init (void);

#ifdef CONFIG_MBX
/* $(BOARD)/mbx8xx.c */
void	mbx_init (void);
void	board_serial_init (void);
void	board_ether_init (void);
#endif

#ifdef CONFIG_HERMES
/* $(BOARD)/hermes.c */
void hermes_start_lxt980 (int speed);
#endif

#ifdef CONFIG_EVB64260
void  evb64260_init(void);
void  debug_led(int, int);
void  display_mem_map(void);
void  perform_soft_reset(void);
#endif

/* $(BOARD)/$(BOARD).c */
int board_early_init_f (void);
int board_late_init (void);
int board_postclk_init (void); /* after clocks/timebase, before env/serial */
int board_early_init_r (void);
void board_poweroff (void);

#if defined(CONFIG_SYS_DRAM_TEST)
int testdram(void);
#endif /* CONFIG_SYS_DRAM_TEST */

/* $(CPU)/start.S */
#if defined(CONFIG_5xx) || \
    defined(CONFIG_8xx)
uint	get_immr      (uint);
#endif
uint	get_pir	      (void);
#if defined(CONFIG_MPC5xxx)
uint	get_svr       (void);
#endif
uint	get_pvr	      (void);
uint	get_svr	      (void);
uint	rd_ic_cst     (void);
void	wr_ic_cst     (uint);
void	wr_ic_adr     (uint);
uint	rd_dc_cst     (void);
void	wr_dc_cst     (uint);
void	wr_dc_adr     (uint);
int	icache_status (void);
void	icache_enable (void);
void	icache_disable(void);
int	dcache_status (void);
void	dcache_enable (void);
void	dcache_disable(void);
void	mmu_disable(void);
void	relocate_code (ulong, gd_t *, ulong) __attribute__ ((noreturn));
ulong	get_endaddr   (void);
void	trap_init     (ulong);
#if defined (CONFIG_4xx)	|| \
    defined (CONFIG_MPC5xxx)	|| \
    defined (CONFIG_74xx_7xx)	|| \
    defined (CONFIG_74x)	|| \
    defined (CONFIG_75x)	|| \
    defined (CONFIG_74xx)	|| \
    defined (CONFIG_MPC8220)	|| \
    defined (CONFIG_MPC85xx)	|| \
    defined (CONFIG_MPC86xx)	|| \
    defined (CONFIG_MPC83xx)
unsigned char	in8(unsigned int);
void		out8(unsigned int, unsigned char);
unsigned short	in16(unsigned int);
unsigned short	in16r(unsigned int);
void		out16(unsigned int, unsigned short value);
void		out16r(unsigned int, unsigned short value);
unsigned long	in32(unsigned int);
unsigned long	in32r(unsigned int);
void		out32(unsigned int, unsigned long value);
void		out32r(unsigned int, unsigned long value);
void		ppcDcbf(unsigned long value);
void		ppcDcbi(unsigned long value);
void		ppcSync(void);
void		ppcDcbz(unsigned long value);
#endif
#if defined (CONFIG_MICROBLAZE)
unsigned short	in16(unsigned int);
void		out16(unsigned int, unsigned short value);
#endif

#if defined (CONFIG_MPC83xx)
void		ppcDWload(unsigned int *addr, unsigned int *ret);
void		ppcDWstore(unsigned int *addr, unsigned int *value);
void disable_addr_trans(void);
void enable_addr_trans(void);
#if defined(CONFIG_DDR_ECC) && !defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)
void ddr_enable_ecc(unsigned int dram_size);
#endif
#endif

/* $(CPU)/cpu.c */
static inline int cpumask_next(int cpu, unsigned int mask)
{
	for (cpu++; !((1 << cpu) & mask); cpu++)
		;

	return cpu;
}

#define for_each_cpu(iter, cpu, num_cpus, mask) \
	for (iter = 0, cpu = cpumask_next(-1, mask); \
		iter < num_cpus; \
		iter++, cpu = cpumask_next(cpu, mask)) \

int	cpu_numcores  (void);
u32	cpu_mask      (void);
int	is_core_valid (unsigned int);
int	probecpu      (void);
int	checkcpu      (void);
int	checkicache   (void);
int	checkdcache   (void);
void	upmconfig     (unsigned int, unsigned int *, unsigned int);
ulong	get_tbclk     (void);
void	reset_cpu     (ulong addr);
#if defined (CONFIG_OF_LIBFDT) && defined (CONFIG_OF_BOARD_SETUP)
void ft_cpu_setup(void *blob, bd_t *bd);
#ifdef CONFIG_PCI
void ft_pci_setup(void *blob, bd_t *bd);
#endif
#endif


/* $(CPU)/serial.c */
int	serial_init   (void);
void	serial_setbrg (void);
void	serial_putc   (const char);
void	serial_putc_raw(const char);
void	serial_puts   (const char *);
int	serial_getc   (void);
int	serial_tstc   (void);

void	_serial_setbrg (const int);
void	_serial_putc   (const char, const int);
void	_serial_putc_raw(const char, const int);
void	_serial_puts   (const char *, const int);
int	_serial_getc   (const int);
int	_serial_tstc   (const int);

/* $(CPU)/speed.c */
int	get_clocks (void);
int	get_clocks_866 (void);
int	sdram_adjust_866 (void);
int	adjust_sdram_tbs_8xx (void);
#if defined(CONFIG_8260)
int	prt_8260_clks (void);
#elif defined(CONFIG_MPC5xxx)
int	prt_mpc5xxx_clks (void);
#endif
#if defined(CONFIG_MPC512X)
int	prt_mpc512xxx_clks (void);
#endif
#if defined(CONFIG_MPC8220)
int	prt_mpc8220_clks (void);
#endif
#ifdef CONFIG_4xx
ulong	get_OPB_freq (void);
ulong	get_PCI_freq (void);
#endif
#if defined(CONFIG_S3C24X0) || \
    defined(CONFIG_LH7A40X) || \
    defined(CONFIG_S3C6400) || \
    defined(CONFIG_EP93XX)
ulong	get_FCLK (void);
ulong	get_HCLK (void);
ulong	get_PCLK (void);
ulong	get_UCLK (void);
#endif
#if defined(CONFIG_LH7A40X)
ulong	get_PLLCLK (void);
#endif
#if defined CONFIG_INCA_IP
uint	incaip_get_cpuclk (void);
#endif
#if defined(CONFIG_IMX)
ulong get_systemPLLCLK(void);
ulong get_FCLK(void);
ulong get_HCLK(void);
ulong get_BCLK(void);
ulong get_PERCLK1(void);
ulong get_PERCLK2(void);
ulong get_PERCLK3(void);
#endif
ulong	get_bus_freq  (ulong);
int get_serial_clock(void);

#if defined(CONFIG_MPC83xx) || defined(CONFIG_MPC85xx)
ulong get_ddr_freq(ulong);
#endif
#if defined(CONFIG_MPC85xx)
typedef MPC85xx_SYS_INFO sys_info_t;
void	get_sys_info  ( sys_info_t * );
#endif
#if defined(CONFIG_MPC86xx)
typedef MPC86xx_SYS_INFO sys_info_t;
void   get_sys_info  ( sys_info_t * );
static inline ulong get_ddr_freq(ulong dummy)
{
	return get_bus_freq(dummy);
}
#endif

#if defined(CONFIG_4xx) || defined(CONFIG_IOP480)
#  if defined(CONFIG_440)
#	if defined(CONFIG_440SPE)
	 unsigned long determine_sysper(void);
	 unsigned long determine_pci_clock_per(void);
#	endif
#  endif
typedef PPC4xx_SYS_INFO sys_info_t;
int	ppc440spe_revB(void);
void	get_sys_info  ( sys_info_t * );
#endif

/* $(CPU)/cpu_init.c */
#if defined(CONFIG_8xx) || defined(CONFIG_8260)
void	cpu_init_f    (volatile immap_t *immr);
#endif
#if defined(CONFIG_4xx) || defined(CONFIG_MPC85xx) || defined(CONFIG_MCF52x2) ||defined(CONFIG_MPC86xx)
void	cpu_init_f    (void);
#endif

int	cpu_init_r    (void);
#if defined(CONFIG_8260)
int	prt_8260_rsr  (void);
#elif defined(CONFIG_MPC83xx)
int	prt_83xx_rsr  (void);
#endif

/* $(CPU)/interrupts.c */
int	interrupt_init	   (void);
void	timer_interrupt	   (struct pt_regs *);
void	external_interrupt (struct pt_regs *);
void	irq_install_handler(int, interrupt_handler_t *, void *);
void	irq_free_handler   (int);
void	reset_timer	   (void);
ulong	get_timer	   (ulong base);
void	enable_interrupts  (void);
int	disable_interrupts (void);

/* $(CPU)/.../commproc.c */
int	dpram_init (void);
uint	dpram_base(void);
uint	dpram_base_align(uint align);
uint	dpram_alloc(uint size);
uint	dpram_alloc_align(uint size,uint align);
void	bootcount_store (ulong);
ulong	bootcount_load (void);
#define BOOTCOUNT_MAGIC		0xB001C041

/* $(CPU)/.../<eth> */
void mii_init (void);

/* $(CPU)/.../lcd.c */
ulong	lcd_setmem (ulong);

/* $(CPU)/.../video.c */
ulong	video_setmem (ulong);

/* arch/$(ARCH)/lib/cache.c */
void	enable_caches(void);
void	flush_cache   (unsigned long, unsigned long);
void	flush_dcache_all(void);
void	flush_dcache_range(unsigned long start, unsigned long stop);
void	invalidate_dcache_range(unsigned long start, unsigned long stop);
void	invalidate_dcache_all(void);
void	invalidate_icache_all(void);

/* arch/$(ARCH)/lib/ticks.S */
unsigned long long get_ticks(void);
void	wait_ticks    (unsigned long);

/* arch/$(ARCH)/lib/time.c */
void	__udelay      (unsigned long);
ulong	usec2ticks    (unsigned long usec);
ulong	ticks2usec    (unsigned long ticks);
int	init_timebase (void);

/* lib/gunzip.c */
int gunzip(void *, int, unsigned char *, unsigned long *);
int zunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp,
						int stoponerr, int offset);

/* lib/net_utils.c */
#include <net.h>
static inline IPaddr_t getenv_IPaddr (char *var)
{
	return (string_to_ip(getenv(var)));
}

/* lib/qsort.c */
void qsort(void *base, size_t nmemb, size_t size,
	   int(*compar)(const void *, const void *));
int strcmp_compar(const void *, const void *);

/* lib/time.c */
void	udelay        (unsigned long);

/* lib/vsprintf.c */
ulong	simple_strtoul(const char *cp,char **endp,unsigned int base);
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);
unsigned long long	simple_strtoull(const char *cp,char **endp,unsigned int base);
long	simple_strtol(const char *cp,char **endp,unsigned int base);
void	panic(const char *fmt, ...)
		__attribute__ ((format (__printf__, 1, 2), noreturn));
int	sprintf(char * buf, const char *fmt, ...)
		__attribute__ ((format (__printf__, 2, 3)));
int	vsprintf(char *buf, const char *fmt, va_list args);

/* lib/strmhz.c */
char *	strmhz(char *buf, unsigned long hz);

/* lib/crc32.c */
#include <u-boot/crc.h>

/* common/console.c */
int	console_init_f(void);	/* Before relocation; uses the serial  stuff	*/
int	console_init_r(void);	/* After  relocation; uses the console stuff	*/
int	console_assign(int file, const char *devname);	/* Assign the console	*/
int	ctrlc (void);
int	had_ctrlc (void);	/* have we had a Control-C since last clear? */
void	clear_ctrlc (void);	/* clear the Control-C condition */
int	disable_ctrlc (int);	/* 1 to disable, 0 to enable Control-C detect */

/*
 * STDIO based functions (can always be used)
 */
/* serial stuff */
int	serial_printf (const char *fmt, ...)
		__attribute__ ((format (__printf__, 1, 2)));
/* stdin */
int	getc(void);
int	tstc(void);

/* stdout */
void	putc(const char c);
void	puts(const char *s);
int	printf(const char *fmt, ...)
		__attribute__ ((format (__printf__, 1, 2)));
int	vprintf(const char *fmt, va_list args);

/* stderr */
#define eputc(c)		fputc(stderr, c)
#define eputs(s)		fputs(stderr, s)
#define eprintf(fmt,args...)	fprintf(stderr,fmt ,##args)

/*
 * FILE based functions (can only be used AFTER relocation!)
 */
#define stdin		0
#define stdout		1
#define stderr		2
#define MAX_FILES	3

int	fprintf(int file, const char *fmt, ...)
		__attribute__ ((format (__printf__, 2, 3)));
void	fputs(int file, const char *s);
void	fputc(int file, const char c);
int	ftstc(int file);
int	fgetc(int file);

/*
 * CONSOLE multiplexing.
 */
#ifdef CONFIG_CONSOLE_MUX
#include <iomux.h>
#endif

int	pcmcia_init (void);

#ifdef CONFIG_STATUS_LED
# include <status_led.h>
#endif
/*
 * Board-specific Platform code can reimplement show_boot_progress () if needed
 */
void show_boot_progress(int val);

/* Multicore arch functions */
#ifdef CONFIG_MP
int cpu_status(int nr);
int cpu_reset(int nr);
int cpu_disable(int nr);
int cpu_release(int nr, int argc, char * const argv[]);
#endif

#endif /* __ASSEMBLER__ */

#ifdef CONFIG_PPC
/*
 * Has to be included outside of the #ifndef __ASSEMBLER__ section.
 * Otherwise might lead to compilation errors in assembler files.
 */
#include <asm/cache.h>
#endif

/* Put only stuff here that the assembler can digest */

#ifdef CONFIG_POST
#define CONFIG_HAS_POST
#ifndef CONFIG_POST_ALT_LIST
#define CONFIG_POST_STD_LIST
#endif
#endif

#ifdef CONFIG_INIT_CRITICAL
#error CONFIG_INIT_CRITICAL is deprecated!
#error Read section CONFIG_SKIP_LOWLEVEL_INIT in README.
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define ROUND(a,b)		(((a) + (b) - 1) & ~((b) - 1))
#define DIV_ROUND(n,d)		(((n) + ((d)/2)) / (d))
#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define roundup(x, y)		((((x) + ((y) - 1)) / (y)) * (y))

#define ALIGN(x,a)		__ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))

/* Pull in stuff for the build system */
#ifdef DO_DEPS_ONLY
# include <environment.h>
#endif

#endif	/* __COMMON_H_ */
