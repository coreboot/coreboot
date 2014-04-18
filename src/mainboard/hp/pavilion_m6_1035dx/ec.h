/*
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef _MAINBOARD_HP_PAVILION_M6_1035DX_EC_H
#define _MAINBOARD_HP_PAVILION_M6_1035DX_EC_H

#define EC_SMI_GEVENT		23
#define EC_LID_GEVENT		22

#ifndef __SMM__
void pavilion_m6_1035dx_ec_init(void);
#endif

#endif /* _MAINBOARD_HP_PAVILION_M6_1035DX_EC_H   */
