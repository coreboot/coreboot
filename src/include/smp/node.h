/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SMP_NODE_H_
#define _SMP_NODE_H_

#if CONFIG(SMP)
int boot_cpu(void);
#else
#define boot_cpu(x) 1
#endif

#endif /* _SMP_NODE_H_ */
