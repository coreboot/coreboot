/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include "sysincludes.h"

#include "crc8.h"
#include "rollback_index.h"
#include "tlcl.h"
#include "tss_constants.h"
#include "utility.h"
#include "vboot_api.h"

#ifndef offsetof
#define offsetof(A,B) __builtin_offsetof(A,B)
#endif

/*
 * Provide protoypes for functions not in the header file. These prototypes
 * fix -Wmissing-prototypes warnings.
 */
uint32_t ReadSpaceFirmware(RollbackSpaceFirmware *rsf);
uint32_t WriteSpaceFirmware(RollbackSpaceFirmware *rsf);
uint32_t ReadSpaceKernel(RollbackSpaceKernel *rsk);
uint32_t WriteSpaceKernel(RollbackSpaceKernel *rsk);

#ifdef FOR_TEST
/*
 * Compiling for unit test, so we need the real implementations of
 * rollback functions.  The unit test mocks the underlying tlcl
 * functions, so this is ok to run on the host.
 */
#undef CHROMEOS_ENVIRONMENT
#undef DISABLE_ROLLBACK_TPM
#endif

#define RETURN_ON_FAILURE(tpm_command) do {				\
		uint32_t result_;					\
		if ((result_ = (tpm_command)) != TPM_SUCCESS) {		\
			VBDEBUG(("Rollback: %08x returned by " #tpm_command \
				 "\n", (int)result_));			\
			return result_;					\
		}							\
	} while (0)


uint32_t TPMClearAndReenable(void)
{
	VBDEBUG(("TPM: Clear and re-enable\n"));
	RETURN_ON_FAILURE(TlclForceClear());
	RETURN_ON_FAILURE(TlclSetEnable());
	RETURN_ON_FAILURE(TlclSetDeactivated(0));

	return TPM_SUCCESS;
}

uint32_t SafeWrite(uint32_t index, const void *data, uint32_t length)
{
	uint32_t result = TlclWrite(index, data, length);
	if (result == TPM_E_MAXNVWRITES) {
		RETURN_ON_FAILURE(TPMClearAndReenable());
		return TlclWrite(index, data, length);
	} else {
		return result;
	}
}

uint32_t SafeDefineSpace(uint32_t index, uint32_t perm, uint32_t size)
{
	uint32_t result = TlclDefineSpace(index, perm, size);
	if (result == TPM_E_MAXNVWRITES) {
		RETURN_ON_FAILURE(TPMClearAndReenable());
		return TlclDefineSpace(index, perm, size);
	} else {
		return result;
	}
}

/* Functions to read and write firmware and kernel spaces. */
uint32_t ReadSpaceFirmware(RollbackSpaceFirmware *rsf)
{
	uint32_t r;
	int attempts = 3;

	while (attempts--) {
		r = TlclRead(FIRMWARE_NV_INDEX, rsf,
			     sizeof(RollbackSpaceFirmware));
		if (r != TPM_SUCCESS)
			return r;

		/*
		 * No CRC in this version, so we'll create one when we write
		 * it. Note that we're marking this as version 2, not
		 * ROLLBACK_SPACE_FIRMWARE_VERSION, because version 2 just
		 * added the CRC. Later versions will need to set default
		 * values for any extra fields explicitly (probably here).
		 */
		if (rsf->struct_version < 2) {
			/* Danger Will Robinson! Danger! */
			rsf->struct_version = 2;
			return TPM_SUCCESS;
		}

		/*
		 * If the CRC is good, we're done. If it's bad, try a couple
		 * more times to see if it gets better before we give up. It
		 * could just be noise.
		 */
		if (rsf->crc8 == Crc8(rsf,
				      offsetof(RollbackSpaceFirmware, crc8)))
			return TPM_SUCCESS;

		VBDEBUG(("TPM: %s() - bad CRC\n", __func__));
	}

	VBDEBUG(("TPM: %s() - too many bad CRCs, giving up\n", __func__));
	return TPM_E_CORRUPTED_STATE;
}

