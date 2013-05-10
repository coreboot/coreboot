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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __COMMON_H_
#define __COMMON_H_	1

#ifndef __ASSEMBLER__		/* put C only stuff in this section */

#include <types.h>

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

/*
 * Return the time since boot in microseconds, This is needed for bootstage
 * and should be defined in CPU- or board-specific code. If undefined then
 * millisecond resolution will be used (the standard get_timer()).
 */
unsigned long timer_get_boot_us(void);

/*
 * Return the current value of a monotonically increasing microsecond timer.
 * Granularity may be larger than 1us if hardware does not support this.
 */
unsigned long timer_get_us(void);

/*
 * General Purpose Utilities
 */
#if 0
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

int	init_timer(void);	/* FIXME(dhendrix): used to be timer_init() */
int	cpu_init(void);

/* */
unsigned long long initdram (int);
int	display_options (void);
void	print_size(unsigned long long, const char *);
int	print_buffer (unsigned long addr, void* data, unsigned int width, unsigned int count, unsigned int linelen);

/* common/main.c */
void	main_loop	(void);
int	run_command	(const char *cmd, int flag);
int	readline	(const char *const prompt);
int	readline_into_buffer	(const char *const prompt, char * buffer);
int	parse_line (char *, char *[]);
void	init_cmd_timeout(void);
void	reset_cmd_timeout(void);

/* arch/$(ARCH)/lib/board.c */
void	board_init_f  (void);
int	checkboard    (void);
int	checkflash    (void);
int	checkdram     (void);
int	last_stage_init(void);
extern unsigned long monitor_flash_len;
int mac_read_from_eeprom(void);

#ifdef CONFIG_ARM
# include <asm/mach-types.h>
# include <asm/setup.h>
# include <asm/u-boot-arm.h>	/* ARM version to be fixed! */
#endif /* CONFIG_ARM */

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
int  eeprom_read  (unsigned dev_addr, unsigned offset, unsigned char *buffer, unsigned cnt);
int  eeprom_write (unsigned dev_addr, unsigned offset, unsigned char *buffer, unsigned cnt);

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
extern ssize_t spi_read	 (unsigned char *, int, unsigned char *, int);
extern ssize_t spi_write (unsigned char *, int, unsigned char *, int);
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
unsigned int	get_immr      (unsigned int);
#endif
unsigned int	get_pir	      (void);
#if defined(CONFIG_MPC5xxx)
unsigned int	get_svr       (void);
#endif
unsigned int	get_pvr	      (void);
unsigned int	get_svr	      (void);
unsigned int	rd_ic_cst     (void);
void	wr_ic_cst     (unsigned int);
void	wr_ic_adr     (unsigned int);
unsigned int	rd_dc_cst     (void);
void	wr_dc_cst     (unsigned int);
void	wr_dc_adr     (unsigned int);
int	icache_status (void);
void	icache_enable (unsigned long start, unsigned long size);
void	icache_disable(void);
int	dcache_status (void);
void	dcache_enable (unsigned long start, unsigned long size);
void	dcache_disable(void);
void	mmu_disable(void);
unsigned long	get_endaddr   (void);
void	trap_init     (unsigned long);
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
unsigned long	get_tbclk     (void);


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
unsigned long	get_OPB_freq (void);
unsigned long	get_PCI_freq (void);
#endif
#if defined(CONFIG_S3C24X0) || \
    defined(CONFIG_LH7A40X) || \
    defined(CONFIG_S3C6400) || \
    defined(CONFIG_EP93XX)
unsigned long	get_FCLK (void);
unsigned long	get_HCLK (void);
unsigned long	get_PCLK (void);
unsigned long	get_UCLK (void);
#endif
#if defined(CONFIG_LH7A40X)
unsigned long	get_PLLCLK (void);
#endif
#if defined CONFIG_INCA_IP
unsigned int	incaip_get_cpuclk (void);
#endif
#if defined(CONFIG_IMX)
unsigned long get_systemPLLCLK(void);
unsigned long get_FCLK(void);
unsigned long get_HCLK(void);
unsigned long get_BCLK(void);
unsigned long get_PERCLK1(void);
unsigned long get_PERCLK2(void);
unsigned long get_PERCLK3(void);
#endif
unsigned long	get_bus_freq  (unsigned long);
int get_serial_clock(void);

struct pt_regs;
/* $(CPU)/interrupts.c */
int	interrupt_init	   (void);
void	timer_interrupt	   (struct pt_regs *);
void	external_interrupt (struct pt_regs *);
void	irq_install_handler(int, interrupt_handler_t *, void *);
void	irq_free_handler   (int);
void	reset_timer	   (void);
unsigned long	get_timer	   (unsigned long base);
void	enable_interrupts  (void);
int	disable_interrupts (void);

/* $(CPU)/.../commproc.c */
int	dpram_init (void);
unsigned int	dpram_base(void);
unsigned int	dpram_base_align(unsigned int align);
unsigned int	dpram_alloc(unsigned int size);
unsigned int	dpram_alloc_align(unsigned int size,unsigned int align);
void	bootcount_store (unsigned long);
unsigned long	bootcount_load (void);
#define BOOTCOUNT_MAGIC		0xB001C041

/* $(CPU)/.../<eth> */
void mii_init (void);

/* $(CPU)/.../lcd.c */
unsigned long	lcd_setmem (unsigned long);

/* $(CPU)/.../video.c */
unsigned long	video_setmem (unsigned long);

/* arch/$(ARCH)/lib/cache.c */
unsigned long	dcache_get_line_size(void);
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
unsigned long	usec2ticks    (unsigned long usec);
unsigned long	ticks2usec    (unsigned long ticks);
int	init_timebase (void);

/* lib/qsort.c */
void qsort(void *base, size_t nmemb, size_t size,
	   int(*compar)(const void *, const void *));
int strcmp_compar(const void *, const void *);

/* lib/time.c */
void	udelay        (unsigned long);

/* Multicore arch functions */
#ifdef CONFIG_MP
int cpu_status(int nr);
int cpu_reset(int nr);
int cpu_disable(int nr);
int cpu_release(int nr, int argc, char * const argv[]);
#endif

#endif /* __ASSEMBLER__ */

/* Put only stuff here that the assembler can digest */

#define ROUND(a,b)		(((a) + (b) - 1) & ~((b) - 1))
#define DIV_ROUND(n,d)		(((n) + ((d)/2)) / (d))
#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define roundup(x, y)		((((x) + ((y) - 1)) / (y)) * (y))

//#define ALIGN(x,a)		__ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))

#endif	/* __COMMON_H_ */
