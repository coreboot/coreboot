#include <pciconf.h>
#include <subr.h>
#include <printk.h>
#include <cpu/p5/cpuid.h>
#ifdef i586
#include <cpu/p6/msr.h>
#endif

#ifdef i586
int intel_mtrr_check(void)
{
	unsigned long low, high;

	printk(KERN_INFO "\nMTRR check\n");

	rdmsr(0x2ff, low, high);
	low = low >> 10;

	printk(KERN_INFO "Fixed MTRRs   : ");
	if (low & 0x01)
		printk(KERN_INFO "Enabled\n");
	else
		printk(KERN_INFO "Disabled\n");

	printk(KERN_INFO "Variable MTRRs: ");
	if (low & 0x02)
		printk(KERN_INFO "Enabled\n");
	else
		printk(KERN_INFO "Disabled\n");

	printk(KERN_INFO "\n");

	return ((int) low);
}
#endif

void intel_display_cpuid(void)
{
	int op, eax, ebx, ecx, edx;
	int max_op;

	max_op = 0;

	printk(KERN_INFO "\n");

	for (op = 0; op <= max_op; op++) {
		intel_cpuid(op, &eax, &ebx, &ecx, &edx);

		if (0 == op) {
			max_op = eax;
			printk(KERN_INFO "Max cpuid index    : %d\n", eax);
			printk(KERN_INFO "Vendor ID          : "
			       "%c%c%c%c%c%c%c%c%c%c%c%c\n",
			       ebx, ebx >> 8, ebx >> 16, ebx >> 24, edx,
			       edx >> 8, edx >> 16, edx >> 24, ecx, ecx >> 8,
			       ecx >> 16, ecx >> 24);
		} else if (1 == op) {
			printk(KERN_INFO "Processor Type     : 0x%02x\n",
			       (eax >> 12) & 0x03);
			printk(KERN_INFO "Processor Family   : 0x%02x\n",
			       (eax >> 8) & 0x0f);
			printk(KERN_INFO "Processor Model    : 0x%02x\n",
			       (eax >> 4) & 0x0f);
			printk(KERN_INFO "Processor Mask     : 0x%02x\n",
			       (ecx >> 0) & 0x0f);
			printk(KERN_INFO "Processor Stepping : 0x%02x\n",
			       (eax >> 0) & 0x0f);
			printk(KERN_INFO "Feature flags      : 0x%08x\n", edx);
		} else if (2 == op) {
			int desc[4];
			int ii;
			int _desc;

			printk(KERN_INFO "\n");

			printk(KERN_INFO "Cache/TLB descriptor values: %d "
			       "reads required\n", eax & 0xff);

			desc[0] = eax;
			desc[1] = ebx;
			desc[2] = ecx;
			desc[3] = edx;

			for (ii = 1; ii < 16; ii++) {
				if (desc[ii >> 2] & 0x80000000) {
					printk(KERN_INFO
					       "reserved descriptor\n");
					continue;
				}

				_desc =
				  ((desc[ii>>2]) >> ((ii & 0x3) << 3)) & 0xff;
				printk(KERN_INFO "Desc 0x%02x : ", _desc);

				switch (_desc) {
				case 0x00:
					printk(KERN_INFO "null\n");
					break;

				case 0x01:
					printk(KERN_INFO "Instr TLB: "
					       "4KB pages, "
					       "4-way set assoc, "
					       "32 entries\n");
					break;

				case 0x02:
					printk(KERN_INFO "Instr TLB: "
					       "4MB pages, "
					       "fully assoc, "
					       "2 entries\n");
					break;

				case 0x03:
					printk(KERN_INFO "Data TLB: "
					       "4KB pages, "
					       "4-way set assoc, "
					       "64 entries\n");
					break;

				case 0x04:
					printk(KERN_INFO "Data TLB: "
					       "4MB pages, "
					       "4-way set assoc, "
					       "8 entries\n");
					break;

				case 0x06:
					printk(KERN_INFO "Inst cache: "
					       "8K bytes, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x08:
					printk(KERN_INFO "Inst cache: "
					       "16K bytes, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x0a:
					printk(KERN_INFO "Data cache: "
					       "8K bytes, "
					       "2-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x0c:
					printk(KERN_INFO "Data cache: "
					       "16K bytes, "
					       "2-way or 4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x40:
					printk(KERN_INFO "No L2 cache\n");
					break;

				case 0x41:
					printk(KERN_INFO "L2 Unified cache: "
					       "128K bytes, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x42:
					printk(KERN_INFO "L2 Unified cache: "
					       "256K bytes, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x43:
					printk(KERN_INFO "L2 Unified cache: "
					       "512K bytes, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x44:
					printk(KERN_INFO "L2 Unified cache: "
					       "1M byte, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x45:
					printk(KERN_INFO "L2 Unified cache: "
					       "2M byte, "
					       "4-way set assoc, "
					       "32 byte line size\n");
					break;

				case 0x82:
					printk(KERN_INFO "L2 Unified cache: "
					       "256K bytes, "
					       "8-way set assoc, "
					       "32 byte line size\n");
					break;

				default:
					printk(KERN_INFO "UNKNOWN\n");
				}
			}
			printk(KERN_INFO "\n");
		} else {
			printk(KERN_INFO "op: 0x%02x  eax:0x%08x  "
			       "ebx:0x%08x  ecx:0x%08x  edx:0x%08x\n",
			       op, eax, ebx, ecx, edx);
		}
	}

	printk(KERN_INFO "\n");
}
