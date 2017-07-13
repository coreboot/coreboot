/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Ported from mosys project (http://code.google.com/p/mosys/).
 */

#ifndef __LIB_VPD_TABLES_H__
#define __LIB_VPD_TABLES_H__

#include <compiler.h>
#include <inttypes.h>

#define VPD_ENTRY_MAGIC    "_SM_"
#define VPD_INFO_MAGIC     \
  "\xfe"      /* type: VPD header */       \
  "\x09"      /* key length, 9 = 1 + 8 */  \
  "\x01"      /* info version, 1 */        \
  "gVpdInfo"  /* signature, 8 bytes */ \
  "\x04"      /* value length */

/* Google specific VPD info */
struct google_vpd_info {
  union {
    struct {
      uint8_t type;
      uint8_t key_len;
      uint8_t info_ver;
      uint8_t signature[8];
      uint8_t value_len;
    } tlv;
    uint8_t magic[12];
  } header;
  uint32_t size;
} __packed;

/* Entry */
struct vpd_entry {
  uint8_t anchor_string[4];
  uint8_t entry_cksum;
  uint8_t entry_length;
  uint8_t major_ver;
  uint8_t minor_ver;
  uint16_t max_size;
  uint8_t entry_rev;
  uint8_t format_area[5];
  uint8_t inter_anchor_string[5];
  uint8_t inter_anchor_cksum;
  uint16_t table_length;
  uint32_t table_address;
  uint16_t table_entry_count;
  uint8_t bcd_revision;
} __packed;

/* Header */
struct vpd_header {
  uint8_t type;
  uint8_t length;
  uint16_t handle;
} __packed;

/* Type 0 - firmware information */
struct vpd_table_firmware {
  uint8_t vendor;
  uint8_t version;
  uint16_t start_address;
  uint8_t release_date;
  uint8_t rom_size_64k_blocks;
  uint32_t characteristics;
  uint8_t extension[2];  /* v2.4+ */
  uint8_t major_ver;     /* v2.4+ */
  uint8_t minor_ver;     /* v2.4+ */
  uint8_t ec_major_ver;  /* v2.4+ */
  uint8_t ec_minor_ver;  /* v2.4+ */
} __packed;

/* Type 1 - system information */
struct vpd_table_system {
  uint8_t manufacturer;
  uint8_t name;
  uint8_t version;
  uint8_t serial_number;
  uint8_t uuid[16];
  uint8_t wakeup_type;
  uint8_t sku_number;  /* v2.4+ */
  uint8_t family;      /* v2.4+ */
} __packed;

/* Type 127 - end of table */
struct vpd_table_eot {
  struct vpd_header header;
} __packed;

/* Type 241 - binary blob pointer */
struct vpd_table_binary_blob_pointer {
  uint8_t struct_major_version;
  uint8_t struct_minor_version;
  uint8_t vendor;
  uint8_t description;
  uint8_t major_version;
  uint8_t minor_version;
  uint8_t variant;
  uint8_t reserved[5];
  uint8_t uuid[16];
  uint32_t offset;
  uint32_t size;
} __packed;

/* The length and number of strings defined here is not a limitation of VPD.
 * These numbers were deemed good enough during development. */
#define VPD_MAX_STRINGS 10
#define VPD_MAX_STRING_LENGTH 64

#endif /* __LIB_VPD_TABLES_H__ */
