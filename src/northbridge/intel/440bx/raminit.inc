
#define RAM_256M

#if 0
/* CAS latency 2 */
#define CAS_NB 0x17
#define CAS_RAM 0x150
#elif 1
/* CAS latency 3 */
#define CAS_NB 0x03
#define CAS_RAM 0x1d0
#endif

/* 1 or 2 dimms */

#ifdef RAM_256M

#define DRB 0x20
#define DIMM2_READ2 mov 0x08002000, %eax
#define DIMM2_READ mov 0x08000000, %eax
#define DIMM2_MRS mov 0x08000000+CAS_RAM, %eax
#define RPS 0x22
#define PGPOL 0x26
#define MBSC 0x01

#define DRB2 0x10
#define RPS2 0x02
#define PGPOL2 0x23
#define MBSC2 0x00

#else

#define DRB 0x10
#define DIMM2_READ2
#define DIMM2_READ
#define DIMM2_MRS
#define RPS 0x02
#define PGPOL 0x23
#define MBSC 0x00

#endif
	
/* this sets up the intel 440BX SDRAM. */

	invd
	mov %cr0, %eax
	or $0x60000000, %eax    // cache disable bits on
	mov %eax, %cr0

	CS_WRITE($0x77, $0x01)  /* SDRAMC */

	CS_WRITE($0x57, $0x08)  /* DRAMC */

	/* PAM - Programmable Attribute Map Registers */

	CS_WRITE($0x59, $0)
	CS_WRITE($0x5a, $0)
	CS_WRITE($0x5b, $0)
	CS_WRITE($0x5c, $0)
	CS_WRITE($0x5d, $0)
	CS_WRITE($0x5e, $0)
	CS_WRITE($0x5f, $0)

	/* DRB - DRAM Row Boundry Registers */

	CS_WRITE($0x60, $0x10)   /* 128 MB on slot 0 front */
	CS_WRITE($0x61, $0x10)   /*   0 MB on slot 0 back  */
	CS_WRITE($0x62, $DRB )   /* 128 MB on slot 1 front */
	CS_WRITE($0x63, $DRB )   /*   0 MB on slot 1 back  */
	CS_WRITE($0x64, $DRB )   /*   0 MB on slot 2 front */
	CS_WRITE($0x65, $DRB )   /*   0 MB on slot 2 back  */
	CS_WRITE($0x66, $DRB )   /*   0 MB on slot 3 front */
	CS_WRITE($0x67, $DRB )   /*   0 MB on slot 3 back  */
	
	/* MBSC - Memory buffer strength control */

	CS_WRITE($0x69, $0x00)
	CS_WRITE($0x6a, $0x40)
	CS_WRITE($0x6b, $MBSC)
	CS_WRITE($0x6c, $0x00)
	CS_WRITE($0x6d, $0x8a)
	CS_WRITE($0x6e, $0x00)
	
	/* RPS - Row Page Size Register */

	CS_WRITE($0x74, $RPS)
	CS_WRITE($0x75, $0x00)

	/* PGPOL - Paging Policy Register */

	CS_WRITE($0x78, $PGPOL)
	CS_WRITE($0x79, $0xff)

	/* MBFS - Memory Buffer Frequency Select Register */

	CS_WRITE($0xca, $0xff)
	CS_WRITE($0xcb, $0xff)
	CS_WRITE($0xcc, $0x61)

	/* NOP */

	CS_WRITE($0x76, $0x20+CAS_NB) /* SDRAMC */

	mov 0x00000000, %eax
	DIMM2_READ

	/* delay 200 us (100us/loop) */
        mov $2, %ecx
1:
	nop
	loop 1b

	/* Precharge all */

	CS_WRITE($0x76, $0x40+CAS_NB)
	mov 0x00002000, %eax
	DIMM2_READ2

	/* eight CBRs */

	CS_WRITE($0x76, $0x80+CAS_NB)

	/* delay at least 100 ns between reads. (ISA access/no cache, slow) */

	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ
	mov 0x00000000, %eax
	DIMM2_READ

	/* mode register set*/

	CS_WRITE($0x76, $0x60+CAS_NB)

	mov 0x00000000+CAS_RAM, %eax
	DIMM2_MRS

	/* normal operation */
	CS_WRITE($0x76, $0x00+CAS_NB)

	CS_WRITE($0x57, $0x09)  /* DRAMC */


#ifdef RAM_256M
	mov $0x40, %ecx
	mov $0x8000000, %ebx
1:
	mov %ebx, (%ebx)
	add $4, %ebx
	loop 1b

	mov $0x40, %ecx
	mov $0x8000000, %ebx
1:
	cmp %ebx, (%ebx)
	jne 1f
	add $4, %ebx
	loop 1b
	jmp 2f
1:
	CS_WRITE($0x62, $DRB2 )   /*   0 MB on slot 1 front */
	CS_WRITE($0x63, $DRB2 )   /*   0 MB on slot 1 back  */
	CS_WRITE($0x64, $DRB2 )   /*   0 MB on slot 2 front */
	CS_WRITE($0x65, $DRB2 )   /*   0 MB on slot 2 back  */
	CS_WRITE($0x66, $DRB2 )   /*   0 MB on slot 3 front */
	CS_WRITE($0x67, $DRB2 )   /*   0 MB on slot 3 back  */
	
	CS_WRITE($0x6b, $MBSC2)
	CS_WRITE($0x74, $RPS2)
	CS_WRITE($0x78, $PGPOL2)
2:
#endif