uint32_t WriteSpaceFirmware(RollbackSpaceFirmware *rsf)
{
	RollbackSpaceFirmware rsf2;
	uint32_t r;
	int attempts = 3;

	/* All writes should use struct_version 2 or greater. */
	if (rsf->struct_version < 2)
		rsf->struct_version = 2;
	rsf->crc8 = Crc8(rsf, offsetof(RollbackSpaceFirmware, crc8));

	while (attempts--) {
		r = SafeWrite(FIRMWARE_NV_INDEX, rsf,
			      sizeof(RollbackSpaceFirmware));
		/* Can't write, not gonna try again */
		if (r != TPM_SUCCESS)
			return r;

		/* Read it back to be sure it got the right values. */
		r = ReadSpaceFirmware(&rsf2);    /* This checks the CRC */
		if (r == TPM_SUCCESS)
			return r;

		VBDEBUG(("TPM: %s() - bad CRC\n", __func__));
		/* Try writing it again. Maybe it was garbled on the way out. */
	}

	VBDEBUG(("TPM: %s() - too many bad CRCs, giving up\n", __func__));
	return TPM_E_CORRUPTED_STATE;
}

uint32_t SetVirtualDevMode(int val)
{
	RollbackSpaceFirmware rsf;

	VBDEBUG(("TPM: Entering %s()\n", __func__));
	if (TPM_SUCCESS != ReadSpaceFirmware(&rsf))
		return VBERROR_TPM_FIRMWARE_SETUP;

	VBDEBUG(("TPM: flags were 0x%02x\n", rsf.flags));
	if (val)
		rsf.flags |= FLAG_VIRTUAL_DEV_MODE_ON;
	else
		rsf.flags &= ~FLAG_VIRTUAL_DEV_MODE_ON;
	/*
	 * NOTE: This doesn't update the FLAG_LAST_BOOT_DEVELOPER bit.  That
	 * will be done by SetupTPM() on the next boot.
	 */
	VBDEBUG(("TPM: flags are now 0x%02x\n", rsf.flags));

	if (TPM_SUCCESS != WriteSpaceFirmware(&rsf))
		return VBERROR_TPM_SET_BOOT_MODE_STATE;

	VBDEBUG(("TPM: Leaving %s()\n", __func__));
	return VBERROR_SUCCESS;
}

uint32_t ReadSpaceKernel(RollbackSpaceKernel *rsk)
{
	uint32_t r;
	int attempts = 3;

	while (attempts--) {
		r = TlclRead(KERNEL_NV_INDEX, rsk, sizeof(RollbackSpaceKernel));
		if (r != TPM_SUCCESS)
			return r;

		/*
		 * No CRC in this version, so we'll create one when we write
		 * it. Note that we're marking this as version 2, not
		 * ROLLBACK_SPACE_KERNEL_VERSION, because version 2 just added
		 * the CRC. Later versions will need to set default values for
		 * any extra fields explicitly (probably here).
		 */
		if (rsk->struct_version < 2) {
			/* Danger Will Robinson! Danger! */
			rsk->struct_version = 2;
			return TPM_SUCCESS;
		}

		/*
		 * If the CRC is good, we're done. If it's bad, try a couple
		 * more times to see if it gets better before we give up. It
		 * could just be noise.
		 */
		if (rsk->crc8 == Crc8(rsk, offsetof(RollbackSpaceKernel, crc8)))
			return TPM_SUCCESS;

		VBDEBUG(("TPM: %s() - bad CRC\n", __func__));
	}

	VBDEBUG(("TPM: %s() - too many bad CRCs, giving up\n", __func__));
	return TPM_E_CORRUPTED_STATE;
}

uint32_t WriteSpaceKernel(RollbackSpaceKernel *rsk)
{
	RollbackSpaceKernel rsk2;
	uint32_t r;
	int attempts = 3;

	/* All writes should use struct_version 2 or greater. */
	if (rsk->struct_version < 2)
		rsk->struct_version = 2;
	rsk->crc8 = Crc8(rsk, offsetof(RollbackSpaceKernel, crc8));

	while (attempts--) {
		r = SafeWrite(KERNEL_NV_INDEX, rsk,
			      sizeof(RollbackSpaceKernel));
		/* Can't write, not gonna try again */
		if (r != TPM_SUCCESS)
			return r;

		/* Read it back to be sure it got the right values. */
		r = ReadSpaceKernel(&rsk2);    /* This checks the CRC */
		if (r == TPM_SUCCESS)
			return r;

		VBDEBUG(("TPM: %s() - bad CRC\n", __func__));
		/* Try writing it again. Maybe it was garbled on the way out. */
	}

	VBDEBUG(("TPM: %s() - too many bad CRCs, giving up\n", __func__));
	return TPM_E_CORRUPTED_STATE;
}

