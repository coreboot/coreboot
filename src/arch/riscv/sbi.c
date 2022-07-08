/* SPDX-License-Identifier: GPL-2.0-only */

#include <mcall.h>
#include <stdint.h>
#include <arch/exception.h>
#include <sbi.h>
#include <vm.h>
#include <console/uart.h>
#include <commonlib/helpers.h>

static uintptr_t send_ipi(uintptr_t *pmask, intptr_t type)
{
	uintptr_t mask = mprv_read_uintptr_t(pmask);
	for (int i = 0; mask; i++) {
		if (mask & 1) {
			OTHER_HLS(i)->ipi_pending |= type;
			/* send soft interrupt to target hart */
			set_msip(i, 1);
		}
		mask = mask >> 1;
	}
	return 0;
}

static uintptr_t sbi_set_timer(uint64_t when)
{
	clear_csr(mip, MIP_STIP);
	set_csr(mie, MIP_MTIP);
	*(HLS()->timecmp) = when;
	return 0;
}

#if CONFIG(CONSOLE_SERIAL)
static uintptr_t sbi_console_putchar(uint8_t ch)
{
	uart_tx_byte(CONFIG_UART_FOR_CONSOLE, ch);
	return 0;
}

static uintptr_t sbi_console_getchar(void)
{
	return uart_rx_byte(CONFIG_UART_FOR_CONSOLE);
}
#endif

static uintptr_t sbi_clear_ipi(void)
{
	clear_csr(mip, MIP_SSIP);
	return 0;
}

/*
 * sbi is triggered by the s-mode ecall
 * parameter : register a0 a1 a2
 * function  : register a7
 * return    : register a0
 */
void handle_sbi(trapframe *tf)
{
	uintptr_t ret = 0;
	uintptr_t arg0 = tf->gpr[10];
	__maybe_unused uintptr_t arg1 = tf->gpr[11];
	uintptr_t which = tf->gpr[17];

	switch (which) {
	case SBI_SET_TIMER:
#if __riscv_xlen == 32
		ret = sbi_set_timer(arg0 + ((uint64_t)arg1 << 32));
#else
		ret = sbi_set_timer(arg0);
#endif
		break;
#if CONFIG(CONSOLE_SERIAL)
	case SBI_CONSOLE_PUTCHAR:
		ret = sbi_console_putchar(arg0);
		break;
	case SBI_CONSOLE_GETCHAR:
		ret = sbi_console_getchar();
		break;
#endif
	case SBI_CLEAR_IPI:
		ret = sbi_clear_ipi();
		break;
	case SBI_SEND_IPI:
		ret = send_ipi((uintptr_t *)arg0, IPI_SOFT);
		break;
	case SBI_REMOTE_FENCE_I:
		ret = send_ipi((uintptr_t *)arg0, IPI_FENCE_I);
		break;
	case SBI_REMOTE_SFENCE_VMA:
		ret = send_ipi((uintptr_t *)arg0, IPI_SFENCE_VMA);
		break;
	case SBI_REMOTE_SFENCE_VMA_ASID:
		ret = send_ipi((uintptr_t *)arg0, IPI_SFENCE_VMA_ASID);
		break;
	case SBI_SHUTDOWN:
		ret = send_ipi((uintptr_t *)arg0, IPI_SHUTDOWN);
		break;
	default:
		ret = -SBI_ENOSYS;
		break;
	}
	tf->gpr[10] = ret;
	write_csr(mepc, read_csr(mepc) + 4);
}
