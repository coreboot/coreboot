/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE)
Scope(\_SB)
{
	// Secure Enclave memory
	Device (EPC)
	{
		External (EPCS, IntObj)
		External (EMNA, IntObj)
		External (ELNG, IntObj)
		Name (_HID, EISAID ("INT0E0C"))
		Name (_STR, Unicode ("Enclave Page Cache 1.0"))
		Name (_MLS, Package () {
			Package (2) { "en", Unicode ("Enclave Page Cache 1.0") }
		})

		Name (RBUF, ResourceTemplate ()
		{
			// _MIN, _MAX and  _LEN get patched runtime
			QWordMemory (
			ResourceConsumer, // ResourceUsage
			PosDecode,	  // Decode		_DEC
			MinNotFixed,	  // IsMinFixed		_MIF
			MaxNotFixed,	  // IsMaxFixed		_MAF
			NonCacheable,	  // Cacheable		_MEM
			ReadWrite,	  // ReadAndWrite	_RW
			0,		  // AddressGranularity	_GRA
			0,		  // AddressMinimum	_MIN
			0,		  // AddressMaximum	_MAX
			0,		  // AddressTranslation	_TRA
			1,		  // RangeLength	_LEN
			,		  // ResourceSourceIndex
			,		  // ResourceSource
			BAR0		  // DescriptorName
			)
		})

		Method (_CRS, 0x0, NotSerialized)
		{
			CreateQwordField (RBUF, ^BAR0._MIN, EMIN)
			CreateQwordField (RBUF, ^BAR0._MAX, EMAX)
			CreateQwordField (RBUF, ^BAR0._LEN, ELEN)
			EMIN = EMNA
			ELEN = ELNG
			EMAX = EMNA + ELNG - 1
			Return (RBUF)
		}

		Method (_STA, 0x0, NotSerialized)
		{
			If (EPCS != 0)
			{
				Return (0xF)
			}
			Return (0x0)
		}

	} // end EPC Device
} // End of Scope(\_SB)
#endif
