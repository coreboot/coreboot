#include <console/console.h>
#include <cpu/p5/cpuid.h>
#ifdef i586
#include <cpu/p6/msr.h>
#endif


int mtrr_check(void)
{
#ifdef i686
	/* Only Pentium Pro and later have MTRR */
	msr_t msr;
	printk_debug("\nMTRR check\n");

	msr = rdmsr(0x2ff);
	msr.lo >>= 10;

	printk_debug("Fixed MTRRs   : ");
	if (msr.lo & 0x01)
		printk_debug("Enabled\n");
	else
		printk_debug("Disabled\n");

	printk_debug("Variable MTRRs: ");
	if (msr.lo & 0x02)
		printk_debug("Enabled\n");
	else
		printk_debug("Disabled\n");

	printk_debug("\n");

	post_code(0x93);
	return ((int) msr.lo);
#else /* !i686 */
	return 0;
#endif /* i686 */
}

void display_cpuid(void)
{
	int op, eax, ebx, ecx, edx;
	int max_op;

	max_op = 0;

	printk_debug("\n");

	for (op = 0; op <= max_op; op++) {
		cpuid(op, &eax, &ebx, &ecx, &edx);

		if (0 == op) {
			max_op = eax;
			printk_debug("Max cpuid index    : %d\n", eax);
			printk_debug("Vendor ID          : "
			    "%c%c%c%c%c%c%c%c%c%c%c%c\n",
			    ebx, ebx >> 8, ebx >> 16, ebx >> 24, edx,
			    edx >> 8, edx >> 16, edx >> 24, ecx, ecx >> 8,
			    ecx >> 16, ecx >> 24);
		} else if (1 == op) {
			printk_debug("Processor Type     : 0x%02x\n",
			    (eax >> 12) & 0x03);
			printk_debug("Processor Family   : 0x%02x\n",
			    (eax >> 8) & 0x0f);
			printk_debug("Processor Model    : 0x%02x\n",
			    (eax >> 4) & 0x0f);
			printk_debug("Processor Mask     : 0x%02x\n",
			    (ecx >> 0) & 0x0f);
			printk_debug("Processor Stepping : 0x%02x\n",
			    (eax >> 0) & 0x0f);
			printk_debug("Feature flags      : 0x%08x\n", edx);
		} else if (2 == op) {
			int desc[4];
			int ii;
			int _desc;

			printk_debug("\n");

			printk_debug("Cache/TLB descriptor values: %d "
			    "reads required\n", eax & 0xff);

			desc[0] = eax;
			desc[1] = ebx;
			desc[2] = ecx;
			desc[3] = edx;

			for (ii = 1; ii < 16; ii++) {
				if (desc[ii >> 2] & 0x80000000) {
					printk_debug("reserved descriptor\n");
					continue;
				}

				_desc =
				    ((desc[ii >> 2]) >> ((ii & 0x3) << 3))
				    & 0xff;
				printk_debug("Desc 0x%02x : ", _desc);

				switch (_desc) {
				case 0x00:
					printk_debug("null\n");
					break;

				case 0x01:
					printk_debug("Instr TLB: "
					    "4KB pages, "
					    "4-way set assoc, "
					    "32 entries\n");
					break;

				case 0x02:
					printk_debug("Instr TLB: "
					    "4MB pages, "
					    "fully assoc, " "2 entries\n");
					break;

				case 0x03:
					printk_debug("Data TLB: "
					    "4KB pages, "
					    "4-way set assoc, "
					    "64 entries\n");
					break;

				case 0x04:
					printk_debug("Data TLB: "
					    "4MB pages, "
					    "4-way set assoc, "
					    "8 entries\n");
					break;

				case 0x06:
					printk_debug("Inst cache: "
					    "8K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x08:
					printk_debug("Inst cache: "
					    "16K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x0a:
					printk_debug("Data cache: "
					    "8K bytes, "
					    "2-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x0c:
					printk_debug("Data cache: "
					    "16K bytes, "
					    "2-way or 4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x40:
					printk_debug("No L2 cache\n");
					break;

				case 0x41:
					printk_debug("L2 Unified cache: "
					    "128K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x42:
					printk_debug("L2 Unified cache: "
					    "256K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x43:
					printk_debug("L2 Unified cache: "
					    "512K bytes, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x44:
					printk_debug("L2 Unified cache: "
					    "1M byte, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x45:
					printk_debug("L2 Unified cache: "
					    "2M byte, "
					    "4-way set assoc, "
					    "32 byte line size\n");
					break;

				case 0x82:
					printk_debug("L2 Unified cache: "
					    "256K bytes, "
					    "8-way set assoc, "
					    "32 byte line size\n");
					break;

				default:
					printk_debug("UNKNOWN\n");
				}
			}
			printk_debug("\n");
		} else {
			printk_debug("op: 0x%02x  eax:0x%08x  "
			    "ebx:0x%08x  ecx:0x%08x  edx:0x%08x\n",
			    op, eax, ebx, ecx, edx);
		}
	}

	printk_debug("\n");
	post_code(0x92);
}