uint32_t OneTimeInitializeTPM(RollbackSpaceFirmware *rsf,
                              RollbackSpaceKernel *rsk)
{
	static const RollbackSpaceFirmware rsf_init = {
		.struct_version = ROLLBACK_SPACE_FIRMWARE_VERSION,
	};
	static const RollbackSpaceKernel rsk_init = {
		.struct_version = ROLLBACK_SPACE_KERNEL_VERSION,
		.uid = ROLLBACK_SPACE_KERNEL_UID,
	};
	TPM_PERMANENT_FLAGS pflags;
	uint32_t result;

	VBDEBUG(("TPM: One-time initialization\n"));

	/*
	 * Do a full test.  This only happens the first time the device is
	 * turned on in the factory, so performance is not an issue.  This is
	 * almost certainly not necessary, but it gives us more confidence
	 * about some code paths below that are difficult to
	 * test---specifically the ones that set lifetime flags, and are only
	 * executed once per physical TPM.
	 */
	result = TlclSelfTestFull();
	if (result != TPM_SUCCESS)
		return result;

	result = TlclGetPermanentFlags(&pflags);
	if (result != TPM_SUCCESS)
		return result;

	/*
	 * TPM may come from the factory without physical presence finalized.
	 * Fix if necessary.
	 */
	VBDEBUG(("TPM: physicalPresenceLifetimeLock=%d\n",
		 pflags.physicalPresenceLifetimeLock));
	if (!pflags.physicalPresenceLifetimeLock) {
		VBDEBUG(("TPM: Finalizing physical presence\n"));
		RETURN_ON_FAILURE(TlclFinalizePhysicalPresence());
	}

	/*
	 * The TPM will not enforce the NV authorization restrictions until the
	 * execution of a TPM_NV_DefineSpace with the handle of
	 * TPM_NV_INDEX_LOCK.  Here we create that space if it doesn't already
	 * exist. */
	VBDEBUG(("TPM: nvLocked=%d\n", pflags.nvLocked));
	if (!pflags.nvLocked) {
		VBDEBUG(("TPM: Enabling NV locking\n"));
		RETURN_ON_FAILURE(TlclSetNvLocked());
	}

	/* Clear TPM owner, in case the TPM is already owned for some reason. */
	VBDEBUG(("TPM: Clearing owner\n"));
	RETURN_ON_FAILURE(TPMClearAndReenable());

	/* Initializes the firmware and kernel spaces */
	Memcpy(rsf, &rsf_init, sizeof(RollbackSpaceFirmware));
	Memcpy(rsk, &rsk_init, sizeof(RollbackSpaceKernel));

	/* Define the backup space. No need to initialize it, though. */
	RETURN_ON_FAILURE(SafeDefineSpace(
			BACKUP_NV_INDEX, TPM_NV_PER_PPWRITE, BACKUP_NV_SIZE));

	/* Define and initialize the kernel space */
	RETURN_ON_FAILURE(SafeDefineSpace(KERNEL_NV_INDEX, TPM_NV_PER_PPWRITE,
					  sizeof(RollbackSpaceKernel)));
	RETURN_ON_FAILURE(WriteSpaceKernel(rsk));

	/* Do the firmware space last, so we retry if we don't get this far. */
	RETURN_ON_FAILURE(SafeDefineSpace(
			FIRMWARE_NV_INDEX,
			TPM_NV_PER_GLOBALLOCK | TPM_NV_PER_PPWRITE,
			sizeof(RollbackSpaceFirmware)));
	RETURN_ON_FAILURE(WriteSpaceFirmware(rsf));

	return TPM_SUCCESS;
}


