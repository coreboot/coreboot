/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>

#define AWAC_GPE		0x72
#define AWAC_WADT_AC		0x1800
#define AWAC_GCP_S4		0xa7
#define AWAC_GCP_S5		0x1e7
#define AWAC_AC_STATUS		1
#define AWAC_DC_STATUS		2
#define RTC_DISABLED		0xffffffff
#define RTC_SET			0x80
#define RTC_UIP			0x80
#define RTC_UIP_POLL_US		10
#define RTC_UIP_TIMEOUT_US	3000

#if CONFIG(SOC_INTEL_COMMON_ACPI_TIME_ALARM_S5)
#define AWAC_LOWEST_SLEEP_STATE	5
#define AWAC_GCP		AWAC_GCP_S5
#else
#define AWAC_LOWEST_SLEEP_STATE	4
#define AWAC_GCP		AWAC_GCP_S4
#endif

Scope (\_SB.PCI0.LPCB.RTC)
{
	OperationRegion (RTCM, SystemCMOS, 0, 0x3f)
	Field (RTCM, ByteAcc, Lock, Preserve)
	{
		Offset (0), SEC, 8,
		Offset (2), MIN, 8,
		Offset (4), HOR, 8,
		Offset (7), DAY, 8,
		Offset (8), MON, 8,
		Offset (9), YEAR, 8,
		Offset (0x0a), REGA, 8,
		REGB, 8,
		Offset (0x32), CNTY, 8,
	}

	Mutex (RTCL, 0)

	Method (GRTT, 0, Serialized)
	{
		Name (BUFF, Buffer (0x10) {})
		CreateWordField (BUFF, 0x0, Y)
		CreateByteField (BUFF, 0x2, M)
		CreateByteField (BUFF, 0x3, D)
		CreateByteField (BUFF, 0x4, H)
		CreateByteField (BUFF, 0x5, MNT)
		CreateByteField (BUFF, 0x6, S)
		CreateByteField (BUFF, 0x7, V)
		CreateWordField (BUFF, 0xa, TZ)
		CreateByteField (BUFF, 0xc, DL)

		TZ = 2047
		DL = 0
		Acquire (RTCL, 0xffff)
		Local0 = 0
		Local2 = 0
		While ((Local0 < RTC_UIP_TIMEOUT_US) && !Local2)
		{
			If (!(REGA & RTC_UIP))
			{
				Local1 = SEC
#if CONFIG(USE_PC_CMOS_ALTCENTURY)
				Y = (FromBCD (CNTY) * 100) + FromBCD (YEAR)
#else
				Y = FromBCD (YEAR) + 1900
				If (Y < 1970)
				{
					Y += 100
				}
#endif
				M = FromBCD (MON)
				D = FromBCD (DAY)
				H = FromBCD (HOR)
				MNT = FromBCD (MIN)
				S = FromBCD (Local1)

				If (!(REGA & RTC_UIP) && (Local1 == SEC))
				{
					Local2 = 1
				}
			}

			If (!Local2)
			{
				Stall (RTC_UIP_POLL_US)
				Local0 += RTC_UIP_POLL_US
			}
		}
		Release (RTCL)

		If (Local2)
		{
			V = 1
		}
		Return (BUFF)
	}

	Method (SRTT, 1, Serialized)
	{
		CreateWordField (Arg0, 0x0, Y)
		CreateByteField (Arg0, 0x2, M)
		CreateByteField (Arg0, 0x3, D)
		CreateByteField (Arg0, 0x4, H)
		CreateByteField (Arg0, 0x5, MNT)
		CreateByteField (Arg0, 0x6, S)

		Acquire (RTCL, 0xffff)
		Local0 = 0
		While ((REGA & RTC_UIP) && (Local0 < RTC_UIP_TIMEOUT_US))
		{
			Stall (RTC_UIP_POLL_US)
			Local0 += RTC_UIP_POLL_US
		}

		If (Local0 >= RTC_UIP_TIMEOUT_US)
		{
			Release (RTCL)
			Return (RTC_DISABLED)
		}

#if !CONFIG(USE_PC_CMOS_ALTCENTURY)
		If ((Y < 1970) || (Y > 2069))
		{
			Release (RTCL)
			Return (RTC_DISABLED)
		}
#endif

		Local3 = REGB
		REGB = Local3 | RTC_SET
		Local1 = Y % 100
		YEAR = ToBCD (Local1)
#if CONFIG(USE_PC_CMOS_ALTCENTURY)
		Local2 = Y / 100
		CNTY = ToBCD (Local2)
#endif
		MON = ToBCD (M)
		DAY = ToBCD (D)
		HOR = ToBCD (H)
		MIN = ToBCD (MNT)
		SEC = ToBCD (S)
		REGB = Local3
		Release (RTCL)

		Return (0)
	}
}

