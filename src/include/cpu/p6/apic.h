#ifndef APIC_H
#define APIC_H

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_BOOTSTRAP_PROCESSOR (1 << 8)
#define APIC_BASE_MSR_ENABLE (1 << 11)
#define APIC_BASE_MSR_ADDR_MASK 0xFFFFF000

#define APIC_DEFAULT_BASE 0xfee00000

#define APIC_ID		0x020
#define APIC_LVR	0x030
#define APIC_ARBID	0x090
#define	APIC_RRR	0x0C0
#define APIC_SVR	0x0f0
#define APIC_SPIV	0x0f0
#define 	APIC_SPIV_ENABLE  0x100
#define APIC_ESR	0x280
#define		APIC_ESR_SEND_CS	0x00001
#define		APIC_ESR_RECV_CS	0x00002
#define		APIC_ESR_SEND_ACC	0x00004
#define		APIC_ESR_RECV_ACC	0x00008
#define		APIC_ESR_SENDILL	0x00020
#define		APIC_ESR_RECVILL	0x00040
#define		APIC_ESR_ILLREGA	0x00080
#define APIC_ICR 	0x300
#define		APIC_DEST_SELF		0x40000
#define		APIC_DEST_ALLINC	0x80000
#define		APIC_DEST_ALLBUT	0xC0000
#define		APIC_ICR_RR_MASK	0x30000
#define		APIC_ICR_RR_INVALID	0x00000
#define		APIC_ICR_RR_INPROG	0x10000
#define		APIC_ICR_RR_VALID	0x20000
#define		APIC_INT_LEVELTRIG	0x08000
#define		APIC_INT_ASSERT		0x04000
#define		APIC_ICR_BUSY		0x01000
#define		APIC_DEST_LOGICAL	0x00800
#define		APIC_DM_FIXED		0x00000
#define		APIC_DM_LOWEST		0x00100
#define		APIC_DM_SMI		0x00200
#define		APIC_DM_REMRD		0x00300
#define		APIC_DM_NMI		0x00400
#define		APIC_DM_INIT		0x00500
#define		APIC_DM_STARTUP		0x00600
#define		APIC_DM_EXTINT		0x00700
#define		APIC_VECTOR_MASK	0x000FF
#define APIC_ICR2	0x310
#define		GET_APIC_DEST_FIELD(x)	(((x)>>24)&0xFF)
#define		SET_APIC_DEST_FIELD(x)	((x)<<24)
#define APIC_LVTT	0x320
#define APIC_LVTPC	0x340
#define APIC_LVT0	0x350
#define		APIC_LVT_TIMER_BASE_MASK	(0x3<<18)
#define		GET_APIC_TIMER_BASE(x)		(((x)>>18)&0x3)
#define		SET_APIC_TIMER_BASE(x)		(((x)<<18))
#define		APIC_TIMER_BASE_CLKIN		0x0
#define		APIC_TIMER_BASE_TMBASE		0x1
#define		APIC_TIMER_BASE_DIV		0x2
#define		APIC_LVT_TIMER_PERIODIC		(1<<17)
#define		APIC_LVT_MASKED			(1<<16)
#define		APIC_LVT_LEVEL_TRIGGER		(1<<15)
#define		APIC_LVT_REMOTE_IRR		(1<<14)
#define		APIC_INPUT_POLARITY		(1<<13)
#define		APIC_SEND_PENDING		(1<<12)
#define		APIC_LVT_RESERVED_1		(1<<11)
#define		APIC_DELIVERY_MODE_MASK		(7<<8)
#define		APIC_DELIVERY_MODE_FIXED	(0<<8)
#define		APIC_DELIVERY_MODE_NMI		(4<<8)
#define		APIC_DELIVERY_MODE_EXTINT	(7<<8)
#define		GET_APIC_DELIVERY_MODE(x)	(((x)>>8)&0x7)
#define		SET_APIC_DELIVERY_MODE(x,y)	(((x)&~0x700)|((y)<<8))
#define			APIC_MODE_FIXED		0x0
#define			APIC_MODE_NMI		0x4
#define			APIC_MODE_EXINT		0x7
#define APIC_LVT1	0x360
#define APIC_LVTERR	0x370


#if !defined(ASSEMBLY)

#include <printk.h>


#define xchg(ptr,v) ((__typeof__(*(ptr)))__xchg((unsigned long)(v),(ptr),sizeof(*(ptr))))

struct __xchg_dummy { unsigned long a[100]; };
#define __xg(x) ((struct __xchg_dummy *)(x))

/*
 * Note: no "lock" prefix even on SMP: xchg always implies lock anyway
 * Note 2: xchg has side effect, so that attribute volatile is necessary,
 *	  but generally the primitive is invalid, *ptr is output argument. --ANK
 */
static inline unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
	switch (size) {
		case 1:
			__asm__ __volatile__("xchgb %b0,%1"
				:"=q" (x)
				:"m" (*__xg(ptr)), "0" (x)
				:"memory");
			break;
		case 2:
			__asm__ __volatile__("xchgw %w0,%1"
				:"=r" (x)
				:"m" (*__xg(ptr)), "0" (x)
				:"memory");
			break;
		case 4:
			__asm__ __volatile__("xchgl %0,%1"
				:"=r" (x)
				:"m" (*__xg(ptr)), "0" (x)
				:"memory");
			break;
	}
	return x;
}


static inline unsigned long apic_read(unsigned long reg)
{
	return *((volatile unsigned long *)(APIC_DEFAULT_BASE+reg));
}

extern inline void apic_write_atomic(unsigned long reg, unsigned long v)
{
	xchg((volatile unsigned long *)(APIC_DEFAULT_BASE+reg), v);
}

static inline void apic_write(unsigned long reg, unsigned long v)
{
	*((volatile unsigned long *)(APIC_DEFAULT_BASE+reg)) = v;
}

static inline void apic_wait_icr_idle(void)
{
	do { } while ( apic_read( APIC_ICR ) & APIC_ICR_BUSY );
}

#ifdef CONFIG_X86_GOOD_APIC
# define FORCE_READ_AROUND_WRITE 0
# define apic_read_around(x)
# define apic_write_around(x,y) apic_write((x),(y))
#else
# define FORCE_READ_AROUND_WRITE 1
# define apic_read_around(x) apic_read(x)
# define apic_write_around(x,y) apic_write_atomic((x),(y))
#endif

static inline unsigned long apic_remote_read(int apicid, int reg)
{
	int timeout;
	unsigned long status, result;
	apic_wait_icr_idle();
	apic_write_around(APIC_ICR2, SET_APIC_DEST_FIELD(apicid));
	apic_write_around(APIC_ICR, APIC_DM_REMRD | (reg >> 4));
	timeout = 0;
	do {
#if 0
		udelay(100);
#endif
		status = apic_read(APIC_ICR) & APIC_ICR_RR_MASK;
	} while (status == APIC_ICR_RR_INPROG && timeout++ < 1000);

	result = -1;
	if (status == APIC_ICR_RR_VALID) {
		result = apic_read(APIC_RRR);
	}
	else {
		printk_err("remote apic read failed\n");
	}
	return result;
}
#endif /* ASSEMBLY */

#endif /* APIC_H */
