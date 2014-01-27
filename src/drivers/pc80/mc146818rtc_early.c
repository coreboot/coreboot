#include <stdint.h>
#include <pc80/mc146818rtc.h>
#include <fallback.h>
#ifdef __ROMCC__
#include <arch/cbfs.h>
#else
#include <cbfs.h>
#endif
#include <boot/coreboot_tables.h>

#ifndef CONFIG_MAX_REBOOT_CNT
#error "CONFIG_MAX_REBOOT_CNT not defined"
#endif
#if  CONFIG_MAX_REBOOT_CNT > 15
#error "CONFIG_MAX_REBOOT_CNT too high"
#endif

static int cmos_error(void)
{
	unsigned char reg_d;
	/* See if the cmos error condition has been flagged */
	reg_d = cmos_read(RTC_REG_D);
	return (reg_d & RTC_VRT) == 0;
}

#ifdef __ROMCC__
#define const_pointer uint32_t
#else
#define const_pointer const void *
#endif

#if CONFIG_USE_OPTION_TABLE
static const_pointer find_first_entry(void)
{
	const struct cmos_option_table *ct;
#ifdef __ROMCC__
	ct = walkcbfs("cmos_layout.bin");
#else
	ct = cbfs_get_file_content(CBFS_DEFAULT_MEDIA, "cmos_layout.bin",
				   CBFS_COMPONENT_CMOS_LAYOUT, NULL);
#endif
	if (!ct)
		return 0;
	return (const_pointer) ((const unsigned char *) ct + ct->header_length);
}

static const_pointer find_entry(const char *name)
{
	const struct cmos_entries *ce;

	ce = (struct cmos_entries *) find_first_entry();
	if (!ce)
		return 0;
	for(;ce->tag==LB_TAG_OPTION;
		ce=(const struct cmos_entries*)((const unsigned char *)ce + ce->size)) {
		unsigned i;
		for (i = 0; ; i++) {
			if (!(name[i] && i < CMOS_MAX_NAME_LENGTH))
				return (const_pointer) ce;
			if (name[i] != ce->name[i])
				break;
		}
	}
	return 0;
}
#endif

static int cmos_chksum_valid(void)
{
#if CONFIG_USE_OPTION_TABLE
	unsigned char addr;
	u16 sum, old_sum;
	const struct cmos_checksum *cc;
	const struct cmos_entries *ce;

	ce = (const struct cmos_entries *) find_entry("version");
	if (!ce)
		return 0;
	if (cmos_read (ce->bit / 8) != (ce->config_id & 0xff))
		return 0;
	if (cmos_read (ce->bit / 8 + 1) != ((ce->config_id >> 8) & 0xff))
		return 0;

	cc = (struct cmos_checksum *) find_first_entry();

	if (!cc)
		return 0;

	for(;cc->tag==LB_TAG_OPTION || cc->tag==LB_TAG_OPTION_ENUM
		    || cc->tag == LB_TAG_OPTION_DEFAULTS;
	    cc=(struct cmos_checksum*)((unsigned char *)cc + cc->size));
	if (cc->tag != LB_TAG_OPTION_CHECKSUM)
		return 0;

	sum = 0;
	/* Compute the cmos checksum */
	for(addr = cc->range_start; addr <= cc->range_end; addr++) {
		sum += cmos_read(addr);
	}

	/* Read the stored checksum */
	old_sum = cmos_read(cc->location) << 8;
	old_sum |=  cmos_read(cc->location+1);

	return sum == old_sum;
#else
	return 0;
#endif
}


static inline int last_boot_normal(void)
{
	unsigned char byte;
	byte = cmos_read(RTC_BOOT_BYTE);
	return (byte & (1 << 1));
}

static inline int do_normal_boot(void)
{
	unsigned char byte;

	if (cmos_error() || !cmos_chksum_valid()) {
		/* There are no impossible values, no checksums so just
		 * trust whatever value we have in the the cmos,
		 * but clear the fallback bit.
		 */
		byte = cmos_read(RTC_BOOT_BYTE);
		byte &= 0x0c;
		byte |= CONFIG_MAX_REBOOT_CNT << 4;
		cmos_write(byte, RTC_BOOT_BYTE);
	}

	/* The RTC_BOOT_BYTE is now o.k. see where to go. */
	byte = cmos_read(RTC_BOOT_BYTE);

	/* Are we in normal mode? */
	if (byte & 1) {
		byte &= 0x0f; /* yes, clear the boot count */
	}

	/* Properly set the last boot flag */
	byte &= 0xfc;
	if ((byte >> 4) < CONFIG_MAX_REBOOT_CNT) {
		byte |= (1<<1);
	}

	/* Are we already at the max count? */
	if ((byte >> 4) < CONFIG_MAX_REBOOT_CNT) {
		byte += 1 << 4; /* No, add 1 to the count */
	}
	else {
		byte &= 0xfc;	/* Yes, put in fallback mode */
	}

	/* Save the boot byte */
	cmos_write(byte, RTC_BOOT_BYTE);

	return (byte & (1<<1));
}

unsigned read_option(const char *name, unsigned def)
{
#if CONFIG_USE_OPTION_TABLE
	const struct cmos_entries *ce;
	unsigned byte;

	ce = (struct cmos_entries *)find_entry(name);

	if (!ce)
		return def;
	
	byte = cmos_read(ce->bit/8);
	byte >>= (ce->bit & 7U);

	return (byte) & ((1U << ce->length) - 1U);
#else
	return def;
#endif
}
