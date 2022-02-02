/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <cbmem.h>
#include <console/console.h>

#include "tpm_ppi.h"

#define BCD(x, y) (((x) << 4) | ((y) << 0))

static void set_package_element_op(const char *package_name, unsigned int element,
				   uint8_t src_op)
{
	acpigen_write_store();
	acpigen_emit_byte(src_op);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring(package_name);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
}

static void set_package_element_name(const char *package_name, unsigned int element,
				     const char *src)
{
	acpigen_write_store();
	acpigen_emit_namestring(src);
	acpigen_emit_byte(INDEX_OP);
	acpigen_emit_namestring(package_name);
	acpigen_write_integer(element);
	acpigen_emit_byte(ZERO_OP); /* Ignore Index() Destination */
}

/* PPI function is passed in src_op. Converted to Local2. Clobbers Local1 and Local2 */
static void verify_supported_ppi(uint8_t src_op)
{
	/*
	 * Old OSes incorrectly pass a Buffer instead of a Package.
	 * See TCG Physical Presence Interface Specification Chapter 8.1.2 for details.
	 */

	/* If (ObjectType(Arg3) == Package) */
	acpigen_write_store();
	acpigen_emit_byte(OBJ_TYPE_OP);
	acpigen_emit_byte(src_op);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lequal_op_int(LOCAL1_OP, 4);
	acpigen_get_package_op_element(src_op, 0, LOCAL2_OP);
	acpigen_pop_len();

	/* If (ObjectType(Arg3) == Buffer) */
	acpigen_write_store();
	acpigen_emit_byte(OBJ_TYPE_OP);
	acpigen_emit_byte(src_op);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lequal_op_int(LOCAL1_OP, 3);
	acpigen_write_to_integer(src_op, LOCAL2_OP);
	acpigen_pop_len();

	/* Check if it's a valid PPI function */
	acpigen_write_store();
	acpigen_emit_namestring("^FSUP");
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_emit_byte(CONFIG(TPM1) ? ONE_OP : ZERO_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lequal_op_int(LOCAL1_OP, 0);

	/*
	 * Note: Must fake success for 1-4, 6-13, 15-16, 19-20
	 * see "Trusted Execution Environment ACPI Profile"
	 *
	 * Even if not available, the TPM 1.2 PPI must be advertised as
	 * supported. Tests showed that Windows relies on it, even when
	 * a TPM2.0 is present!
	 * The functions aren't actually used when a TPM2.0 is present...
	 * Without this the Windows TPM 2.0 stack refuses to work.
	 */

	/*
	 * Check if we have TPM1.2 but a TPM2 PPI function was called
	 * or if we have TPM2.0 but a TPM1.2 PPI function was called.
	 */
	acpigen_write_store();
	acpigen_emit_namestring("^FSUP");
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_emit_byte(CONFIG(TPM1) ? ZERO_OP : ONE_OP);
	acpigen_emit_byte(LOCAL1_OP);

	acpigen_write_if_lequal_op_int(LOCAL1_OP, 1);
	acpigen_write_return_integer(PPI2_RET_SUCCESS);	/* As per TPM spec */
	acpigen_pop_len();
	acpigen_write_return_integer(PPI2_RET_NOT_SUPPORTED);

	acpigen_pop_len();
}

/* TPM PPI functions */

static void tpm_ppi_func0_cb(void *arg)
{
	/* Functions 1-8. */
	u8 buf[] = {0xff, 0x01};
	acpigen_write_return_byte_buffer(buf, 2);
}

 /*
 * PPI 1.0: 2.1.1 Get Physical Presence Interface Version
 *
 * Arg2 (Integer): Function Index = 1
 * Arg3 (Package): Arguments = Empty Package
 *
 * Returns: Type: String
 */
static void tpm_ppi_func1_cb(void *arg)
{
	if (CONFIG(TPM2))
		/* Interface version: 1.3 */
		acpigen_write_return_string("1.3");
	else
		/* Interface version: 1.2 */
		acpigen_write_return_string("1.2");
}

/*
 * Submit TPM Operation Request to Pre-OS Environment [Windows optional]
 * PPI 1.0: 2.1.3 Submit TPM Operation Request to Pre-OS Environment
 *
 * Supported Revisions: 1
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 2
 * Arg3 (Package): Arguments = Package: Type: Integer
 *                             Operation Value of the Request
 *
 * Returns: Type: Integer
 *          0: Success
 *          1: Operation Value of the Request Not Supported
 *          2: General Failure
 */
static void tpm_ppi_func2_cb(void *arg)
{
	/* Revision 1 */
	acpigen_write_to_integer(ARG1_OP, LOCAL0_OP);
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 1);

	/* Local2 = ConvertAndVerify(Arg3) */
	verify_supported_ppi(ARG3_OP);

	acpigen_write_store_op_to_namestr(LOCAL2_OP, "^CMDR");
	acpigen_write_store_op_to_namestr(ZERO_OP, "^OARG");
	acpigen_write_store_op_to_namestr(ZERO_OP, "^USER");

	acpigen_write_return_integer(PPI2_RET_SUCCESS);
	acpigen_pop_len();

	acpigen_write_return_integer(PPI2_RET_GENERAL_FAILURE);
}

