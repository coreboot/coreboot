/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#ifndef _BSP_H
#define _BSP_H

/* This function is called very early on, and may not have a functioning
   stack. It should be careful not to touch memory until it has enabled
   memory accesses. */
void bsp_init_mp107(void);

/* Called early to initialize i/o for logging */
void bsp_init_io(void);

/* Called to relocate data */
void bsp_relocate(void);

/* Called before the relocation jump - data has been relocated. */
unsigned bsp_init_memory(unsigned offset);

/* This function is called immediately after relocation. Do whatever
   is necessary to perform printf. */
void bsp_init_post_reloc(unsigned memory);

/* Called after the first print to the outside world */
void bsp_init_post_hello(void);

/* Called if things have failed very, very badly. */
void bsp_indicate_dead(void);

/* Prints out a BSP description */
void bsp_identify(void);

/* BSP primary clock */
unsigned bsp_clock_speed(void);

/* Memory regions - all of memory runs from memory_base to top.  */
extern unsigned long memory_base;
extern unsigned long memory_top;
extern unsigned long physical_memory_size;

#endif