/*
 * SetupTPM starts the TPM and establishes the root of trust for the
 * anti-rollback mechanism.  SetupTPM can fail for three reasons.  1 A bug. 2 a
 * TPM hardware failure. 3 An unexpected TPM state due to some attack.  In
 * general we cannot easily distinguish the kind of failure, so our strategy is
 * to reboot in recovery mode in all cases.  The recovery mode calls SetupTPM
 * again, which executes (almost) the same sequence of operations.  There is a
 * good chance that, if recovery mode was entered because of a TPM failure, the
 * failure will repeat itself.  (In general this is impossible to guarantee
 * because we have no way of creating the exact TPM initial state at the
 * previous boot.)  In recovery mode, we ignore the failure and continue, thus
 * giving the recovery kernel a chance to fix things (that's why we don't set
 * bGlobalLock).  The choice is between a knowingly insecure device and a
 * bricked device.
 *
 * As a side note, observe that we go through considerable hoops to avoid using
 * the STCLEAR permissions for the index spaces.  We do this to avoid writing
 * to the TPM flashram at every reboot or wake-up, because of concerns about
 * the durability of the NVRAM.
 */
uint32_t SetupTPM(int developer_mode, int disable_dev_request,
                  int clear_tpm_owner_request, RollbackSpaceFirmware* rsf)
{
	uint8_t in_flags;
	uint8_t disable;
	uint8_t deactivated;
	uint32_t result;
	uint32_t versions;

	RETURN_ON_FAILURE(TlclLibInit());

#ifdef TEGRA_SOFT_REBOOT_WORKAROUND
	result = TlclStartup();
	if (result == TPM_E_INVALID_POSTINIT) {
		/*
		 * Some prototype hardware doesn't reset the TPM on a CPU
		 * reset.  We do a hard reset to get around this.
		 */
		VBDEBUG(("TPM: soft reset detected\n", result));
		return TPM_E_MUST_REBOOT;
	} else if (result != TPM_SUCCESS) {
		VBDEBUG(("TPM: TlclStartup returned %08x\n", result));
		return result;
	}
#else
	RETURN_ON_FAILURE(TlclStartup());
#endif

  /*
   * Some TPMs start the self test automatically at power on.  In that case we
   * don't need to call ContinueSelfTest.  On some (other) TPMs,
   * ContinueSelfTest may block.  In that case, we definitely don't want to
   * call it here.  For TPMs in the intersection of these two sets, we're
   * screwed.  (In other words: TPMs that require manually starting the
   * self-test AND block will have poor performance until we split
   * TlclSendReceive() into Send() and Receive(), and have a state machine to
   * control setup.)
   *
   * This comment is likely to become obsolete in the near future, so don't
   * trust it.  It may have not been updated.
   */
#ifdef TPM_MANUAL_SELFTEST
#ifdef TPM_BLOCKING_CONTINUESELFTEST
#warning "lousy TPM!"
#endif
	RETURN_ON_FAILURE(TlclContinueSelfTest());
#endif
	result = TlclAssertPhysicalPresence();
	if (result != TPM_SUCCESS) {
		/*
		 * It is possible that the TPM was delivered with the physical
		 * presence command disabled.  This tries enabling it, then
		 * tries asserting PP again.
		 */
		RETURN_ON_FAILURE(TlclPhysicalPresenceCMDEnable());
		RETURN_ON_FAILURE(TlclAssertPhysicalPresence());
	}

	/* Check that the TPM is enabled and activated. */
	RETURN_ON_FAILURE(TlclGetFlags(&disable, &deactivated, NULL));
	if (disable || deactivated) {
		VBDEBUG(("TPM: disabled (%d) or deactivated (%d).  Fixing...\n",
			 disable, deactivated));
		RETURN_ON_FAILURE(TlclSetEnable());
		RETURN_ON_FAILURE(TlclSetDeactivated(0));
		VBDEBUG(("TPM: Must reboot to re-enable\n"));
		return TPM_E_MUST_REBOOT;
	}

	/* Read the firmware space. */
	result = ReadSpaceFirmware(rsf);
	if (TPM_E_BADINDEX == result) {
		RollbackSpaceKernel rsk;

		/*
		 * This is the first time we've run, and the TPM has not been
		 * initialized.  Initialize it.
		 */
		VBDEBUG(("TPM: Not initialized yet.\n"));
		RETURN_ON_FAILURE(OneTimeInitializeTPM(rsf, &rsk));
	} else if (TPM_SUCCESS != result) {
		VBDEBUG(("TPM: Firmware space in a bad state; giving up.\n"));
		return TPM_E_CORRUPTED_STATE;
	}
	Memcpy(&versions, &rsf->fw_versions, sizeof(versions));
	VBDEBUG(("TPM: Firmware space sv%d f%x v%x\n",
		 rsf->struct_version, rsf->flags, versions));
	in_flags = rsf->flags;

	/* If we've been asked to clear the virtual dev-mode flag, do so now */
	if (disable_dev_request) {
		rsf->flags &= ~FLAG_VIRTUAL_DEV_MODE_ON;
		VBDEBUG(("TPM: Clearing virt dev-switch: f%x\n", rsf->flags));
	}

	/*
	 * The developer_mode value that's passed in is only set by a hardware
	 * dev-switch. We should OR it with the virtual switch, whether or not
	 * the virtual switch is used. If it's not used, it shouldn't change,
	 * so it doesn't matter.
	 */
	if (rsf->flags & FLAG_VIRTUAL_DEV_MODE_ON)
		developer_mode = 1;

	/*
	 * Clear ownership if developer flag has toggled, or if an owner-clear
	 * has been requested.
	 */
	if ((developer_mode ? FLAG_LAST_BOOT_DEVELOPER : 0) !=
	    (in_flags & FLAG_LAST_BOOT_DEVELOPER)) {
		VBDEBUG(("TPM: Developer flag changed; clearing owner.\n"));
		RETURN_ON_FAILURE(TPMClearAndReenable());
	} else if (clear_tpm_owner_request) {
		VBDEBUG(("TPM: Clearing owner as specifically requested.\n"));
		RETURN_ON_FAILURE(TPMClearAndReenable());
	}

	if (developer_mode)
		rsf->flags |= FLAG_LAST_BOOT_DEVELOPER;
	else
		rsf->flags &= ~FLAG_LAST_BOOT_DEVELOPER;


	/* If firmware space is dirty, flush it back to the TPM */
	if (rsf->flags != in_flags) {
		VBDEBUG(("TPM: Updating firmware space.\n"));
		RETURN_ON_FAILURE(WriteSpaceFirmware(rsf));
	}

	VBDEBUG(("TPM: SetupTPM() succeeded\n"));
	return TPM_SUCCESS;
}


