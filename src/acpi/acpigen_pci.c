/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_type.h>
#include <types.h>

void acpigen_write_ADR_pci_devfn(pci_devfn_t devfn)
{
	/*
	 * _ADR for PCI Bus is encoded as follows:
	 * [63:32] - unused
	 * [31:16] - device #
	 * [15:0]  - function #
	 */
	acpigen_write_ADR(PCI_SLOT(devfn) << 16 | PCI_FUNC(devfn));
}

void acpigen_write_ADR_pci_device(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_PCI);
	acpigen_write_ADR_pci_devfn(dev->path.pci.devfn);
}

void acpigen_write_PRT_GSI_entry(unsigned int pci_dev, unsigned int acpi_pin, unsigned int gsi)
{
	acpigen_write_package(4);
	acpigen_write_dword((pci_dev << 16) | 0xffff);
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_write_byte(0);

	/* Source Index */
	acpigen_write_dword(gsi);

	acpigen_pop_len(); /* Package */
}

void acpigen_write_PRT_source_entry(unsigned int pci_dev, unsigned int acpi_pin,
				    const char *source_path, unsigned int index)
{
	acpigen_write_package(4);
	acpigen_write_dword((pci_dev << 16) | 0xffff);
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_emit_namestring(source_path);

	/* Source Index */
	acpigen_write_dword(index);

	acpigen_pop_len(); /* Package */
}

#define PCI_HOST_BRIDGE_OSC_UUID     "33db4d5b-1ff7-401c-9657-7441c03dd766"
#define CXL_HOST_BRIDGE_OSC_UUID     "68f2d50b-c469-4d8a-bd3d-941a103fd3fc"

#define OSC_RET_FAILURE              0x02
#define OSC_RET_UNRECOGNIZED_UUID    0x04
#define OSC_RET_UNRECOGNIZED_REV     0x08
#define OSC_RET_CAPABILITIES_MASKED  0x10

#define OSC_QUERY_SUPPORT_SET        0x01

#define ASL_UUID_UNHANDLED           0x00
#define ASL_UUID_HANDLED             0x01

static void acpigen_OSC_handle_pcie_request(const struct device *domain);
static void acpigen_OSC_handle_cxl_request(const struct device *domain);

/*
 * acpigen_write_OSC_pci_domain
 *
 * Reference:
 * 6.2.11 in https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/06_Device_Configuration/Device_Configuration.html
 *
 * _OSC ASL Arguments: (4)
 * Arg0 - A Buffer containing a UUID
 * Arg1 - An Integer containing a Revision ID of the buffer format
 * Arg2 - An Integer containing a count of entries in Arg3
 * Arg3 - A Buffer containing a list of DWORD capabilities
 *
 * _OSC ASL Return Value:
 * A Buffer containing a list of capabilities
 *
 * Local Variables Assignment:
 * Local0 - Temp assigned
 * Local1 - Temp assigned
 * Local2 - Not used
 * Local3 - Not used
 * Local4 - Not used
 * Local5 - Not used
 * Local6 - Record whether the UUID is handled
 * Local7 - Backs up the input value of Arg3
 *
 * Field Definitions:
 * Name - Width	Source	Offset	Description
 * --------------------------------
 * QSUP - DWord	Local7	0x00	Query support
 * RETE - DWord	Arg3	0x00	Returned errors
 * SUPP - Dword	Arg3	0x04	PCIe Features that OS supported
 * CTRL - Dword	Arg3	0x08	PCIe Features that firmware grant control to OS
 * OTRL - Dword	Local7	0x08	PCIe Features that OS requests for control
 * SUPC - Dword	Arg3	0x0C	CXL Features that OS supported
 * CTRC - Dword	Arg3	0x10	CXL Features that firmware grant control to OS
 * OTRC - Dword	Local7	0x10	CXL Features that OS requests for control
 */
