/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

void mainboard_program_early_gpios(void); /* bootblock GPIO configuration */
void mainboard_program_gpios(void); /* ramstage GPIO configuration */

#endif  /* MAINBOARD_GPIO_H */
