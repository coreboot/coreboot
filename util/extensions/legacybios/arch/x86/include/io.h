/* I/O implementation
 *
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 * 
 * this code is inspired by the linux kernel
 *
 */

#ifndef IO_H
#define IO_H

#include <types.h>

#define __EXTERN__ extern

__EXTERN__ inline u8 inb(u32 port)
{
	u8 result;
	__asm__ __volatile__ ( "inb %w1,%0" :"=a" (result) :"Nd" (port));
	return result;
}

__EXTERN__ inline u16 inw(u32 port)
{
	u16 result;
	__asm__ __volatile__ ( "inw %w1,%0" :"=a" (result) :"Nd" (port));
	return result;
}

__EXTERN__ inline u32 inl(u32 port)
{
	u32 result;
	__asm__ __volatile__ ( "inl %w1,%0" :"=a" (result) :"Nd" (port));
	return result;
}

__EXTERN__ inline void outb (u32 port, u8 value)
{
	__asm__ __volatile__ ( "outb %b0,%w1" : :"a" (value), "Nd" (port));
}

__EXTERN__ inline void outw (u32 port, u16 value)
{
	__asm__ __volatile__ ( "outw %w0,%w1" : :"a" (value), "Nd" (port));
}

__EXTERN__ inline void outl (u32 port, u32 value)
{
	__asm__ __volatile__ ( "outl %0,%w1" : :"a" (value), "Nd" (port));
}

#endif /* IO_H */
