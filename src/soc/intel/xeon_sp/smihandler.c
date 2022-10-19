/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <console/uart.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <drivers/uart/uart8250reg.h>
#include <intelblocks/smihandler.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

struct uart8250_state {
	uint8_t IER, IIR, MCR, LCR, DLL, DLM;
};

static struct uart8250_state s_uart8250_state;

static void uart8250_store(unsigned int base_port)
{
	/* Save previous state for restoring later. */
	s_uart8250_state.IER = inb(base_port + UART8250_IER);
	s_uart8250_state.IIR = inb(base_port + UART8250_FCR);
	s_uart8250_state.MCR = inb(base_port + UART8250_MCR);
	s_uart8250_state.LCR = inb(base_port + UART8250_LCR);
	s_uart8250_state.DLL = inb(base_port + UART8250_DLL);
	s_uart8250_state.DLM = inb(base_port + UART8250_DLM);
}

void smm_soc_early_init(void)
{
	if (CONFIG(DRIVERS_UART_8250IO) && CONFIG(DEBUG_SMI))
		uart8250_store(uart_platform_base(CONFIG_UART_FOR_CONSOLE));
}

static void uart8250_restore(unsigned int base_port)
{
	outb(s_uart8250_state.DLL, base_port + UART8250_DLL);
	outb(s_uart8250_state.DLM, base_port + UART8250_DLM);
	outb(s_uart8250_state.MCR, base_port + UART8250_MCR);
	outb(s_uart8250_state.LCR, base_port + UART8250_LCR);
	if ((s_uart8250_state.IIR & UART8250_IIR_FIFO_EN) == UART8250_IIR_FIFO_EN)
		outb(UART8250_FCR_FIFO_EN, base_port + UART8250_FCR);
	outb(s_uart8250_state.IER, base_port + UART8250_IER);
}

void smm_soc_exit(void)
{
	if (CONFIG(DRIVERS_UART_8250IO) && CONFIG(DEBUG_SMI))
		uart8250_restore(uart_platform_base(CONFIG_UART_FOR_CONSOLE));
}

/*
 * Specific SOC SMI handler during ramstage finalize phase
 */
void smihandler_soc_at_finalize(void)
{
	/* SMM_FEATURE_CONTROL can only be written within SMM. */
	printk(BIOS_DEBUG, "Lock SMM_FEATURE_CONTROL\n");
	const pci_devfn_t dev = UBOX_DEV_PMON;
	pci_or_config32(dev, SMM_FEATURE_CONTROL,
			SMM_CODE_CHK_EN | SMM_FEATURE_CONTROL_LOCK);

}

/* This is needed by common SMM code */
const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
};
