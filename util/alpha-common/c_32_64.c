#ifdef	CROSS_COMPILE_32_TO_64_BITS

#include "c_32_64.h"

int64	zero = {0,0};

/* taken from my simulator */

/* add an unsigned int32 to a int64 */
#define add_l_ui(r,l,i)							\
    {									\
	register unsigned int32 lp, hp, carry;				\
	lp = (low32(l) & 0xffff) + ((i) & 0xffff);			\
	carry = lp >> 16;						\
	hp = (((unsigned)low32(l)) >> 16) + (((unsigned)i) >> 16) + carry;\
	carry = hp >> 16;						\
	low32(r) = (hp << 16) | (lp & 0xffff);				\
	high32(r) = high32(l) + carry;					\
    }

/* add to int64s */
#define add_l_l(r,a,b)							\
    {									\
	register unsigned int32 lp, hp, carry;				\
	lp = (low32(a) & 0xffff) + (low32(b) & 0xffff);			\
	carry = lp >> 16;						\
	hp = (((unsigned)low32(a))>>16) + (((unsigned)low32(b))>>16) + carry;\
	carry = hp >> 16;						\
	low32(r) = (hp << 16) | (lp & 0xffff);				\
	/* now the hi part */						\
	lp = (high32(a) & 0xffff) + (high32(b) & 0xffff) + carry;	\
	carry = lp >> 16;						\
	hp = (((unsigned)high32(a))>>16) + (((unsigned)high32(b))>>16) + carry;\
	high32(r) = (hp << 16) | (lp & 0xffff);				\
    }

/* add an int32 to a int64 */
#define add_l_i(r,l,i)							\
    {									\
	if (i == 0) {							\
	    low32(r) = low32(l); high32(r) = high32(l);			\
	} else if (i > 0) {						\
	    add_l_ui(r,l,i);						\
	} else {							\
	    int64 m;							\
	    low32(m) = i; high32(m) = -1;				\
	    add_l_l(r,l,m);						\
	}								\
    }

/* sub two int64s */
#define sub_l_l(r,a,b)							\
    {									\
	register unsigned int32 lp, hp, carry;				\
	lp = (low32(a) & 0xffff) + ((~low32(b)) & 0xffff) + 1;		\
	carry = lp >> 16;						\
	hp = (((unsigned)low32(a))>>16) + ((~(unsigned)low32(b))>>16) + carry;\
	carry = hp >> 16;						\
	low32(r) = (hp << 16) | (lp & 0xffff);				\
	/* now the hi part */						\
	lp = (high32(a) & 0xffff) + ((~high32(b)) & 0xffff) + carry;		\
	carry = lp >> 16;						\
	hp = (((unsigned)high32(a))>>16) + ((~(unsigned)high32(b))>>16) + carry;\
	high32(r) = (hp << 16) | (lp & 0xffff);				\
    }

int64 plus(a,b)
	int64 a,b;
{
	int64 c;
	add_l_l(c,a,b);
	return c;
}

int64 plus_a_32(a,b)
	int64 a;
	int32 b;
{
	int64 c;
	add_l_i(c,a,b);
	return c;
}

int64 minus(a,b)
	int64 a,b;
{
	int64 c;
	sub_l_l(c,a,b);
	return c;
}

#endif	/* CROSS_COMPILE_32_TO_64_BITS */