void acpigen_write_OSC_pci_domain(const struct device *domain, const bool is_cxl_domain)
{
	/*
	 * Method (_OSC, 4, NotSerialized)
	 * {
	 */
	acpigen_write_method("_OSC", 4);

	/*
	 *	//
	 *	// Check revision ID
	 *	//
	 *	If (Arg1 != 1)
	 *	{
	 *		RETE = OSC_RET_UNRECOGNIZED_REV
	 *		Return (Arg3)
	 *	}
	 */
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(ARG1_OP);
	acpigen_write_integer(0x1);

	acpigen_write_store_int_to_namestr(OSC_RET_UNRECOGNIZED_REV, "RETE");
	acpigen_write_return_op(ARG3_OP);

	acpigen_write_if_end();

	/*
	 *	//
	 *	// Setup up local variables
	 *	//
	 *	Local7 = Arg3
	 *	CreateDwordField (Local7, 0x00, QSUP)
	 *	CreateDWordField (Arg3, 0x00, RETE)
	 *	RETE = 0x0
	 *	Local6 = ASL_UUID_UNHANDLED
	 */
	acpigen_write_store_ops(ARG3_OP, LOCAL7_OP);
	acpigen_write_create_dword_field(LOCAL7_OP, 0x00, "QSUP");
	acpigen_write_create_dword_field(ARG3_OP, 0x00, "RETE");
	acpigen_write_store_int_to_namestr(0x0, "RETE");
	acpigen_write_store_int_to_op(ASL_UUID_UNHANDLED, LOCAL6_OP);

	/*
	 *	//
	 *	// Refer to CXL-3.1-Specification, 9.18.2
	 *	// A CXL Host Bridge also originates a PCIe hierarchy and will have a
	 *	// _CID of EISAID("PNP0A08"). As such, a CXL Host Bridge device may expose
	 *	// both CXL _OSC and PCIe _OSC.
	 *	//
	 *
	 *	If (Arg0 == ToUUID (PCI_HOST_BRIDGE_OSC_UUID))
	 *	{
	 *		//
	 *		// Handle PCIe _OSC request
	 *		// Mark UUID handled
	 *		//
	 *	}
	 */
	acpigen_write_if();
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_byte(ARG0_OP);
	acpigen_write_uuid(PCI_HOST_BRIDGE_OSC_UUID);

	acpigen_OSC_handle_pcie_request(domain);
	acpigen_write_store_int_to_op(ASL_UUID_HANDLED, LOCAL6_OP);

	acpigen_write_if_end();

	if (is_cxl_domain) {
	/*
	 *	If (Arg0 == ToUUID (CXL_HOST_BRIDGE_OSC_UUID))
	 *	{
	 *		//
	 *		// Handle CXL _OSC request
	 *		// Mark UUID handled
	 *		//
	 *	}
	 */
		acpigen_write_if();
		acpigen_emit_byte(LEQUAL_OP);
		acpigen_emit_byte(ARG0_OP);
		acpigen_write_uuid(CXL_HOST_BRIDGE_OSC_UUID);

		acpigen_OSC_handle_cxl_request(domain);
		acpigen_write_store_int_to_op(ASL_UUID_HANDLED, LOCAL6_OP);

		acpigen_write_if_end();
	}

	/*
	 *	//
	 *	// Handle unrecognized UUID
	 *	//
	 *	If (Local6 == ASL_UUID_UNHANDLED)
	 *	{
	 *		RETE = OSC_RET_UNRECOGNIZED_UUID
	 *	}
	 */
	acpigen_write_if_lequal_op_int(LOCAL6_OP, ASL_UUID_UNHANDLED);
	acpigen_write_store_int_to_namestr(OSC_RET_UNRECOGNIZED_UUID, "RETE");
	acpigen_write_if_end();

	/*
	 *	//
	 *	// All done, return
	 *	//
	 *	Return (Arg3)
	 */
	acpigen_write_return_op(ARG3_OP);

	/*
	 * } // Method (_OSC, 4, NotSerialized)
	 */
	acpigen_pop_len();

}