/*
 * PPI 1.0: 2.1.4 Get Pending TPM Operation Requested By the OS
 *
 * Supported Revisions: 1, 2
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 3
 * Arg3 (Package): Empty package
 *
 * Returns: Type: Package(Integer, Integer, Integer (optional))
 *      Integer 1:
 *          0: Success
 *          1: General Failure
 *      Integer 2:
 *          Pending TPM operation requested by OS
 *      Integer 3:
 *           Pending TPM operation argument requested by OS
 */
static void tpm_ppi_func3_cb(void *arg)
{
	acpigen_write_store();
	acpigen_write_integer(PPI3_RET_GENERAL_FAILURE);
	acpigen_emit_byte(LOCAL0_OP);

	/* ^TPM3 [0] = PPI3_RET_GENERAL_FAILURE */
	set_package_element_op("^TPM3", 0, LOCAL0_OP);

	/* ^TPM2 [0] = PPI3_RET_GENERAL_FAILURE */
	set_package_element_op("^TPM2", 0, LOCAL0_OP);

	acpigen_write_to_integer(ARG1_OP, LOCAL0_OP);

	/* Revision 1 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 1);

	/* ^TPM2 [0] = PPI3_RET_SUCCESS */
	acpigen_write_store();
	acpigen_write_integer(PPI3_RET_SUCCESS);
	acpigen_emit_byte(LOCAL1_OP);
	set_package_element_op("^TPM2", 0, LOCAL1_OP);

	/* ^TPM2 [1] = ^CMDR */
	set_package_element_name("^TPM2", 1, "^CMDR");

	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("^TPM2");
	acpigen_pop_len();

	/*
	 * A return value of {0, 23, 1} indicates that operation 23
	 * with argument 1 is pending.
	 */

	/* Revision 2 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 2);

	/* ^TPM3 [0] = PPI3_RET_SUCCESS */
	acpigen_write_store();
	acpigen_write_integer(PPI3_RET_SUCCESS);
	acpigen_emit_byte(LOCAL1_OP);
	set_package_element_op("^TPM3", 0, LOCAL1_OP);

	/* ^TPM3 [1] = ^CMDR */
	set_package_element_name("^TPM3", 1, "^CMDR");

	/* ^TPM3 [2] = ^OARG */
	set_package_element_name("^TPM3", 2, "^OARG");

	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("^TPM3");
	acpigen_pop_len();

	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("^TPM3");
}

/*
 * PPI 1.0: 2.1.5 Get Platform-Specific Action to Transition to Pre-OS Environment
 *
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 4
 * Arg3 (Package): Empty package
 *
 * Returns: Type: Integer
 *          0: None
 *          1: Shutdown
 *          2: Reboot
 *          3: Vendor specific
 */
static void tpm_ppi_func4_cb(void *arg)
{
	/* Pre-OS transition method: reboot. */
	acpigen_write_return_byte(PPI4_RET_REBOOT);
}

/*
 * PPI 1.0: 2.1.6 Return TPM Operation Response to OS Environment
 *
 * Supported Revisions: 1
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 5
 * Arg3 (Package): Empty package
 *
 * Returns: Type: Package(Integer, Integer, Integer)
 *      Integer 1:
 *          0: Success
 *          1: General Failure
 *      Integer 2:
 *          Most recent TPM operation requested by OS
 *      Integer 3:
 *          Response to most recent TPM operation requested by OS
 */
