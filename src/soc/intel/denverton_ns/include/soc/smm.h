/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _DENVERTON_NS_SMM_H_
#define _DENVERTON_NS_SMM_H_

struct smm_relocation_attrs {
	uint32_t smbase;
	uint32_t smrr_base;
	uint32_t smrr_mask;
};

#endif /* _DENVERTON_NS_SMM_H_ */