#ifdef DISABLE_ROLLBACK_TPM
/* Dummy implementations which don't support TPM rollback protection */

uint32_t RollbackS3Resume(void)
{
#ifndef CHROMEOS_ENVIRONMENT
	/*
	 * Initialize the TPM, but ignore return codes.  In ChromeOS
	 * environment, don't even talk to the TPM.
	 */
	TlclLibInit();
	TlclResume();
#endif
	return TPM_SUCCESS;
}

uint32_t RollbackFirmwareSetup(int is_hw_dev,
                               int disable_dev_request,
                               int clear_tpm_owner_request,
                               int *is_virt_dev, uint32_t *version)
{
#ifndef CHROMEOS_ENVIRONMENT
	/*
	 * Initialize the TPM, but ignores return codes.  In ChromeOS
	 * environment, don't even talk to the TPM.
	 */
	TlclLibInit();
	TlclStartup();
	TlclContinueSelfTest();
#endif
	*is_virt_dev = 0;
	*version = 0;
	return TPM_SUCCESS;
}

uint32_t RollbackFirmwareWrite(uint32_t version)
{
	return TPM_SUCCESS;
}

uint32_t RollbackFirmwareLock(void)
{
	return TPM_SUCCESS;
}

uint32_t RollbackKernelRead(uint32_t* version)
{
	*version = 0;
	return TPM_SUCCESS;
}

uint32_t RollbackKernelWrite(uint32_t version)
{
	return TPM_SUCCESS;
}

uint32_t RollbackBackupRead(uint8_t *raw)
{
	return TPM_SUCCESS;
}

uint32_t RollbackBackupWrite(uint8_t *raw)
{
	return TPM_SUCCESS;
}

uint32_t RollbackKernelLock(int recovery_mode)
{
	return TPM_SUCCESS;
}

#else

uint32_t RollbackS3Resume(void)
{
	uint32_t result;
	RETURN_ON_FAILURE(TlclLibInit());
	result = TlclResume();
	if (result == TPM_E_INVALID_POSTINIT) {
		/*
		 * We're on a platform where the TPM maintains power in S3, so
		 * it's already initialized.
		 */
		return TPM_SUCCESS;
	}
	return result;
}

