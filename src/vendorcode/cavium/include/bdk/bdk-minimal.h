/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
 * reserved.
 * Copyright 2017-present Facebook, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * bdk-minimal.h: Subset of bdk.h used by coreboot
 */

#ifndef BDK_MINIMAL_H__
#define BDK_MINIMAL_H__

#include <console/console.h>	/* for printk */
#include <endian.h>
#include <stddef.h>		/* for NULL */

#define bdk_le16_to_cpu(x)	le16_to_cpu(x)
#define bdk_le32_to_cpu(x)	le32_to_cpu(x)
#define bdk_le64_to_cpu(x)	le64_to_cpu(x)
#define bdk_be16_to_cpu(x)	be16_to_cpu(x)
#define bdk_be32_to_cpu(x)	be32_to_cpu(x)
#define bdk_be64_to_cpu(x)	be64_to_cpu(x)
#define bdk_cpu_to_le16(x)	cpu_to_le16(x)
#define bdk_cpu_to_le32(x)	cpu_to_le32(x)
#define bdk_cpu_to_le64(x)	cpu_to_le64(x)

#define printf(format, ...)	printk(BIOS_DEBUG, format, ##__VA_ARGS__)
#define puts(str)		printk(BIOS_INFO, str)
#define fflush(x)		/* output gets flushed automatically */

/* careful, the ordering matters for some headers */
#include <libbdk-arch/bdk-warn.h>
#include <libbdk-arch/bdk-asm.h>
#include <libbdk-arch/bdk-model.h>
#include <libbdk-arch/bdk-numa.h>
#include <libbdk-hal/bdk-access.h>

#include <libbdk-arch/bdk-require.h>

#include <libbdk-arch/bdk-csr.h>

#include <libbdk-os/bdk-thread.h>

/* FIXME: experiment to see if including the universe here will solve some
 * current build issues... */
#include <libbdk-arch/bdk-arch.h>
#include <libbdk-boot/bdk-boot.h>
#include <libbdk-dram/bdk-dram.h>
#include <libdram/libdram.h>

static inline char *getenv(const char *name) { return NULL; }

#endif /* BDK_MINIMAL_H__ */
