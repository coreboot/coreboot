#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <smp/start_stop.h>
#include <arch/smp/mpspec.h>
#include <string.h>
#include <cpu/p5/cpuid.h>
#include <cpu/p6/apic.h>
#include <printk.h>

unsigned char smp_compute_checksum(void *v, int len)
{
	unsigned char *bytes;
	unsigned char checksum;
	int i;
	bytes = v;
	checksum = 0;
	for(i = 0; i < len; i++) {
		checksum -= bytes[i];
	}
	return checksum;
}

void *smp_write_floating_table(unsigned long addr)
{
	struct intel_mp_floating *mf;
	void *v;
	
	/* 16 byte align the table address */
	addr += 15;
	addr &= ~15;
	v = (void *)addr;

	mf = v;
	mf->mpf_signature[0] = '_';
	mf->mpf_signature[1] = 'M';
	mf->mpf_signature[2] = 'P';
	mf->mpf_signature[3] = '_';
	mf->mpf_physptr = (unsigned long)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	mf->mpf_length = 1;
	mf->mpf_specification = 4;
	mf->mpf_checksum = 0;
	mf->mpf_feature1 = 0;
	mf->mpf_feature2 = 0;
	mf->mpf_feature3 = 0;
	mf->mpf_feature4 = 0;
	mf->mpf_feature5 = 0;
	mf->mpf_checksum = smp_compute_checksum(mf, mf->mpf_length*16);
	return v;
}

void *smp_next_mpc_entry(struct mp_config_table *mc)
{
	void *v;
	v = (void *)(((char *)mc) + mc->mpc_length);
	return v;
}
static void smp_add_mpc_entry(struct mp_config_table *mc, unsigned length)
{
	mc->mpc_length += length;
	mc->mpc_entry_count++;
}

void *smp_next_mpe_entry(struct mp_config_table *mc)
{
	void *v;
	v = (void *)(((char *)mc) + mc->mpc_length + mc->mpe_length);
	return v;
}
static void smp_add_mpe_entry(struct mp_config_table *mc, mpe_t mpe)
{
	mc->mpe_length += mpe->mpe_length;
}