static void tpm_ppi_func5_cb(void *arg)
{
	/* ^TPM3 [0] = PPI5_RET_GENERAL_FAILURE */
	acpigen_write_store();
	acpigen_write_integer(PPI5_RET_GENERAL_FAILURE);
	acpigen_emit_byte(LOCAL1_OP);
	set_package_element_op("^TPM3", 0, LOCAL1_OP);

	acpigen_write_to_integer(ARG1_OP, LOCAL0_OP);

	/* Revision 1 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 1);

	/* ^TPM3 [0] = PPI5_RET_SUCCESS */
	acpigen_write_store();
	acpigen_write_integer(PPI5_RET_SUCCESS);
	acpigen_emit_byte(LOCAL1_OP);
	set_package_element_op("^TPM3", 0, LOCAL1_OP);

	/* ^TPM3 [1] = ^LCMD */
	set_package_element_name("^TPM3", 1, "^LCMD");

	/* ^TPM3 [2] = ^RESU */
	set_package_element_name("^TPM3", 2, "^RESU");

	acpigen_pop_len();

	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring("^TPM3");
}

/*
 * PPI 1.2: 2.1.6 Submit preferred user language [Windows optional]
 *
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 5
 * Arg3 (Package): Empty package
 */
static void tpm_ppi_func6_cb(void *arg)
{
	/*
	 * Set preferred user language: deprecated and must return 3 aka
	 * "not implemented".
	 */
	acpigen_write_return_byte(PPI6_RET_NOT_IMPLEMENTED);
}

/*
 * PPI 1.2: 2.1.7 Submit TPM Operation Request to Pre-OS Environment 2
 *
 * Supported Revisions: 1, 2
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 7
 * Arg3 (Package): Integer
 *
 * Returns: Type: Integer
 *          0: Success
 *          1: Not implemented
 *          2: General Failure
 *          3: Blocked by current BIOS settings
 */
static void tpm_ppi_func7_cb(void *arg)
{
	acpigen_write_to_integer(ARG1_OP, LOCAL0_OP);

	/* Local2 = ConvertAndVerify(Arg3) */
	verify_supported_ppi(ARG3_OP);

	/* If (ObjectType(Arg3) == Buffer) */
	acpigen_write_store();
	acpigen_emit_byte(OBJ_TYPE_OP);
	acpigen_emit_byte(ARG3_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lequal_op_int(LOCAL1_OP, 3);

	/* Enforce use of Revision 1 that doesn't take an optional argument. */

	/* Local0 = One */
	acpigen_write_store();
	acpigen_emit_byte(ONE_OP);
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_pop_len();

	// FIXME: Only advertise supported functions

	/* Revision 1 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 1);

	/* ^CMDR = Local2 */
	acpigen_write_store_op_to_namestr(LOCAL2_OP, "^CMDR");

	/* ^OARG = Zero */
	acpigen_write_store_op_to_namestr(ZERO_OP, "^OARG");

	acpigen_write_return_byte(PPI7_RET_SUCCESS);
	acpigen_pop_len();

	/* Revision 2 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 2);
	/* ^CMDR = Local2 */
	acpigen_write_store_op_to_namestr(LOCAL2_OP, "^CMDR");

	/* ^OARG = Arg3 [1] */
	acpigen_get_package_op_element(ARG3_OP, 1, LOCAL3_OP);
	acpigen_write_store();
	acpigen_emit_byte(LOCAL3_OP);
	acpigen_emit_namestring("^OARG");

	acpigen_write_return_byte(PPI7_RET_SUCCESS);
	acpigen_pop_len();

	acpigen_write_return_byte(PPI7_RET_GENERAL_FAILURE);
}

/*
 * PPI 1.2: 2.1.8 Get User Confirmation Status for Operation
 *
 * Returns if a command is supported and allowed by firmware
 * Supported Revisions: 1
 * Arg1 (Integer): Revision
 * Arg2 (Integer): Function Index = 7
 * Arg3 (Package): Integer
 *
 * Returns: Type: Integer
 *          0: Not implemented
 *          1: BIOS only
 *          2: Blocked for OS by BIOS settings
 *          3: Allowed and physical present user required
 *          4: Allowed and physical present user not required
 */
static void tpm_ppi_func8_cb(void *arg)
{
	acpigen_write_to_integer(ARG1_OP, LOCAL0_OP);

	/* Revision 1 */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 1);
	acpigen_get_package_op_element(ARG3_OP, 0, LOCAL2_OP);

	/* Check if it's a valid PPI function */
	acpigen_write_store();
	acpigen_emit_namestring("^FSUP");
	acpigen_emit_byte(LOCAL2_OP);
	acpigen_emit_byte(CONFIG(TPM1) ? ONE_OP : ZERO_OP);
	acpigen_emit_byte(LOCAL1_OP);
	acpigen_write_if_lequal_op_int(LOCAL1_OP, 0);
	acpigen_write_return_byte(0);	/* Not implemented */
	acpigen_pop_len();

	// FIXME: Only advertise supported functions

	if (CONFIG(TPM1)) {
		/*
		 * Some functions do not require PP depending on configuration.
		 * Those aren't listed here, so the 'required PP' is always set for those.
		 */
		static const u32 tpm1_funcs[] = {
			TPM_NOOP,
			TPM_SET_NOPPICLEAR_TRUE,
			TPM_SET_NOPPIMAINTAINANCE_TRUE,
			TPM_SET_NOPPIPROVISION_TRUE,
		};
		for (size_t i = 0; i < ARRAY_SIZE(tpm1_funcs); i++) {
			acpigen_write_if_lequal_op_int(LOCAL2_OP, tpm1_funcs[i]);
			acpigen_write_return_integer(PPI8_RET_ALLOWED);
			acpigen_pop_len();	/* Pop : If */
		}
	} else if (CONFIG(TPM2)) {
		/*
		 * Some functions do not require PP depending on configuration.
		 * Those aren't listed here, so the 'required PP' is always set for those.
		 */
		static const u32 tpm2_funcs[] = {
			TPM2_NOOP,
			TPM2_SET_PP_REQUIRED_FOR_CLEAR_TRUE,
			TPM2_SET_PP_REQUIRED_FOR_CHANGE_PCRS_TRUE,
			TPM2_SET_PP_REQUIRED_FOR_TURN_ON_TRUE,
			TPM2_SET_PP_REQUIRED_FOR_CHANGE_EPS_TRUE,
			TPM2_SET_PP_REQUIRED_FOR_TURN_OFF_TRUE,
			TPM2_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_TRUE,
			TPM2_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_TRUE,
		};
		for (size_t i = 0; i < ARRAY_SIZE(tpm2_funcs); i++) {
			acpigen_write_if_lequal_op_int(LOCAL2_OP, tpm2_funcs[i]);
			acpigen_write_return_integer(PPI8_RET_ALLOWED);
			acpigen_pop_len();	/* Pop : If */
		}
	}
	acpigen_write_return_integer(PPI8_RET_ALLOWED_WITH_PP);

	acpigen_pop_len();

	acpigen_write_return_integer(PPI8_RET_NOT_IMPLEMENTED);
}

