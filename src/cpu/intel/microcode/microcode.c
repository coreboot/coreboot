/* SPDX-License-Identifier: GPL-2.0-only */

/* Microcode update for Intel PIII and later CPUs */

#include <stdint.h>
#include <stddef.h>
#include <cbfs.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/microcode.h>
#include <smp/spinlock.h>

DECLARE_SPIN_LOCK(microcode_lock)

struct microcode {
	u32 hdrver;	/* Header Version */
	u32 rev;	/* Update Revision */
	u32 date;	/* Date */
	u32 sig;	/* Processor Signature */

	u32 cksum;	/* Checksum */
	u32 ldrver;	/* Loader Revision */
	u32 pf;		/* Processor Flags */

	u32 data_size;	/* Data Size */
	u32 total_size;	/* Total Size */

	u32 reserved[3];
};

struct ext_sig_table {
	u32 ext_sig_cnt;
	u32 ext_tbl_chksm;
	u32 res[3];
};

struct ext_sig_entry {
	u32 sig;
	u32 pf;
	u32 chksm;
};

static inline u32 read_microcode_rev(void)
{
	/* Some Intel CPUs can be very finicky about the
	 * CPUID sequence used.  So this is implemented in
	 * assembly so that it works reliably.
	 */
	msr_t msr;
	asm volatile (
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl $0x8b, %%ecx\n\t"
		"wrmsr\n\t"
		"movl $0x01, %%eax\n\t"
		"cpuid\n\t"
		"movl $0x08b, %%ecx\n\t"
		"rdmsr\n\t"
		: /* outputs */
		"=a" (msr.lo), "=d" (msr.hi)
		: /* inputs */
		: /* trashed */
		 "ebx", "ecx"
	);
	return msr.hi;
}

#define MICROCODE_CBFS_FILE "cpu_microcode_blob.bin"

void intel_microcode_load_unlocked(const void *microcode_patch)
{
	u32 current_rev;
	msr_t msr;
	const struct microcode *m = microcode_patch;

	if (!m)
		return;

	current_rev = read_microcode_rev();

	/* No use loading the same revision. */
	if (current_rev == m->rev) {
		printk(BIOS_INFO, "microcode: Update skipped, already up-to-date\n");
		return;
	}

#if ENV_RAMSTAGE
	/*SoC specific check to update microcode*/
	if (soc_skip_ucode_update(current_rev, m->rev)) {
		printk(BIOS_DEBUG, "Skip microcode update\n");
		return;
	}
#endif

	msr.lo = (unsigned long)m + sizeof(struct microcode);
	msr.hi = 0;
	wrmsr(IA32_BIOS_UPDT_TRIG, msr);

	current_rev = read_microcode_rev();
	if (current_rev == m->rev) {
		printk(BIOS_INFO, "microcode: updated to revision "
		    "0x%x date=%04x-%02x-%02x\n", read_microcode_rev(),
		    m->date & 0xffff, (m->date >> 24) & 0xff,
		    (m->date >> 16) & 0xff);
		return;
	}

	printk(BIOS_INFO, "microcode: Update failed\n");
}

uint32_t get_current_microcode_rev(void)
{
	return read_microcode_rev();
}

uint32_t get_microcode_rev(const void *microcode)
{
	return ((struct microcode *)microcode)->rev;
}

uint32_t get_microcode_size(const void *microcode)
{
	return ((struct microcode *)microcode)->total_size;
}

uint32_t get_microcode_checksum(const void *microcode)
{
	return ((struct microcode *)microcode)->cksum;
}


static struct ext_sig_table *ucode_get_ext_sig_table(const struct microcode *ucode)
{
	struct ext_sig_table *ext_tbl;
	/* header + ucode data blob size */
	u32 size = ucode->data_size + sizeof(struct microcode);

	ssize_t ext_tbl_len = ucode->total_size - size;

	if (ext_tbl_len < (ssize_t)sizeof(struct ext_sig_table))
		return NULL;

	ext_tbl = (struct ext_sig_table *)((uintptr_t)ucode + size);

	if (ext_tbl_len < (sizeof(struct ext_sig_table) +
				ext_tbl->ext_sig_cnt * sizeof(struct ext_sig_entry)))
		return NULL;

	return ext_tbl;
}

static const void *find_cbfs_microcode(void)
{
	const struct microcode *ucode_updates;
	struct ext_sig_table *ext_tbl;
	size_t microcode_len;
	u32 eax;
	u32 pf, rev, sig, update_size;
	msr_t msr;
	struct cpuinfo_x86 c;

	ucode_updates = cbfs_map(MICROCODE_CBFS_FILE, &microcode_len);
	if (ucode_updates == NULL)
		return NULL;

	rev = read_microcode_rev();
	eax = cpuid_eax(1);
	get_fms(&c, eax);
	sig = eax;

	pf = 0;
	if ((c.x86_model >= 5) || (c.x86 > 6)) {
		msr = rdmsr(IA32_PLATFORM_ID);
		pf = 1 << ((msr.hi >> 18) & 7);
	}

	printk(BIOS_DEBUG, "microcode: sig=0x%x pf=0x%x revision=0x%x\n",
			sig, pf, rev);

	while (microcode_len >= sizeof(*ucode_updates)) {
		/* Newer microcode updates include a size field, whereas older
		 * containers set it at 0 and are exactly 2048 bytes long */
		if (ucode_updates->total_size) {
			update_size = ucode_updates->total_size;
		} else {
			printk(BIOS_SPEW, "Microcode size field is 0\n");
			update_size = 2048;
		}

		/* Checkpoint 1: The microcode update falls within CBFS */
		if (update_size > microcode_len) {
			printk(BIOS_WARNING, "Microcode header corrupted!\n");
			break;
		}

		if ((ucode_updates->sig == sig) && (ucode_updates->pf & pf))
			return ucode_updates;


		/* Check if there is extended signature table */
		ext_tbl = ucode_get_ext_sig_table(ucode_updates);

		if (ext_tbl != NULL) {
			int i;
			struct ext_sig_entry *entry = (struct ext_sig_entry *)(ext_tbl + 1);

			for (i = 0; i < ext_tbl->ext_sig_cnt; i++, entry++) {

				if ((sig == entry->sig) && (pf & entry->pf)) {
					return ucode_updates;
				}
			}
		}

		ucode_updates = (void *)((char *)ucode_updates + update_size);
		microcode_len -= update_size;
	}

	return NULL;
}

const void *intel_microcode_find(void)
{
	static bool microcode_checked;
	static const void *ucode_update;

	if (microcode_checked)
		return ucode_update;

	/*
	 * Since this function caches the found microcode (NULL or a valid
	 * microcode pointer), it is expected to be run from BSP before starting
	 * any other APs. This sequence is not multithread safe otherwise.
	 */
	ucode_update = find_cbfs_microcode();
	microcode_checked = true;

	return ucode_update;
}

void intel_update_microcode_from_cbfs(void)
{
	const void *patch = intel_microcode_find();

	spin_lock(&microcode_lock);

	intel_microcode_load_unlocked(patch);

	spin_unlock(&microcode_lock);
}

#if ENV_RAMSTAGE
__weak int soc_skip_ucode_update(u32 current_patch_id,
	u32 new_patch_id)
{
	return 0;
}
#endif
