#include <boot/uniform_boot.h>
#include <boot/elf.h>
#include <arch/boot/hwrpb.h>

/* FIXME remove the hardcodes
 * MAX_ASM, CMD_LINE, SYSNAME, CYCLE_FREQ, CPU, SYS_VARIATION, SYS_TYPE
 */

#ifndef CMD_LINE
#define CMD_LINE ""
#endif

/* Magic values.
 * Hard code these for the DS10 for now.
 */
#define HWRPB_SIGNATURE 	0x4857525042000000
#define HWRPB_SYS_TYPE		ST_DEC_TSUNAMI
#define HWRPB_SYS_VARIATION	(7 << 10)
#define HWRPB_CPU		EV6_CPU
#define HWRPB_CYCLE_FREQ	462962962
#define HWRPB_INTR_FREQ		(4096*1024)
#define HWRPB_SYSNAME		"linuxBIOS DS10"
#define HWRPB_COMMAND_LINE	CMD_LINE
#define HWRPB_MAX_ASN		255

#define OFFSET(x, y) ((unsigned long)(((char *)&(x)) - ((char *)&(y))))
#define ELEMENTS(x) (sizeof(x)/sizeof((x)[0]))
#define ADDR(x) ((unsigned long)&(x))

struct sysname_struct {
	unsigned long length;
	unsigned char name[30];
};

static struct boot_data {
	struct hwrpb_struct hwrpb;
	struct dsr_struct dsr;
	struct sysname_struct sysname;
	struct percpu_struct cpu[1];
	struct memdesc_struct mem;
	struct memclust_struct mem_cluster[2];
} boot_data = 
{
	.hwrpb = {
		.phys_addr = ADDR(boot_data.hwrpb),
		.id = HWRPB_SIGNATURE,
		.revision = 6,
		.size = sizeof(boot_data.hwrpb),
		.cpuid = 0,
		.pagesize = 8192,
		.pa_bits = 13,
		.max_asn = HWRPB_MAX_ASN,
		.ssn = "MILO-LinuxBIOS",
		.sys_type = HWRPB_SYS_TYPE, 
		.sys_variation = HWRPB_SYS_VARIATION,
		.sys_revision = 0,
		.intr_freq = HWRPB_INTR_FREQ,
		.cycle_freq = HWRPB_CYCLE_FREQ,
		.vptb = 0,
		.res1 = 0,
		.tbhb_offset = 0,
		.nr_processors = ELEMENTS(boot_data.cpu),
		.processor_size = sizeof(boot_data.cpu[0]),
		.processor_offset = OFFSET(boot_data.cpu, boot_data.hwrpb),
		.ctb_nr = 0,
		.ctb_size = 0,
		.ctbt_offset = 0,
		.mddt_offset = OFFSET(boot_data.mem, boot_data.hwrpb),
		.cdb_offset = 0,
		.frut_offset = 0,
		.save_terminal = 0,
		.save_terminal_data = 0,
		.restore_terminal = 0,
		.restore_terminal_data = 0,
		.CPU_restart = 0,
		.CPU_restart_data = 0,
		.res2 = 0,
		.res3 = 0,
		.chksum = 0,
		.rxrdy = 0,
		.txrdy = 0,
		.dsr_offset = OFFSET(boot_data.dsr, boot_data.hwrpb),
	},
	.dsr = {
		.smm = 0,
		.lurt_off = 0,
		.sysname_off = OFFSET(boot_data.sysname, boot_data.dsr),
	},
	.sysname = {
		.length = sizeof(HWRPB_SYSNAME) -1,
		.name = HWRPB_SYSNAME,
	},

	.cpu = {
		{
			.serial_no = { 
				0x73695f78756e694c,
				0x002174616572475f,
			},
			.type = HWRPB_CPU,
		}
	},
	.mem = {
		.chksum = 0,
		.optional_pa = ADDR(boot_data.mem), 
		.numclusters = ELEMENTS(boot_data.mem_cluster),
	},
	.mem_cluster = {
		{
			.start_pfn = 0,
			.numpages = 128,
			.numtested = 0,
			.bitmap_va = 0,
			.bitmap_pa = 0,
			.bitmap_chksum = 0,
			.usage = 1, /* console/PALcode reserved */
		},
		{
			.start_pfn = 128,
			.numpages = 0,
			.numtested = 0,
			.bitmap_va = 0,
			.bitmap_pa = 0,
			.bitmap_chksum = 0,
			.usage = 0,
		},
	},
};


static struct {
	struct uniform_boot_header header;
	struct {
		struct ube_hwrpb hwrpb;
	} env;
	unsigned char command_line[1024];
} ube_all = {
	.header = {
		.header_bytes = sizeof(ube_all.header),
		.header_checksum = 0,
		.arg = (unsigned long)&ube_all.command_line,
		.arg_bytes = sizeof(ube_all.command_line),
		.env = (unsigned long)&ube_all.env,
		.env_bytes = sizeof(ube_all.env),
	},
	.env = {
		.hwrpb = {
			.tag = UBE_TAG_HWRPB,
			.size = sizeof(ube_all.env.hwrpb),
			.hwrpb = (unsigned long)&boot_data.hwrpb,
		},
	},
	.command_line = HWRPB_COMMAND_LINE,
};

static unsigned long hwrpb_compute_checksum(struct hwrpb_struct *hwrpb)
{
	unsigned long sum = 0, *l;
        for (l = (unsigned long *)hwrpb; l <= (unsigned long *) &hwrpb->chksum; ++l)
                sum += *l;
	return sum;

}


void *get_ube_pointer(unsigned long totalram)
{
	/* Set the amount of RAM I have */
	boot_data.mem_cluster[1].numpages = (totalram >> 3) -
		boot_data.mem_cluster[1].start_pfn;
	boot_data.hwrpb.chksum = 0;
	boot_data.hwrpb.chksum = hwrpb_compute_checksum(&boot_data.hwrpb);
	ube_all.header.header_checksum = 0;
	ube_all.header.header_checksum = 
		uniform_boot_compute_header_checksum(&ube_all.header);
	return &ube_all;
}


int elf_check_arch(Elf_ehdr *ehdr)
{
	return (
		(ehdr->e_machine == EM_ALPHA) &&
		(ehdr->e_ident[EI_CLASS] == ELFCLASS64) &&
		(ehdr->e_ident[EI_DATA] == ELFDATA2LSB)
		);
}

void jmp_to_elf_entry(void *entry, void *ptr)
{
	void (*kernel_entry)(void *ptr);
	kernel_entry = entry;

	/* Jump to kernel */
	kernel_entry(ptr);
}

