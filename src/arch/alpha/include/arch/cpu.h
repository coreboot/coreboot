#ifndef ALPHA_CPU_H
#define ALPHA_CPU_H

/* Alpha Logical Register Definitions
 * =========================================================
 */

/* Return value */
#define v0 $0
/* temporaries */
#define t0 $1
#define t1 $2
#define t2 $3
#define t3 $4
#define t4 $5
#define t5 $6
#define t6 $7
#define t7 $8
#define t8 $22
#define t9 $23
#define t10 $24
#define t11 $25
#define t12 $27
/* Saved registers */
#define s0 $9
#define s1 $10
#define s2 $11
#define s3 $12
#define s4 $13
#define s5 $14
#define s6 $15
/* Frame pointer */
#define fp $15
/* Argument registers */
#define a0 $16
#define a1 $17	
#define a2 $18	
#define a3 $19	
#define a4 $20	
#define a5 $21
/* return address */
#define ra $26
/* Procedure value */
#define pv $27
/* Assember temporary */
#define at $28
/* Global pointer */
#define gp $29
/* Stack Pointer */
#define sp $30
/* zero */
#define zero $31

#if USE_CPU_EV4
#include <cpu/ev4/dc21064.h>
#elif USE_CPU_EV5
#include <cpu/ev5/dc21164.h>
#elif USE_CPU_EV6
#include <cpu/ev6/dc21264.h>
#else
#error Uknown alpha cpu type
#endif

#endif /* ALPHA_CPU_H */
