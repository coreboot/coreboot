/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

Scope (\_SB)
{
	#include "uncore_irq.asl"

	// Keep in sync with iio_domain_set_acpi_name()!
	#define SOCKET 0
	#define SOCKET_NAME 0
	#include "iiostack.asl"
	#undef SOCKET

	#if CONFIG(SOC_ACPI_HEST)
		Method (_OSC, 4, NotSerialized)
		{
			CreateDWordField (Arg3, 0x00, CDW1)
			CDW1 |= 0x10 /* enable apei */
			Return (Arg3)
		}
	#endif

	#if (CONFIG_MAX_SOCKET > 1)
		// Keep in sync with iio_domain_set_acpi_name()!
		#define SOCKET 1
		#define SOCKET_NAME 2
		#include "iiostack.asl"
		#undef SOCKET
	#endif

	#if (CONFIG_MAX_SOCKET > 2)
		// Keep in sync with iio_domain_set_acpi_name()!
		#define SOCKET 2
		#define SOCKET_NAME 4
		#include "iiostack.asl"
		#undef SOCKET
	#endif

	#if (CONFIG_MAX_SOCKET > 3)
		// Keep in sync with iio_domain_set_acpi_name()!
		#define SOCKET 3
		#define SOCKET_NAME 6
		#include "iiostack.asl"
		#undef SOCKET
	#endif
}