uint32_t RollbackFirmwareSetup(int is_hw_dev,
                               int disable_dev_request,
                               int clear_tpm_owner_request,
                               int *is_virt_dev, uint32_t *version)
{
	RollbackSpaceFirmware rsf;

	/* Set version to 0 in case we fail */
	*version = 0;

	RETURN_ON_FAILURE(SetupTPM(is_hw_dev, disable_dev_request,
				   clear_tpm_owner_request, &rsf));
	Memcpy(version, &rsf.fw_versions, sizeof(*version));
	*is_virt_dev = (rsf.flags & FLAG_VIRTUAL_DEV_MODE_ON) ? 1 : 0;
	VBDEBUG(("TPM: RollbackFirmwareSetup %x\n", (int)*version));
	return TPM_SUCCESS;
}

uint32_t RollbackFirmwareWrite(uint32_t version)
{
	RollbackSpaceFirmware rsf;
	uint32_t old_version;

	RETURN_ON_FAILURE(ReadSpaceFirmware(&rsf));
	Memcpy(&old_version, &rsf.fw_versions, sizeof(old_version));
	VBDEBUG(("TPM: RollbackFirmwareWrite %x --> %x\n", (int)old_version,
		 (int)version));
	Memcpy(&rsf.fw_versions, &version, sizeof(version));
	return WriteSpaceFirmware(&rsf);
}

uint32_t RollbackFirmwareLock(void)
{
	return TlclSetGlobalLock();
}

uint32_t RollbackKernelRead(uint32_t* version)
{
	RollbackSpaceKernel rsk;
	uint32_t perms, uid;

	/*
	 * Read the kernel space and verify its permissions.  If the kernel
	 * space has the wrong permission, or it doesn't contain the right
	 * identifier, we give up.  This will need to be fixed by the
	 * recovery kernel.  We have to worry about this because at any time
	 * (even with PP turned off) the TPM owner can remove and redefine a
	 * PP-protected space (but not write to it).
	 */
	RETURN_ON_FAILURE(ReadSpaceKernel(&rsk));
	RETURN_ON_FAILURE(TlclGetPermissions(KERNEL_NV_INDEX, &perms));
	Memcpy(&uid, &rsk.uid, sizeof(uid));
	if (TPM_NV_PER_PPWRITE != perms || ROLLBACK_SPACE_KERNEL_UID != uid)
		return TPM_E_CORRUPTED_STATE;

	Memcpy(version, &rsk.kernel_versions, sizeof(*version));
	VBDEBUG(("TPM: RollbackKernelRead %x\n", (int)*version));
	return TPM_SUCCESS;
}

uint32_t RollbackKernelWrite(uint32_t version)
{
	RollbackSpaceKernel rsk;
	uint32_t old_version;
	RETURN_ON_FAILURE(ReadSpaceKernel(&rsk));
	Memcpy(&old_version, &rsk.kernel_versions, sizeof(old_version));
	VBDEBUG(("TPM: RollbackKernelWrite %x --> %x\n",
		 (int)old_version, (int)version));
	Memcpy(&rsk.kernel_versions, &version, sizeof(version));
	return WriteSpaceKernel(&rsk);
}

/*
 * We don't really care whether the TPM owner has been messing with this or
 * not. We lock it along with the Kernel space just to avoid problems, but it's
 * only useful in dev-mode and only when the battery has been drained
 * completely. There aren't any security issues. It's just in the TPM because
 * we don't have any other place to keep it.
 */
uint32_t RollbackBackupRead(uint8_t *raw)
{
	uint32_t r;
	r = TlclRead(BACKUP_NV_INDEX, raw, BACKUP_NV_SIZE);
	VBDEBUG(("TPM: %s returning 0x%x\n", __func__, r));
	return r;
}

uint32_t RollbackBackupWrite(uint8_t *raw)
{
	uint32_t r;
	r = TlclWrite(BACKUP_NV_INDEX, raw, BACKUP_NV_SIZE);
	VBDEBUG(("TPM: %s returning 0x%x\n", __func__, r));
	return r;
}

uint32_t RollbackKernelLock(int recovery_mode)
{
	if (recovery_mode)
		return TPM_SUCCESS;
	else
		return TlclLockPhysicalPresence();
}

#endif /* DISABLE_ROLLBACK_TPM */
