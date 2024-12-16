/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE)
#define LPC_GMR_SIZE 0x10000
#endif

Device (CREC)
{
	Name (_HID, "GOOG0004")
	Name (_UID, 1)
	Name (_DDN, "EC Command Device")
#ifdef EC_ENABLE_WAKE_PIN
#ifndef EC_ENABLE_SYNC_IRQ_GPIO
	Name (_PRW, Package () { EC_ENABLE_WAKE_PIN, 0x5 })
#else
#define EC_SYNC_IRQ_WAKE_CAPABLE
#endif
#endif

#ifdef EC_SYNC_IRQ_WAKE_CAPABLE
	#define EC_SYNC_SHARE_TYPE ExclusiveAndWake
#else
	#define EC_SYNC_SHARE_TYPE Exclusive
#endif

#if defined(EC_ENABLE_SYNC_IRQ) || defined(EC_ENABLE_SYNC_IRQ_GPIO)
	Method (_CRS, 0x0, NotSerialized)
	{
		Name (RBUF, ResourceTemplate()
		{
#ifdef EC_ENABLE_SYNC_IRQ
			Interrupt (ResourceConsumer, Level, ActiveLow, EC_SYNC_SHARE_TYPE) { EC_SYNC_IRQ }
#endif

#ifdef EC_ENABLE_SYNC_IRQ_GPIO
			GpioInt (Level, ActiveLow, EC_SYNC_SHARE_TYPE, PullDefault,
				 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, ,)
			{
				EC_SYNC_IRQ
			}
#endif

#if CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE)
		Memory32Fixed (ReadWrite, 0, 0, MADR)
#endif
		})

#if CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE)
		CreateDWordField (RBUF, MADR._BAS, BAS0)
		CreateDWordField (RBUF, MADR._LEN, LEN0)
		BAS0 = \_SB.PCI0.LPCB.GLGM()
		LEN0 = LPC_GMR_SIZE
#endif
		Return (RBUF)
	}
#endif

#ifdef EC_ENABLE_MKBP_DEVICE
	Device (CKSC)
	{
		Name (_HID, "GOOG0007")
		Name (_UID, 1)
		Name (_DDN, "EC MKBP Device")
	}
#endif

#ifdef EC_ENABLE_CBAS_DEVICE
	Device (CBAS)
	{
		Name (_HID, "GOOG000B")
		Name (_UID, 1)
		Name (_DDN, "EC Base Switch Device")
	}
#endif

#ifdef EC_ENABLE_PD_MCU_DEVICE
	#include "pd.asl"
#endif

#ifdef EC_ENABLE_TBMC_DEVICE
	#include "tbmc.asl"
#endif

	Method(_STA, 0)
	{
		Return (0xF)
	}

#if CONFIG(DRIVERS_ACPI_THERMAL_ZONE)
	Method(TMP, 1)
	{
		Return(^^TSRD(Arg0))
	}
#endif
}
