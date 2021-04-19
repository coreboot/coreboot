/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ufs.h>

void ufs_disable_refclk(void)
{
	/* disable ref clock to let UFSHCI release SPM signal */
	write32((void *)UFS_REFCLK_CTRL, 0);
}
