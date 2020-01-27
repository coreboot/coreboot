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
 */

/* Note: This code supports the 82371AB/EB/MB. */

/* Datasheets:
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

#include <device/device.h>

const struct chip_operations southbridge_intel_i82371eb_ops = {
	CHIP_NAME("Intel 82371AB/EB/MB Southbridge")
};
