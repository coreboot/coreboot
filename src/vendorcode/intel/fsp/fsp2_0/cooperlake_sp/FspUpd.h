/*
 * These are fake files which only contain padding and some known
 * data structures from FSP2.x spec.
 */

#ifndef __FSPUPD_H__
#define __FSPUPD_H__

#include <stdint.h>
#include <FspEas.h>

#define FSP_M_CONFIG FSPM_CONFIG

#define FSPT_UPD_SIGNATURE          0x545F445055434F53ULL        /* 'SOCUPD_T' */
#define FSPM_UPD_SIGNATURE          0x4D5F445055434F53ULL        /* 'SOCUPD_M' */
#define FSPS_UPD_SIGNATURE          0x535F445055434F53ULL        /* 'SOCUPD_S' */

#endif
