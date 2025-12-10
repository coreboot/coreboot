/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_RPMH_BCM_H__
#define __SOC_QUALCOMM_RPMH_BCM_H__

#include <types.h>

/**
 * rpmh_bcm_vote() - Send BCM (Bus Clock Manager) vote to RPMh
 * @resource_name: BCM resource name
 * @vote_value: Vote value to send
 *
 * Return: 0 on success, negative error code on failure
 */
int rpmh_bcm_vote(const char *resource_name, u32 vote_value);

#endif /* __SOC_QUALCOMM_RPMH_BCM_H__ */
