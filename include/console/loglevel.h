/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/
#ifndef LOGLEVEL_H
#define LOGLEVEL_H

/* Safe for inclusion in assembly */

#ifndef MAXIMUM_CONSOLE_LOGLEVEL
#define MAXIMUM_CONSOLE_LOGLEVEL 8
#endif

#ifndef DEFAULT_CONSOLE_LOGLEVEL
#define DEFAULT_CONSOLE_LOGLEVEL 8 /* anything MORE serious than BIOS_SPEW */
#endif

#ifndef ASM_CONSOLE_LOGLEVEL
#if (DEFAULT_CONSOLE_LOGLEVEL <= MAXIMUM_CONSOLE_LOGLEVEL)
#define ASM_CONSOLE_LOGLEVEL DEFAULT_CONSOLE_LOGLEVEL
#else
#define ASM_CONSOLE_LOGLEVEL MAXIMUM_CONSOLE_LOGLEVEL
#endif
#endif

#define BIOS_EMERG      0   /* system is unusable                   */
#define BIOS_ALERT      1   /* action must be taken immediately     */
#define BIOS_CRIT       2   /* critical conditions                  */
#define BIOS_ERR        3   /* error conditions                     */
#define BIOS_WARNING    4   /* warning conditions                   */
#define BIOS_NOTICE     5   /* normal but significant condition     */
#define BIOS_INFO       6   /* informational                        */
#define BIOS_DEBUG      7   /* debug-level messages                 */
#define BIOS_SPEW       8   /* Way too many details                 */

#endif /* LOGLEVEL_H */
