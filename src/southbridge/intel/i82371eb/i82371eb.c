/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Note: This code supports the 82371FB/SB/MX/AB/EB/MB and 82437MX. */

/* Datasheets:
 *   - Name: 82371FB (PIIX) AND 82371SB (PIIX3) PCI ISA IDE XCELERATOR
 *   - URL: http://www.intel.com/design/intarch/datashts/290550.htm
 *   - PDF: http://download.intel.com/design/intarch/datashts/29055002.pdf
 *   - Date: April 1997
 *   - Order Number: 290550-002
 *
 *   - Name: 82371FB (PIIX) and 82371SB (PIIX3) PCI ISA IDE Xcelerator
 *           Specification Update
 *   - URL: http://www.intel.com/design/chipsets/specupdt/297658.htm
 *   - PDF: http://download.intel.com/design/chipsets/specupdt/29765801.pdf
 *   - Date: March 1998
 *   - Order Number: 297658-004
 *
 *   - Name: 82371AB PCI-TO-ISA / IDE XCELERATOR (PIIX4)
 *           (applies to 82371AB/EB/MB, a.k.a. PIIX4/PIIX4E/PIIX4M)
 *   - URL: http://www.intel.com/design/intarch/datashts/290562.htm
 *   - PDF: http://www.intel.com/design/intarch/datashts/29056201.pdf
 *   - Date: April 1997
 *   - Order Number: 290562-001
 *
 *   - Name: 82371AB/EB/MB (PIIX4/PIIX4E/PIIX4M) Specification Update
 *   - URL: http://www.intel.com/design/chipsets/specupdt/297738.htm
 *   - PDF: http://www.intel.com/design/chipsets/specupdt/29773817.pdf
 *   - Date: January 2002
 *   - Order Number: 297738-017
 */

/* TODO: List the other datasheets. */

#include <device/device.h>
#include "i82371eb.h"

void i82371eb_enable(struct device *dev)
{
	/* TODO: Nothing to do? */
}

const struct chip_operations southbridge_intel_i82371eb_ops = {
	CHIP_NAME("Intel 82371FB/SB/MX/AB/EB/MB Southbridge")
	.enable_dev = i82371eb_enable,
};