void acpigen_OSC_handle_pcie_request(const struct device *domain)
{
	uint32_t osc_features = soc_get_granted_pci_features(domain);

	/*
	 *		If (Arg2 < 2))
	 *		{
	 *			RETE = OSC_RET_FAILURE
	 *			Return (Arg3)
	 *		}
	 */
	acpigen_write_if();
	acpigen_emit_byte(LLESS_OP);
	acpigen_emit_byte(ARG2_OP);
	acpigen_write_integer(0x2);

	acpigen_write_store_int_to_namestr(OSC_RET_FAILURE, "RETE");
	acpigen_write_return_op(ARG3_OP);

	acpigen_write_if_end();

	/*
	 *		CreateDWordField (Arg3, 0x04, SUPP)
	 *		CreateDWordField (Arg3, 0x08, CTRL)
	 *		CreateDWordField (Local7, 0x08, OTRL)
	 */
	acpigen_write_create_dword_field(ARG3_OP, 0x04, "SUPP");
	acpigen_write_create_dword_field(ARG3_OP, 0x08, "CTRL");
	acpigen_write_create_dword_field(LOCAL7_OP, 0x08, "OTRL");

	/*
	 *		// Grant PCIe feature controls to OS
	 *		CTRL &= osc_features
	 */
	acpigen_write_to_integer_from_namestring("CTRL", LOCAL0_OP);
	acpigen_write_store_int_to_op(osc_features, LOCAL1_OP);
	acpigen_write_and(LOCAL0_OP, LOCAL1_OP, LOCAL0_OP);
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "CTRL");

	/*
	 *		If (CTRL != OTRL)
	 *		{
	 *			RETE = OSC_RET_CAPABILITIES_MASKED
	 *		}
	 */
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("CTRL");
	acpigen_emit_namestring("OTRL");
	acpigen_write_store_int_to_namestr(OSC_RET_CAPABILITIES_MASKED, "RETE");
	acpigen_write_if_end();
};

void acpigen_OSC_handle_cxl_request(const struct device *domain)
{
	uint32_t osc_features = soc_get_granted_cxl_features(domain);

	/*
	 *		If (Arg2 < 4))
	 *		{
	 *			RETE = OSC_RET_FAILURE
	 *			Return (Arg3)
	 *		}
	 */
	acpigen_write_if();
	acpigen_emit_byte(LLESS_OP);
	acpigen_emit_byte(ARG2_OP);
	acpigen_write_integer(0x4);

	acpigen_write_store_int_to_namestr(OSC_RET_FAILURE, "RETE");
	acpigen_write_return_op(ARG3_OP);

	acpigen_write_if_end();

	/*
	 *		CreateDWordField (Arg3, 0x0C, SUPC)
	 *		CreateDWordField (Arg3, 0x10, CTRC)
	 *		CreateDWordField (Local7, 0x10, OTRC)
	 */
	acpigen_write_create_dword_field(ARG3_OP, 0x0C, "SUPC");
	acpigen_write_create_dword_field(ARG3_OP, 0x10, "CTRC");
	acpigen_write_create_dword_field(LOCAL7_OP, 0x10, "OTRC");

	/*
	 *		// Grant CXL feature controls to OS
	 *		CTRC &= osc_features
	 */
	acpigen_write_to_integer_from_namestring("CTRC", LOCAL0_OP);
	acpigen_write_store_int_to_op(osc_features, LOCAL1_OP);
	acpigen_write_and(LOCAL0_OP, LOCAL1_OP, LOCAL0_OP);
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "CTRC");

	/*
	 *		If (CTRC != OTRC)
	 *		{
	 *			RETE = OSC_RET_CAPABILITIES_MASKED
	 *		}
	 */
	acpigen_write_if();
	acpigen_emit_byte(LNOT_OP);
	acpigen_emit_byte(LEQUAL_OP);
	acpigen_emit_namestring("CTRC");
	acpigen_emit_namestring("OTRC");
	acpigen_write_store_int_to_namestr(OSC_RET_CAPABILITIES_MASKED, "RETE");
	acpigen_write_if_end();
};

__weak uint32_t soc_get_granted_pci_features(const struct device *domain)
{
	/*
	 * By default grant no features to OS, which equals to the case where _OSC
	 * is absent.
	 *
	 * Refer to PCI firmware specification, revision 3.1.
	 * If the _OSC control method is absent from the scope of a host bridge device, then
	 * the operating system must not enable or attempt to use any features defined in this
	 * section for the hierarchy originated by the host bridge. Doing so could contend with
	 * platform firmware operations or produce undesired results.
	 */
	return 0;
}

__weak uint32_t soc_get_granted_cxl_features(const struct device *domain)
{
	/*
	 * By default grant no features to OS, which equals to the case where _OSC
	 * is absent.
	 */
	return 0;
}
