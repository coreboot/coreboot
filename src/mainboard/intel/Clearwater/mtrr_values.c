#include <cpu/p6/mtrr.h>

/* We want to cache memory as efficiently as possible.
 */
#define RAM MTRR_TYPE_WRBACK
/* We can't use Write Combining on a legacy frame buffer because
 * it is incompatible with EGA 16 color video modes...
 */
#define FB  MTRR_TYPE_UNCACHABLE
/* For areas that are supposed to cover roms it makes no
 * sense to cache writes.
 */
#define ROM MTRR_TYPE_WRPROT


unsigned char fixed_mtrr_values[][4] = {
	/* MTRRfix64K_00000_MSR, defines memory range from 0KB to 512 KB, each byte cover 64KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix16K_80000_MSR, defines memory range from 512KB to 640KB, each byte cover 16KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix16K_A0000_MSR, defines memory range from A0000 to C0000, each byte cover 16KB area */
	{FB,  FB,  FB,  FB},  {FB,  FB,  FB,  FB},

	/* MTRRfix4K_C0000_MSR, defines memory range from C0000 to C8000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_C8000_MSR, defines memory range from C8000 to D0000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_D0000_MSR, defines memory range from D0000 to D8000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_D8000_MSR, defines memory range from D8000 to E0000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_E0000_MSR, defines memory range from E0000 to E8000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_E8000_MSR, defines memory range from E8000 to F0000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_F0000_MSR, defines memory range from F0000 to F8000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},

	/* MTRRfix4K_F8000_MSR, defines memory range from F8000 to 100000, each byte cover 4KB area */
	{RAM, RAM, RAM, RAM}, {RAM, RAM, RAM, RAM},
};