Scope (\_SB)
{
	OperationRegion (WARM, SystemMemory, PCH_PWRM_BASE_ADDRESS, PCH_PWRM_BASE_SIZE)
	Field (WARM, DWordAcc, NoLock, Preserve)
	{
		Offset (AWAC_WADT_AC),
		ACWA, 32,
		DCWA, 32,
		ACET, 32,
		DCET, 32,
	}

	Device (AWAC)
	{
		Name (_HID, "ACPI000E")
		Mutex (WATL, 0)

		Method (_PRW, 0)
		{
			Return (Package () { AWAC_GPE, AWAC_LOWEST_SLEEP_STATE })
		}

		Method (_STA, 0)
		{
			Return (0xf)
		}

		Method (_GCP, 0)
		{
			Return (AWAC_GCP)
		}

		Method (_GRT, 0, Serialized)
		{
			Return (\_SB.PCI0.LPCB.RTC.GRTT ())
		}

		Method (_SRT, 1, Serialized)
		{
			Return (\_SB.PCI0.LPCB.RTC.SRTT (Arg0))
		}

		Method (_GWS, 1, Serialized)
		{
			Local0 = 0
			Acquire (WATL, 0xffff)

			If (Arg0 == 0)
			{
				If ((ACWA == RTC_DISABLED) &&
				    ((\WATA & AWAC_AC_STATUS) || (\WATS & AWAC_AC_STATUS)))
				{
					Local0 |= 1
					\WATA &= ~AWAC_AC_STATUS
				}
				If (\WATS & AWAC_AC_STATUS)
				{
					Local0 |= 2
					\WATS &= ~AWAC_AC_STATUS
				}
			}
			Else
			{
				If ((DCWA == RTC_DISABLED) &&
				    ((\WATA & AWAC_DC_STATUS) || (\WATS & AWAC_DC_STATUS)))
				{
					Local0 |= 1
					\WATA &= ~AWAC_DC_STATUS
				}
				If (\WATS & AWAC_DC_STATUS)
				{
					Local0 |= 2
					\WATS &= ~AWAC_DC_STATUS
				}
			}

			Release (WATL)
			Return (Local0)
		}

		Method (_CWS, 1, Serialized)
		{
			Acquire (WATL, 0xffff)

			If (Arg0 == 0)
			{
				If (ACWA == RTC_DISABLED)
				{
					\WATA &= ~AWAC_AC_STATUS
				}
				\WATS &= ~AWAC_AC_STATUS
			}
			Else
			{
				If (DCWA == RTC_DISABLED)
				{
					\WATA &= ~AWAC_DC_STATUS
				}
				\WATS &= ~AWAC_DC_STATUS
			}

			Release (WATL)
			Return (0)
		}

		Method (_STP, 2)
		{
			If (Arg0 == 0)
			{
				ACET = Arg1
			}
			Else
			{
				DCET = Arg1
			}

			Return (0)
		}

		Method (_STV, 2, Serialized)
		{
			Acquire (WATL, 0xffff)

			If (Arg0 == 0)
			{
				ACWA = Arg1
				If (Arg1 == RTC_DISABLED)
				{
					\WATA &= ~AWAC_AC_STATUS
				}
				Else
				{
					\WATA |= AWAC_AC_STATUS
				}
				\WATS &= ~AWAC_AC_STATUS
			}
			Else
			{
				DCWA = Arg1
				If (Arg1 == RTC_DISABLED)
				{
					\WATA &= ~AWAC_DC_STATUS
				}
				Else
				{
					\WATA |= AWAC_DC_STATUS
				}
				\WATS &= ~AWAC_DC_STATUS
			}

			Release (WATL)
			Return (0)
		}

		Method (_TIP, 1)
		{
			If (Arg0 == 0)
			{
				Return (ACET)
			}

			Return (DCET)
		}

		Method (_TIV, 1)
		{
			If (Arg0 == 0)
			{
				Return (ACWA)
			}

			Return (DCWA)
		}
	}
}

Scope (\_GPE)
{
	Method (_L72, 0, Serialized)
	{
		If (CondRefOf (\_SB.AWAC))
		{
			Notify (\_SB.AWAC, 0x02)
		}
	}
}
