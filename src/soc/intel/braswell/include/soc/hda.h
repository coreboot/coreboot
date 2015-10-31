/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_HDA_H_
#define _SOC_HDA_H_

/*
 * PCI config registers.
 */

#define HDA_DCKSTS		0x4d
# define HDA_DCKSTS_DS		(1 << 7)
# define HDA_DCKSTS_DM		(1 << 0)

#define HDA_DEVC		0x78
# define HDA_DEVC_MRRS		0x7000
# define HDA_DEVC_NSNPEN	(1 << 11)
# define HDA_DEVC_AUXPEN	(1 << 10)
# define HDA_DEVC_PEEN		(1 << 9)
# define HDA_DEVC_ETEN		(1 << 8)
# define HDA_DEVC_MAXPAY	0x00e0
# define HDA_DEVC_ROEN		(1 << 4)
# define HDA_DEVC_URREN		(1 << 3)
# define HDA_DEVC_FEREN		(1 << 2)
# define HDA_DEVC_NFEREN	(1 << 1)
# define HDA_DEVC_CEREN		(1 << 0)

#endif /* _SOC_HDA_H_ */
