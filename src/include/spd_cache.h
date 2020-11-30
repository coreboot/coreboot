/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SPD_CACHE_H
#define __SPD_CACHE_H

#include <spd_bin.h>
#include <stddef.h>
#include <stdint.h>

#define SPD_CACHE_FMAP_NAME	(CONFIG_SPD_CACHE_FMAP_NAME)
#define SC_SPD_NUMS		(CONFIG_DIMM_MAX)
#define SC_SPD_OFFSET(n)	(CONFIG_DIMM_SPD_SIZE * n)
#define SC_CRC_OFFSET		(CONFIG_DIMM_MAX * CONFIG_DIMM_SPD_SIZE)
#define SC_SPD_TOTAL_LEN	(CONFIG_DIMM_MAX * CONFIG_DIMM_SPD_SIZE)
#define SC_SPD_LEN		(CONFIG_DIMM_SPD_SIZE)
#define SC_CRC_LEN		(sizeof(uint16_t))

enum cb_err update_spd_cache(struct spd_block *blk);
enum cb_err load_spd_cache(uint8_t **spd_cache, size_t *spd_cache_sz);
bool spd_cache_is_valid(uint8_t *spd_cache, size_t spd_cache_sz);
bool check_if_dimm_changed(u8 *spd_cache, struct spd_block *blk);
enum cb_err spd_fill_from_cache(uint8_t *spd_cache, struct spd_block *blk);

#endif
