/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef UFLASHROM_H
#define UFLASHROM_H

#define FLASHROM_PROGRAMMER_INTERNAL_AP "internal"

struct firmware_programmer {
	const char *programmer;
	uint32_t size;
	uint8_t *data;
};

int flashrom_read(struct firmware_programmer *image, const char *region);
int flashrom_write(struct firmware_programmer *image, const char *region);

#endif /* UFLASHROM_H */