void smp_write_processor(struct mp_config_table *mc,
	unsigned char apicid, unsigned char apicver,
	unsigned char cpuflag, unsigned int cpufeature,
	unsigned int featureflag)
{
	struct mpc_config_processor *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_PROCESSOR;
	mpc->mpc_apicid = apicid;
	mpc->mpc_apicver = apicver;
	mpc->mpc_cpuflag = cpuflag;
	mpc->mpc_cpufeature = cpufeature;
	mpc->mpc_featureflag = featureflag;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

/* If we assume a symmetric processor configuration we can
 * get all of the information we need to write the processor
 * entry from the bootstrap processor.
 * Plus I don't think linux really even cares.
 * Having the proper apicid's in the table so the non-bootstrap
 *  processors can be woken up should be enough.
 */
void smp_write_processors(struct mp_config_table *mc, 
			unsigned long *processor_map)
{
	int i;
	int processor_id;
	unsigned apic_version;
	unsigned cpu_features;
	unsigned cpu_feature_flags;
	int eax, ebx, ecx, edx;
	processor_id = this_processors_id();
	apic_version = apic_read(APIC_LVR) & 0xff;
	cpuid(1, &eax, &ebx, &ecx, &edx);
	cpu_features = eax;
	cpu_feature_flags = edx;
	for(i = 0; i < MAX_CPUS; i++) {
		unsigned long cpu_apicid = initial_apicid[i];
		smp_write_processor(mc, cpu_apicid, apic_version,
			processor_map[i],
			cpu_features, cpu_feature_flags
		);
	
	}
}

void smp_write_bus(struct mp_config_table *mc,
	unsigned char id, unsigned char *bustype)
{
	struct mpc_config_bus *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_BUS;
	mpc->mpc_busid = id;
	memcpy(mpc->mpc_bustype, bustype, sizeof(mpc->mpc_bustype));
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

void smp_write_ioapic(struct mp_config_table *mc,
	unsigned char id, unsigned char ver, 
	unsigned long apicaddr)
{
	struct mpc_config_ioapic *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_IOAPIC;
	mpc->mpc_apicid = id;
	mpc->mpc_apicver = ver;
	mpc->mpc_flags = MPC_APIC_USABLE;
	mpc->mpc_apicaddr = apicaddr;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

void smp_write_intsrc(struct mp_config_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	unsigned char srcbus, unsigned char srcbusirq,
	unsigned char dstapic, unsigned char dstirq)
{
	struct mpc_config_intsrc *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_INTSRC;
	mpc->mpc_irqtype = irqtype;
	mpc->mpc_irqflag = irqflag;
	mpc->mpc_srcbus = srcbus;
	mpc->mpc_srcbusirq = srcbusirq;
	mpc->mpc_dstapic = dstapic;
	mpc->mpc_dstirq = dstirq;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}


void smp_write_lintsrc(struct mp_config_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	unsigned char srcbusid, unsigned char srcbusirq,
	unsigned char destapic, unsigned char destapiclint)
{
	struct mpc_config_lintsrc *mpc;
	mpc = smp_next_mpc_entry(mc);
	memset(mpc, '\0', sizeof(*mpc));
	mpc->mpc_type = MP_LINTSRC;
	mpc->mpc_irqtype = irqtype;
	mpc->mpc_irqflag = irqflag;
	mpc->mpc_srcbusid = srcbusid;
	mpc->mpc_srcbusirq = srcbusirq;
	mpc->mpc_destapic = destapic;
	mpc->mpc_destapiclint = destapiclint;
	smp_add_mpc_entry(mc, sizeof(*mpc));
}

void smp_write_address_space(struct mp_config_table *mc,
	unsigned char busid, unsigned char address_type,
	unsigned int address_base_low, unsigned int address_base_high,
	unsigned int address_length_low, unsigned int address_length_high)
{
	struct mp_exten_system_address_space *mpe;
	mpe = smp_next_mpe_entry(mc);
	memset(mpe, '\0', sizeof(*mpe));
	mpe->mpe_type = MPE_SYSTEM_ADDRESS_SPACE;
	mpe->mpe_length = sizeof(*mpe);
	mpe->mpe_busid = busid;
	mpe->mpe_address_type = address_type;
	mpe->mpe_address_base_low  = address_base_low;
	mpe->mpe_address_base_high = address_base_high;
	mpe->mpe_address_length_low  = address_length_low;
	mpe->mpe_address_length_high = address_length_high;
	smp_add_mpe_entry(mc, (mpe_t)mpe);
}


void smp_write_bus_hierarchy(struct mp_config_table *mc,
	unsigned char busid, unsigned char bus_info,
	unsigned char parent_busid)
{
	struct mp_exten_bus_hierarchy *mpe;
	mpe = smp_next_mpe_entry(mc);
	memset(mpe, '\0', sizeof(*mpe));
	mpe->mpe_type = MPE_BUS_HIERARCHY;
	mpe->mpe_length = sizeof(*mpe);
	mpe->mpe_busid = busid;
	mpe->mpe_bus_info = bus_info;
	mpe->mpe_parent_busid = parent_busid;
	smp_add_mpe_entry(mc, (mpe_t)mpe);
}

void smp_write_compatibility_address_space(struct mp_config_table *mc,
	unsigned char busid, unsigned char address_modifier,
	unsigned int range_list)
{
	struct mp_exten_compatibility_address_space *mpe;
	mpe = smp_next_mpe_entry(mc);
	memset(mpe, '\0', sizeof(*mpe));
	mpe->mpe_type = MPE_COMPATIBILITY_ADDRESS_SPACE;
	mpe->mpe_length = sizeof(*mpe);
	mpe->mpe_busid = busid;
	mpe->mpe_address_modifier = address_modifier;
	mpe->mpe_range_list = range_list;
	smp_add_mpe_entry(mc, (mpe_t)mpe);
}

#if 0 
/* memcpy standard block */
const static char smpblock[] =
{0x5F, 0x4D, 0x50, 0x5F, 0x00, 0x00, 0x00,
 0x00, 0x01, 0x04, 0x9B, 0x05, 0x00, 0x00, 0x00, 0x00
};
void write_smp_table(void *v)
{
	memcpy(v, smpblock, sizeof(smpblock));
}
#endif /* 0 */

