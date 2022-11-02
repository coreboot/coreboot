/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef REGISTER
#error "define REGISTER(NAME) before including this file"
#endif

#if !CONFIG(DRIVERS_I2C_SX9324_SUPPORT_LEGACY_LINUX_DRIVER)
#error "This file is only to support the legacy sx932x linux driver"
#endif

REGISTER(reg_adv_ctrl0);
REGISTER(reg_adv_ctrl1);
REGISTER(reg_adv_ctrl2);
REGISTER(reg_adv_ctrl3);
REGISTER(reg_adv_ctrl4);
REGISTER(reg_adv_ctrl5);
REGISTER(reg_adv_ctrl6);
REGISTER(reg_adv_ctrl7);
REGISTER(reg_adv_ctrl8);
REGISTER(reg_adv_ctrl9);
REGISTER(reg_adv_ctrl10);
REGISTER(reg_adv_ctrl11);
REGISTER(reg_adv_ctrl12);
REGISTER(reg_adv_ctrl13);
REGISTER(reg_adv_ctrl14);
REGISTER(reg_adv_ctrl15);
REGISTER(reg_adv_ctrl16);
REGISTER(reg_adv_ctrl17);
REGISTER(reg_adv_ctrl18);
REGISTER(reg_adv_ctrl19);
REGISTER(reg_adv_ctrl20);

REGISTER(reg_afe_ctrl0);
REGISTER(reg_afe_ctrl1);
REGISTER(reg_afe_ctrl2);
REGISTER(reg_afe_ctrl3);
REGISTER(reg_afe_ctrl4);
REGISTER(reg_afe_ctrl5);
REGISTER(reg_afe_ctrl6);
REGISTER(reg_afe_ctrl7);
REGISTER(reg_afe_ctrl8);
REGISTER(reg_afe_ctrl9);

REGISTER(reg_afe_ph0);
REGISTER(reg_afe_ph1);
REGISTER(reg_afe_ph2);
REGISTER(reg_afe_ph3);

REGISTER(reg_gnrl_ctrl0);
REGISTER(reg_gnrl_ctrl1);

REGISTER(reg_irq_msk);
REGISTER(reg_irq_cfg0);
REGISTER(reg_irq_cfg1);
REGISTER(reg_irq_cfg2);

REGISTER(reg_prox_ctrl0);
REGISTER(reg_prox_ctrl1);
REGISTER(reg_prox_ctrl2);
REGISTER(reg_prox_ctrl3);
REGISTER(reg_prox_ctrl4);
REGISTER(reg_prox_ctrl5);
REGISTER(reg_prox_ctrl6);
REGISTER(reg_prox_ctrl7);