static void (*tpm_ppi_callbacks[])(void *) = {
	tpm_ppi_func0_cb,
	tpm_ppi_func1_cb,
	tpm_ppi_func2_cb,
	tpm_ppi_func3_cb,
	tpm_ppi_func4_cb,
	tpm_ppi_func5_cb,
	tpm_ppi_func6_cb,
	tpm_ppi_func7_cb,
	tpm_ppi_func8_cb,
};

static void tpm_mci_func0_cb(void *arg)
{
	/* Function 1. */
	acpigen_write_return_singleton_buffer(0x3);
}
static void tpm_mci_func1_cb(void *arg)
{
	/* Just return success. */
	acpigen_write_return_byte(0);
}

static void (*tpm_mci_callbacks[])(void *) = {
	tpm_mci_func0_cb,
	tpm_mci_func1_cb,
};

void tpm_ppi_acpi_fill_ssdt(const struct device *dev)
{
	struct cb_tpm_ppi_payload_handshake *ppib;

	static const struct fieldlist list[] = {
		FIELDLIST_OFFSET(0x100),// FIXME: Add support for func
		FIELDLIST_NAMESTR("PPIN", 8),// Not used
		FIELDLIST_NAMESTR("PPIP", 32),// Not used
		FIELDLIST_NAMESTR("RESU", 32),// Result of the last operation (TPM error code)
		FIELDLIST_NAMESTR("CMDR", 32),// The command requested by OS. 0 for NOP
		FIELDLIST_NAMESTR("OARG", 32),// The command optional argument requested by OS
		FIELDLIST_NAMESTR("LCMD", 32),// The last command requested by OS.
		FIELDLIST_NAMESTR("FRET", 32),// Not used
	};
	static const u8 tpm1_funcs[] = {
		TPM_NOOP,
		TPM_ENABLE,
		TPM_DISABLE,
		TPM_ACTIVATE,
		TPM_DEACTIVATE,
		TPM_CLEAR,
		TPM_ENABLE_ACTIVATE,
		TPM_DEACTIVATE_DISABLE,
		TPM_SETOWNERINSTALL_TRUE,
		TPM_SETOWNERINSTALL_FALSE,
		TPM_ENABLE_ACTIVATE_SETOWNERINSTALL_TRUE,
		TPM_SETOWNERINSTALL_FALSE_DEACTIVATE_DISABLE,
		TPM_CLEAR_ENABLE_ACTIVATE,
		TPM_SET_NOPPIPROVISION_FALSE,
		TPM_SET_NOPPIPROVISION_TRUE,
		TPM_ENABLE_ACTIVE_CLEAR,
		TPM_ENABLE_ACTIVE_CLEAR_ENABLE_ACTIVE,
	};
	static const u8 tpm2_funcs[] = {
		TPM2_NOOP,
		TPM2_ENABLE,
		TPM2_DISABLE,
		TPM2_CLEAR,
		TPM2_CLEAR_ENABLE_ACTIVE,
		TPM2_SET_PP_REQUIRED_FOR_CLEAR_TRUE,
		TPM2_SET_PP_REQUIRED_FOR_CLEAR_FALSE,
		TPM2_ENABLE_CLEAR,
		TPM2_ENABLE_CLEAR2,
		TPM2_SET_PCR_BANKS,
		TPM2_CHANGE_EPS,
		TPM2_SET_PP_REQUIRED_FOR_CHANGE_PCRS_FALSE,
		TPM2_SET_PP_REQUIRED_FOR_CHANGE_PCRS_TRUE,
		TPM2_SET_PP_REQUIRED_FOR_TURN_ON_FALSE,
		TPM2_SET_PP_REQUIRED_FOR_TURN_ON_TRUE,
		TPM2_SET_PP_REQUIRED_FOR_TURN_OFF_FALSE,
		TPM2_SET_PP_REQUIRED_FOR_TURN_OFF_TRUE,
		TPM2_SET_PP_REQUIRED_FOR_CHANGE_EPS_FALSE,
		TPM2_SET_PP_REQUIRED_FOR_CHANGE_EPS_TRUE,
		TPM2_LOG_ALL_DIGEST,
		TPM2_DISABLE_ENDORSMENT_ENABLE_STORAGE_HISTORY,
		TPM2_ENABLE_BLOCK_SID,
		TPM2_DISABLE_BLOCK_SID,
		TPM2_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_TRUE,
		TPM2_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_FALSE,
		TPM2_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_TRUE,
		TPM2_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_FALSE,
	};

	/*
	 * On hot reset/ACPI S3 the contents are preserved.
	 */
	ppib = (void *)cbmem_add(CBMEM_ID_TPM_PPI, sizeof(*ppib));
	if (!ppib) {
		printk(BIOS_ERR, "PPI: Failed to add CBMEM\n");
		return;
	}
	printk(BIOS_DEBUG, "PPI: Pending OS request: 0x%x (0x%x)\n", ppib->pprq, ppib->pprm);
	printk(BIOS_DEBUG, "PPI: OS response: CMD 0x%x = 0x%x\n", ppib->lppr, ppib->pprp);

	/* Clear unsupported fields */
	ppib->next_step = 0;
	ppib->ppin = 1; // Not used by ACPI. Read by EDK-2, must be 1.
	ppib->ppip = 0;
	ppib->fret = 0;
	ppib->next_step = 0;

	bool found = false;
	/* Fill in defaults, the TPM command executor may overwrite this list */
	memset(ppib->func, 0, sizeof(ppib->func));
	if (CONFIG(TPM1)) {
		for (size_t i = 0; i < ARRAY_SIZE(tpm1_funcs); i++) {
			ppib->func[tpm1_funcs[i]] = 1;
			if (ppib->pprq == tpm1_funcs[i])
				found = true;
		}
	} else {
		for (size_t i = 0; i < ARRAY_SIZE(tpm2_funcs); i++) {
			ppib->func[tpm2_funcs[i]] = 1;
			if (ppib->pprq == tpm2_funcs[i])
				found = true;
		}
	}
	if (!found) {
		// Set sane defaults
		ppib->pprq = 0;
		ppib->pprm = 0;
		ppib->pprp = 0;
	}

	/* Physical Presence OpRegion */
	struct opregion opreg = OPREGION("PPOP", SYSTEMMEMORY, (uintptr_t)ppib,
		sizeof(*ppib));

	acpigen_write_opregion(&opreg);
	acpigen_write_field(opreg.name, list, ARRAY_SIZE(list),
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);

	acpigen_write_name("TPM2");
	acpigen_write_package(2);
	acpigen_write_dword(0);
	acpigen_write_dword(0);
	acpigen_pop_len(); /* Package */

	acpigen_write_name("TPM3");
	acpigen_write_package(3);
	acpigen_write_dword(0);
	acpigen_write_dword(0);
	acpigen_write_dword(0);
	acpigen_pop_len(); /* Package */

	/*
	 * Returns One if the firmware implements this function.
	 *
	 * Arg0: Integer PPI function
	 */
	acpigen_write_method_serialized("FUNC", 1);

	acpigen_write_to_integer(ARG0_OP, LOCAL0_OP);
	acpigen_write_to_integer(ARG1_OP, LOCAL1_OP);
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(VENDOR_SPECIFIC_OFFSET);
	acpigen_write_return_integer(0);
	acpigen_pop_len(); /* If */

	/* TPPF = CreateField("PPOP", Local0) */
	acpigen_emit_byte(CREATE_BYTE_OP);
	acpigen_emit_namestring("PPOP");
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_emit_namestring("TPPF");

	/* Local0 = ToInteger("TPPF") */
	acpigen_emit_byte(TO_INTEGER_OP);
	acpigen_emit_namestring("TPPF");
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_write_return_op(LOCAL0_OP);
	acpigen_pop_len(); /* Method */

	/*
	 * Returns One if the PPI spec supports this functions.
	 * That doesn't necessarily mean that the firmware implemtents it, or the
	 * TPM can execute the function.
	 *
	 * Arg0: Integer PPI function
	 * Arg1: Integer TPMversion (0: TPM2, 1: TPM1.2)
	 */
	acpigen_write_method("FSUP", 2);

	acpigen_write_to_integer(ARG0_OP, LOCAL0_OP);
	acpigen_write_to_integer(ARG1_OP, LOCAL1_OP);
	acpigen_write_if();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_integer(VENDOR_SPECIFIC_OFFSET);
	acpigen_write_return_integer(0);
	acpigen_pop_len(); /* If */

	acpigen_write_if_lequal_op_int(LOCAL1_OP, 1);
	for (size_t i = 0; i < ARRAY_SIZE(tpm1_funcs); i++) {
		acpigen_write_if_lequal_op_int(LOCAL0_OP, tpm1_funcs[i]);
		acpigen_write_return_integer(1);
		acpigen_pop_len();	/* Pop : If */
	}
	acpigen_pop_len(); /* If */

	acpigen_write_if_lequal_op_int(LOCAL1_OP, 0);

	for (size_t i = 0; i < ARRAY_SIZE(tpm2_funcs); i++) {
		acpigen_write_if_lequal_op_int(LOCAL0_OP, tpm2_funcs[i]);
		acpigen_write_return_integer(1);
		acpigen_pop_len();	/* Pop : If */
	}
	acpigen_pop_len(); /* If */

	acpigen_write_return_integer(0);
	acpigen_pop_len(); /* Method */

	/*
	 * _DSM method
	 */
	struct dsm_uuid ids[] = {
		/* Physical presence interface.
		 * This is used to submit commands like "Clear TPM" to
		 * be run at next reboot provided that user confirms
		 * them.
		 */
		DSM_UUID(TPM_PPI_UUID, &tpm_ppi_callbacks[0],
			ARRAY_SIZE(tpm_ppi_callbacks), NULL),
		/* Memory clearing on boot: just a dummy. */
		DSM_UUID(TPM_MCI_UUID, &tpm_mci_callbacks[0],
			ARRAY_SIZE(tpm_mci_callbacks), NULL),
	};

	acpigen_write_dsm_uuid_arr(ids, ARRAY_SIZE(ids));
}

void lb_tpm_ppi(struct lb_header *header)
{
	struct lb_tpm_physical_presence *tpm_ppi;
	void *ppib;

	ppib = cbmem_find(CBMEM_ID_TPM_PPI);
	if (!ppib)
		return;

	tpm_ppi = (struct lb_tpm_physical_presence *)lb_new_record(header);
	tpm_ppi->tag = LB_TAG_TPM_PPI_HANDOFF;
	tpm_ppi->size = sizeof(*tpm_ppi);
	tpm_ppi->ppi_address = (uintptr_t)ppib;
	tpm_ppi->tpm_version = CONFIG(TPM1) ? LB_TPM_VERSION_TPM_VERSION_1_2 :
		LB_TPM_VERSION_TPM_VERSION_2;

	tpm_ppi->ppi_version = BCD(1, 3);
}
