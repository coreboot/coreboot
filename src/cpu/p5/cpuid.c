
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <pciconf.h>
#include <subr.h>
#include <printk.h>
#include <cpu/p5/cpuid.h>
#ifdef i586
#include <cpu/p6/msr.h>
#endif


int intel_mtrr_check(void)
{
#ifdef i686
	/* Only Pentium Pro and later have MTRR */
	unsigned long low, high;

	DBG("\nMTRR check\n");

	rdmsr(0x2ff, low, high);
	low = low >> 10;

	DBG("Fixed MTRRs   : ");
	if (low & 0x01)
		DBG("Enabled\n");
	else
		DBG("Disabled\n");

	DBG("Variable MTRRs: ");
	if (low & 0x02)
		DBG("Enabled\n");
	else
		DBG("Disabled\n");

	DBG("\n");

	post_code(0x93);
	return ((int) low);
#else /* !i686 */
	return 0;
#endif /* i686 */
}

void intel_display_cpuid(void)
{
	int op, eax, ebx, ecx, edx;
	int max_op;

	max_op = 0;

	DBG("\n");

	for (op = 0; op <= max_op; op++) {
		intel_cpuid(op, &eax, &ebx, &ecx, &edx);

		if (0 == op) {
			max_op = eax;
			DBG("Max cpuid index    : %d\n", eax);
			DBG("Vendor ID          : "
			    "%c%c%c%c%c%c%c%c%c%c%c%c\n",
			    ebx, ebx >> 8, ebx >> 16, ebx >> 24, edx,
			    edx >> 8, edx >> 16, edx >> 24, ecx, ecx >> 8,
			    ecx >> 16, ecx >> 24);
		} else if (1 == op) {
			DBG("Processor Type     : 0x%02x\n",
			    (eax >> 12) & 0x03);
			DBG("Processor Family   : 0x%02x\n",
			    (eax >> 8) & 0x0f);
			DBG("Processor Model    : 0x%02x\n",
			    (eax >> 4) & 0x0f);
			DBG("Processor Mask     : 0x%02x\n",
			    (ecx >> 0) & 0x0f);
			DBG("Processor Stepping : 0x%02x\n",
			    (eax >> 0) & 0x0f);
			DBG("Feature flags      : 0x%08x\n", edx);
		} else if (2 == op) {
			int desc[4];
			int ii;
			int _desc;

			DBG("\n");

			DBG("Cache/TLB descriptor values: %d "
			    "reads required\n", eax & 0xff);

			desc[0] = eax;
			desc[1] = ebx;
			desc[2] = ecx;
			desc[3] = edx;

			for (ii = 1; ii < 16; ii++) {
				if (desc[ii >> 2] & 0x80000000) {
					DBG("reserved descriptor\n");
					continue;
				}

				_desc =
				    ((desc[ii >> 2]) >> ((ii & 0x3) << 3))
				    & 0xff;
				DBG("Desc 0x%02x : ", _desc);

				switch (_desc) {
				case 0x00:
					DBG("null\n");
					break;

				case 0x01:
					DBG("Instr TLB: "
					    "4KB pages, "
					    "4-way set assoc, "
					    "32 entries\n");
					break;

				case 0x02:
					DBG("Instr TLB: "
					    "4MB pages, "
					    "fully assoc, " "2 entries\n");
					break;

				case 0x03:
					DBG("Data TLB: "
					    "4KB pages, "
					    "4-way set assoc, "
					    "64 entries\n");
					break;

				case 0x04:
					DBG("Data TLB: "
					    "4MB pages, "
					    "4-way set assoc, "
					    "8 entries\n");
					break;

				case 0x06:
					DBG("Inst cache: "
					    "8K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x08:
					DBG("Inst cache: "
					    "16K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x0a:
					DBG("Data cache: "
					    "8K bytes, "
					    "2-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x0c:
					DBG("Data cache: "
					    "16K bytes, "
					    "2-way or 4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x40:
					DBG("No L2 cache\n");
					break;

				case 0x41:
					DBG("L2 Unified cache: "
					    "128K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x42:
					DBG("L2 Unified cache: "
					    "256K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x43:
					DBG("L2 Unified cache: "
					    "512K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x44:
					DBG("L2 Unified cache: "
					    "1M byte, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x45:
					DBG("L2 Unified cache: "
					    "2M byte, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x82:
					DBG("L2 Unified cache: "
					    "256K bytes, "
					    "8-way set assoc, "
					    "32 byte line size\n");
					break;

				default:
					DBG("UNKNOWN\n");
				}
			}
			DBG("\n");
		} else {
			DBG("op: 0x%02x  eax:0x%08x  "
			    "ebx:0x%08x  ecx:0x%08x  edx:0x%08x\n",
			    op, eax, ebx, ecx, edx);
		}
	}

	DBG("\n");
	post_code(0x92);
}
