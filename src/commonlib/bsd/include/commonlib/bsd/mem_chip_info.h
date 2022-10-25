/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _COMMONLIB_BSD_MEM_CHIP_INFO_H_
#define _COMMONLIB_BSD_MEM_CHIP_INFO_H_

#include <stddef.h>

enum mem_chip_type {
	MEM_CHIP_DDR3 = 0x30,
	MEM_CHIP_LPDDR3 = 0x38,
	MEM_CHIP_DDR4 = 0x40,
	MEM_CHIP_LPDDR4 = 0x48,
	MEM_CHIP_LPDDR4X = 0x49,
};

#define MEM_CHIP_STRUCT_VERSION 0	/* Hopefully we'll never have to bump this... */

/*
 * This structure describes memory topology by channel, rank and DDR chip.
 *
 * Definitions:
 *	channel: A DDR channel is an entire set of DDR pins (including DQ, CS, CA, etc.) coming
 *		 out of the DDR controller on the SoC. An SoC may support one or more DDR
 *		 channels. The DDR chips on different channels are entirely independent from one
 *		 another and not aware of each others' existence (concepts like dual-channel
 *		 mode / channel-interleaving only exist inside the DDR controller and are not
 *		 relevant to this topology description).
 *	chip:	 A physically distinct DDR part on the mainboard, with a single "channel" worth
 *		 of pins (DQ, CS, CA, etc.). Parts that combine multiple "channels" worth of
 *		 pins in one package (e.g. separate DQ[0]_A and DQ[0]_B) count as multiple
 *		 separate chips in this description.
 *	rank:	 DDR ranks are independent sub-units within a single physical DDR chip. Each DDR
 *		 transaction only communicates with one rank, which is selected through the CS
 *		 pins.
 *	MRx:	 Mode registers, as defined in the various (LP)DDR specs. Mode registers are
 *		 read through the DQ and written through the CA pins, and each rank on each chip
 *		 has a separate mode register as selected by the CS pins.
 *
 * The basic purpose of this structure is to record information read from the mode registers on
 * all ranks of all chips on all channels, to later allow the recipient of this information to
 * reconstruct the topology and exact parts used on the board. Since each system may have a
 * variable number of channels, and the chips on those channels may each have a variable number
 * of ranks, this would require a doubly-nested structure with variable array sizes in both
 * dimensions. The size and offset calculations in such a structure would become very cumbersome
 * and error-prone, so instead this design just stores a one-dimensional array of "entries"
 * where each entry stores information about one specific rank on one specific channel. This
 * means that information which is specific to the channel itself is duplicated among all such
 * entries for all ranks on that channel, and in a well-formed instance of this structure the
 * values in per-channel fields (`type` and `channel_io_width`) among all entries that have the
 * same value in the `channel` field MUST be identical.
 *
 * The information read from the mode registers should be decoded into normal integer values,
 * but not otherwise adjusted in any way. For example, if the value read from MR8 on an LPDDR4
 * chip is 0b00010000, the `density_mbits` field should be set to 8192 (MR8[5:2] = 0b0100
 * decoded to 8192 Mbits) and the `io_width` field should be set to 16 (MR8[7:6] = 0b00 decoded
 * to x16.
 *
 * Note that on some systems the I/O width (number of DQ pins) of the SoC controller's channel
 * is a multiple of the I/O width on the DDR chip (which is the reason the two are recorded
 * separately in this structure). This means that two identical DDR chips are wired in parallel
 * on the same channel (e.g. one 16-bit part is wired to DQ[0:15] and the other to DQ[16:31]).
 * All other pins beside DQ are shorted together in this configuration. This means that only the
 * mode registers of the first chip can be read and recorded in this structure (and all mode
 * register writes automatically write the same value to all chips through the shorted CA pins).
 * The other chips are reported "implicitly" via the mismatch in I/O width.
 *
 * That means that the total amount of memory in bytes available on the whole system is:
 *
 * SUM[over all entries](density_mbits * (channel_io_width/io_width)) * 1024 * 1024 / 8
 */
struct mem_chip_info {
	uint8_t num_entries;
	uint8_t reserved[2];		/* Must be set to 0 */
	uint8_t struct_version;		/* Must always be MEM_CHIP_STRUCT_VERSION */
	struct mem_chip_entry {
		uint8_t channel;	/* Channel number this entry belongs to */
		uint8_t rank;		/* Rank number within that channel */

		/* per-channel information */
		uint8_t type;		/* enum mem_chip_type */
		uint8_t channel_io_width; /* I/O width of the channel (no. of DQ pins on SoC) */

		/* per-rank information */
		uint32_t density_mbits;	/* density in megabits, decoded from MR8 */
		uint8_t io_width;	/* I/O width of the DDR chip, decoded from MR8 */
		uint8_t manufacturer_id; /* raw value from MR5 */
		uint8_t revision_id[2];	/* raw values from MR6 and MR7 */
		uint8_t serial_id[8];	/* LPDDR5 only (set to 0 otherwise), MR47 - MR54 */
	} entries[0];
};

static inline size_t mem_chip_info_size(int entries)
{
	return sizeof(struct mem_chip_info) + sizeof(struct mem_chip_entry) * entries;
};

static inline uint64_t mem_chip_info_total_density_bytes(const struct mem_chip_info *info)
{
	uint64_t bytes = 0;
	int i;

	for (i = 0; i < info->num_entries; i++) {
		const struct mem_chip_entry *e = &info->entries[i];
		bytes += e->density_mbits * (e->channel_io_width / e->io_width) * (MiB / 8);
	}

	return bytes;
}

#endif /* _COMMONLIB_BSD_MEM_CHIP_INFO_H_ */
