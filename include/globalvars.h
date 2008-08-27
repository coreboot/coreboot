/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef GLOBALVARS_H
#define GLOBALVARS_H

/* you need to include all files that might be referenced in the global variables struct */
#include <console.h>
#include <types.h>
/* the sys_info struct is architecture-dependent, with parameters controlled from mainboard.h in some cases */
#ifdef CONFIG_CPU_AMD_K8
#include <mainboard.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#endif

#ifdef CONFIG_CPU_AMD_GEODELX
#include <amd_geodelx.h>
#endif

/*
 * struct global_vars is managed entirely from C code. Keep in mind that there
 * is NO buffer at the end of the struct, so having zero-sized arrays at the
 * end or similar stuff for which the compiler can't determine the final size
 * will corrupt memory. If you don't try to be clever, everything will be fine.
 */
struct global_vars {
#ifdef CONFIG_CONSOLE_BUFFER
	struct printk_buffer *printk_buffer;
#endif
	unsigned int loglevel;
	/* these two values are of interest in many stages */
	u32 bist;
	u32 init_detected;
	struct sys_info sys_info;
};

#endif /* GLOBALVARS_H */
