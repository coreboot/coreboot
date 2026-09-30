/* SPDX-License-Identifier: GPL-2.0-only */

#define CREC_HAS_GMR	CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE)
#define LPC_GMR_SIZE	0x10000

#if defined(EC_ENABLE_SYNC_IRQ) || defined(EC_ENABLE_SYNC_IRQ_GPIO) || CREC_HAS_GMR
#define CREC_HAS_CRS	1
#else
#define CREC_HAS_CRS	0
#endif

/* With a GPIO sync interrupt, the interrupt itself is the wake source instead of _PRW. */
#if defined(EC_ENABLE_WAKE_PIN) && !defined(EC_ENABLE_SYNC_IRQ_GPIO)
#define CREC_HAS_PRW	1
#else
#define CREC_HAS_PRW	0
#endif

#if defined(EC_SYNC_IRQ_WAKE_CAPABLE) || \
	(defined(EC_ENABLE_SYNC_IRQ_GPIO) && defined(EC_ENABLE_WAKE_PIN))
#define EC_SYNC_SHARE_TYPE	ExclusiveAndWake
#else
#define EC_SYNC_SHARE_TYPE	Exclusive
#endif

Device (CREC)
{
	Name (_HID, CONFIG_EC_GOOGLE_CHROMEEC_ACPI_HID)
	Name (_UID, 1)
	Name (_DDN, "EC Command Device")
#if CREC_HAS_PRW
	Name (_PRW, Package () { EC_ENABLE_WAKE_PIN, 0x5 })
#endif

#if CREC_HAS_CRS
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

#if CREC_HAS_GMR
		Memory32Fixed (ReadWrite, 0, 0, MADR)
#endif
		})

#if CREC_HAS_GMR
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

		Method (_STA, 0)
		{
#if CONFIG(SYSTEM_TYPE_MINIPC) || CONFIG(SYSTEM_TYPE_ALL_IN_ONE)
			Return (0)
#else
			Return (0xF)
#endif
		}
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
		Return(\_SB.PCI0.LPCB.EC0.TSRD(Arg0))
	}
#endif
}
