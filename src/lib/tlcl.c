/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* A lightweight TPM command library.
 *
 * The general idea is that TPM commands are array of bytes whose
 * fields are mostly compile-time constant.  The goal is to build much
 * of the commands at compile time (or build time) and change some of
 * the fields at run time as needed.  The code in
 * utility/tlcl_generator.c builds structures containing the commands,
 * as well as the offsets of the fields that need to be set at run
 * time.
 */

#include "sysincludes.h"

#include "tlcl.h"
#include "tlcl_internal.h"
#include "tlcl_structures.h"
#include "utility.h"
#include "vboot_api.h"

#ifdef FOR_TEST
/* Allow unit testing implementation of TlclSendReceive() */
#undef CHROMEOS_ENVIRONMENT
#endif

/* Sets the size field of a TPM command. */
static inline void SetTpmCommandSize(uint8_t* buffer, uint32_t size) {
  ToTpmUint32(buffer + sizeof(uint16_t), size);
}

/* Gets the size field of a TPM command. */
__attribute__((unused))
static inline int TpmCommandSize(const uint8_t* buffer) {
  uint32_t size;
  FromTpmUint32(buffer + sizeof(uint16_t), &size);
  return (int) size;
}

/* Gets the size field of a TPM request or response. */
int TlclPacketSize(const uint8_t* packet) {
  return TpmCommandSize(packet);
}

/* Gets the code field of a TPM command. */
static inline int TpmCommandCode(const uint8_t* buffer) {
  uint32_t code;
  FromTpmUint32(buffer + sizeof(uint16_t) + sizeof(uint32_t), &code);
  return code;
}

/* Gets the return code field of a TPM result. */
static inline int TpmReturnCode(const uint8_t* buffer) {
  return TpmCommandCode(buffer);
}

/* Like TlclSendReceive below, but do not retry if NEEDS_SELFTEST or
 * DOING_SELFTEST errors are returned.
 */
static uint32_t TlclSendReceiveNoRetry(const uint8_t* request,
                                       uint8_t* response, int max_length) {

  uint32_t response_length = max_length;
  uint32_t result;

#ifdef EXTRA_LOGGING
  VBDEBUG(("TPM: command: %x%x %x%x%x%x %x%x%x%x\n",
           request[0], request[1],
           request[2], request[3], request[4], request[5],
           request[6], request[7], request[8], request[9]));
#endif

  result = VbExTpmSendReceive(request, TpmCommandSize(request),
                              response, &response_length);
  if (0 != result) {
    /* Communication with TPM failed, so response is garbage */
    VBDEBUG(("TPM: command 0x%x send/receive failed: 0x%x\n",
             TpmCommandCode(request), result));
    return result;
  }
  /* Otherwise, use the result code from the response */
  result = TpmReturnCode(response);

  /* TODO: add paranoia about returned response_length vs. max_length
   * (and possibly expected length from the response header).  See
   * crosbug.com/17017 */

#ifdef EXTRA_LOGGING
  VBDEBUG(("TPM: response: %x%x %x%x%x%x %x%x%x%x\n",
           response[0], response[1],
           response[2], response[3], response[4], response[5],
           response[6], response[7], response[8], response[9]));
#endif

  VBDEBUG(("TPM: command 0x%x returned 0x%x\n",
           TpmCommandCode(request), result));

  return result;
}


/* Sends a TPM command and gets a response.  Returns 0 if success or the TPM
 * error code if error. In the firmware, waits for the self test to complete
 * if needed. In the host, reports the first error without retries. */
uint32_t TlclSendReceive(const uint8_t* request, uint8_t* response,
                         int max_length) {
  uint32_t result = TlclSendReceiveNoRetry(request, response, max_length);
  /* When compiling for the firmware, hide command failures due to the self
   * test not having run or completed. */
#ifndef CHROMEOS_ENVIRONMENT
  /* If the command fails because the self test has not completed, try it
   * again after attempting to ensure that the self test has completed. */
  if (result == TPM_E_NEEDS_SELFTEST || result == TPM_E_DOING_SELFTEST) {
    result = TlclContinueSelfTest();
    if (result != TPM_SUCCESS) {
      return result;
    }
#if defined(TPM_BLOCKING_CONTINUESELFTEST) || defined(VB_RECOVERY_MODE)
    /* Retry only once */
    result = TlclSendReceiveNoRetry(request, response, max_length);
#else
    /* This needs serious testing.  The TPM specification says: "iii. The
     * caller MUST wait for the actions of TPM_ContinueSelfTest to complete
     * before reissuing the command C1."  But, if ContinueSelfTest is
     * non-blocking, how do we know that the actions have completed other than
     * trying again? */
    do {
      result = TlclSendReceiveNoRetry(request, response, max_length);
    } while (result == TPM_E_DOING_SELFTEST);
#endif
  }
#endif  /* ! defined(CHROMEOS_ENVIRONMENT) */
  return result;
}

/* Sends a command and returns the error code. */
static uint32_t Send(const uint8_t* command) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  return TlclSendReceive(command, response, sizeof(response));
}

/* Exported functions. */

uint32_t TlclLibInit(void) {
  return VbExTpmInit();
}

uint32_t TlclLibClose(void) {
  return VbExTpmClose();
}

uint32_t TlclStartup(void) {
  VBDEBUG(("TPM: Startup\n"));
  return Send(tpm_startup_cmd.buffer);
}

uint32_t TlclSaveState(void) {
  VBDEBUG(("TPM: SaveState\n"));
  return Send(tpm_savestate_cmd.buffer);
}

uint32_t TlclResume(void) {
  VBDEBUG(("TPM: Resume\n"));
  return Send(tpm_resume_cmd.buffer);
}

uint32_t TlclSelfTestFull(void) {
  VBDEBUG(("TPM: Self test full\n"));
  return Send(tpm_selftestfull_cmd.buffer);
}

uint32_t TlclContinueSelfTest(void) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  VBDEBUG(("TPM: Continue self test\n"));
  /* Call the No Retry version of SendReceive to avoid recursion. */
  return TlclSendReceiveNoRetry(tpm_continueselftest_cmd.buffer,
                                response, sizeof(response));
}

uint32_t TlclDefineSpace(uint32_t index, uint32_t perm, uint32_t size) {
  struct s_tpm_nv_definespace_cmd cmd;
  VBDEBUG(("TPM: TlclDefineSpace(0x%x, 0x%x, %d)\n", index, perm, size));
  Memcpy(&cmd, &tpm_nv_definespace_cmd, sizeof(cmd));
  ToTpmUint32(cmd.buffer + tpm_nv_definespace_cmd.index, index);
  ToTpmUint32(cmd.buffer + tpm_nv_definespace_cmd.perm, perm);
  ToTpmUint32(cmd.buffer + tpm_nv_definespace_cmd.size, size);
  return Send(cmd.buffer);
}

uint32_t TlclWrite(uint32_t index, const void* data, uint32_t length) {
  struct s_tpm_nv_write_cmd cmd;
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  const int total_length =
    kTpmRequestHeaderLength + kWriteInfoLength + length;

  VBDEBUG(("TPM: TlclWrite(0x%x, %d)\n", index, length));
  Memcpy(&cmd, &tpm_nv_write_cmd, sizeof(cmd));
  VbAssert(total_length <= TPM_LARGE_ENOUGH_COMMAND_SIZE);
  SetTpmCommandSize(cmd.buffer, total_length);

  ToTpmUint32(cmd.buffer + tpm_nv_write_cmd.index, index);
  ToTpmUint32(cmd.buffer + tpm_nv_write_cmd.length, length);
  Memcpy(cmd.buffer + tpm_nv_write_cmd.data, data, length);

  return TlclSendReceive(cmd.buffer, response, sizeof(response));
}

uint32_t TlclRead(uint32_t index, void* data, uint32_t length) {
  struct s_tpm_nv_read_cmd cmd;
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint32_t result_length;
  uint32_t result;

  VBDEBUG(("TPM: TlclRead(0x%x, %d)\n", index, length));
  Memcpy(&cmd, &tpm_nv_read_cmd, sizeof(cmd));
  ToTpmUint32(cmd.buffer + tpm_nv_read_cmd.index, index);
  ToTpmUint32(cmd.buffer + tpm_nv_read_cmd.length, length);

  result = TlclSendReceive(cmd.buffer, response, sizeof(response));
  if (result == TPM_SUCCESS && length > 0) {
    uint8_t* nv_read_cursor = response + kTpmResponseHeaderLength;
    FromTpmUint32(nv_read_cursor, &result_length);
    nv_read_cursor += sizeof(uint32_t);
    Memcpy(data, nv_read_cursor, result_length);
  }

  return result;
}

uint32_t TlclPCRRead(uint32_t index, void* data, uint32_t length) {
  struct s_tpm_pcr_read_cmd cmd;
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint32_t result;

  VBDEBUG(("TPM: TlclPCRRead(0x%x, %d)\n", index, length));
  if (length < kPcrDigestLength) {
    return TPM_E_IOERROR;
  }
  Memcpy(&cmd, &tpm_pcr_read_cmd, sizeof(cmd));
  ToTpmUint32(cmd.buffer + tpm_pcr_read_cmd.pcrNum, index);

  result = TlclSendReceive(cmd.buffer, response, sizeof(response));
  if (result == TPM_SUCCESS) {
    uint8_t* pcr_read_cursor = response + kTpmResponseHeaderLength;
    Memcpy(data, pcr_read_cursor, kPcrDigestLength);
  }

  return result;
}

uint32_t TlclWriteLock(uint32_t index) {
  VBDEBUG(("TPM: Write lock 0x%x\n", index));
  return TlclWrite(index, NULL, 0);
}

uint32_t TlclReadLock(uint32_t index) {
  VBDEBUG(("TPM: Read lock 0x%x\n", index));
  return TlclRead(index, NULL, 0);
}

uint32_t TlclAssertPhysicalPresence(void) {
  VBDEBUG(("TPM: Asserting physical presence\n"));
  return Send(tpm_ppassert_cmd.buffer);
}

uint32_t TlclPhysicalPresenceCMDEnable(void) {
  VBDEBUG(("TPM: Enable the physical presence command\n"));
  return Send(tpm_ppenable_cmd.buffer);
}

uint32_t TlclFinalizePhysicalPresence(void) {
  VBDEBUG(("TPM: Enable PP cmd, disable HW pp, and set lifetime lock\n"));
  return Send(tpm_finalizepp_cmd.buffer);
}

uint32_t TlclAssertPhysicalPresenceResult(void) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  return TlclSendReceive(tpm_ppassert_cmd.buffer, response, sizeof(response));
}

uint32_t TlclLockPhysicalPresence(void) {
  VBDEBUG(("TPM: Lock physical presence\n"));
  return Send(tpm_pplock_cmd.buffer);
}

uint32_t TlclSetNvLocked(void) {
  VBDEBUG(("TPM: Set NV locked\n"));
  return TlclDefineSpace(TPM_NV_INDEX_LOCK, 0, 0);
}

int TlclIsOwned(void) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE + TPM_PUBEK_SIZE];
  uint32_t result;
  result = TlclSendReceive(tpm_readpubek_cmd.buffer,
                           response, sizeof(response));
  return (result != TPM_SUCCESS);
}

uint32_t TlclForceClear(void) {
  VBDEBUG(("TPM: Force clear\n"));
  return Send(tpm_forceclear_cmd.buffer);
}

uint32_t TlclSetEnable(void) {
  VBDEBUG(("TPM: Enabling TPM\n"));
  return Send(tpm_physicalenable_cmd.buffer);
}

uint32_t TlclClearEnable(void) {
  VBDEBUG(("TPM: Disabling TPM\n"));
  return Send(tpm_physicaldisable_cmd.buffer);
}

uint32_t TlclSetDeactivated(uint8_t flag) {
  struct s_tpm_physicalsetdeactivated_cmd cmd;
  VBDEBUG(("TPM: SetDeactivated(%d)\n", flag));
  Memcpy(&cmd, &tpm_physicalsetdeactivated_cmd, sizeof(cmd));
  *(cmd.buffer + cmd.deactivated) = flag;
  return Send(cmd.buffer);
}

uint32_t TlclGetPermanentFlags(TPM_PERMANENT_FLAGS* pflags) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint32_t size;
  uint32_t result =
    TlclSendReceive(tpm_getflags_cmd.buffer, response, sizeof(response));
  if (result != TPM_SUCCESS)
    return result;
  FromTpmUint32(response + kTpmResponseHeaderLength, &size);
  /* TODO(crbug.com/379255): This fails. Find out why.
   * VbAssert(size == sizeof(TPM_PERMANENT_FLAGS));
   */
  Memcpy(pflags,
         response + kTpmResponseHeaderLength + sizeof(size),
         sizeof(TPM_PERMANENT_FLAGS));
  return result;
}

uint32_t TlclGetSTClearFlags(TPM_STCLEAR_FLAGS* vflags) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint32_t size;
  uint32_t result =
    TlclSendReceive(tpm_getstclearflags_cmd.buffer, response, sizeof(response));
  if (result != TPM_SUCCESS)
    return result;
  FromTpmUint32(response + kTpmResponseHeaderLength, &size);
  /* Ugly assertion, but the struct is padded up by one byte. */
  /* TODO(crbug.com/379255): This fails. Find out why.
   * VbAssert(size == 7 && sizeof(TPM_STCLEAR_FLAGS) - 1 == 7);
   */
  Memcpy(vflags,
         response + kTpmResponseHeaderLength + sizeof(size),
         sizeof(TPM_STCLEAR_FLAGS));
  return result;
}

uint32_t TlclGetFlags(uint8_t* disable,
                      uint8_t* deactivated,
                      uint8_t *nvlocked) {
  TPM_PERMANENT_FLAGS pflags;
  uint32_t result = TlclGetPermanentFlags(&pflags);
  if (result == TPM_SUCCESS) {
    if (disable)
      *disable = pflags.disable;
    if (deactivated)
      *deactivated = pflags.deactivated;
    if (nvlocked)
      *nvlocked = pflags.nvLocked;
    VBDEBUG(("TPM: Got flags disable=%d, deactivated=%d, nvlocked=%d\n",
             pflags.disable, pflags.deactivated, pflags.nvLocked));
  }
  return result;
}

uint32_t TlclSetGlobalLock(void) {
  uint32_t x;
  VBDEBUG(("TPM: Set global lock\n"));
  return TlclWrite(TPM_NV_INDEX0, (uint8_t*) &x, 0);
}

uint32_t TlclExtend(int pcr_num, const uint8_t* in_digest,
                    uint8_t* out_digest) {
  struct s_tpm_extend_cmd cmd;
  uint8_t response[kTpmResponseHeaderLength + kPcrDigestLength];
  uint32_t result;

  Memcpy(&cmd, &tpm_extend_cmd, sizeof(cmd));
  ToTpmUint32(cmd.buffer + tpm_extend_cmd.pcrNum, pcr_num);
  Memcpy(cmd.buffer + cmd.inDigest, in_digest, kPcrDigestLength);

  result = TlclSendReceive(cmd.buffer, response, sizeof(response));
  if (result != TPM_SUCCESS)
    return result;

  Memcpy(out_digest, response + kTpmResponseHeaderLength, kPcrDigestLength);
  return result;
}

uint32_t TlclGetPermissions(uint32_t index, uint32_t* permissions) {
  struct s_tpm_getpermissions_cmd cmd;
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint8_t* nvdata;
  uint32_t result;
  uint32_t size;

  Memcpy(&cmd, &tpm_getpermissions_cmd, sizeof(cmd));
  ToTpmUint32(cmd.buffer + tpm_getpermissions_cmd.index, index);
  result = TlclSendReceive(cmd.buffer, response, sizeof(response));
  if (result != TPM_SUCCESS)
    return result;

  nvdata = response + kTpmResponseHeaderLength + sizeof(size);
  FromTpmUint32(nvdata + kNvDataPublicPermissionsOffset, permissions);
  return result;
}

uint32_t TlclGetOwnership(uint8_t* owned) {
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint32_t size;
  uint32_t result =
    TlclSendReceive(tpm_getownership_cmd.buffer, response, sizeof(response));
  if (result != TPM_SUCCESS)
    return result;
  FromTpmUint32(response + kTpmResponseHeaderLength, &size);
  /* TODO(crbug.com/379255): This fails. Find out why.
   * VbAssert(size == sizeof(*owned));
   */
  Memcpy(owned,
         response + kTpmResponseHeaderLength + sizeof(size),
         sizeof(*owned));
  return result;
}

uint32_t TlclGetRandom(uint8_t* data, uint32_t length, uint32_t *size) {
  struct s_tpm_get_random_cmd cmd;
  uint8_t response[TPM_LARGE_ENOUGH_COMMAND_SIZE];
  uint32_t result;

  VBDEBUG(("TPM: TlclGetRandom(%d)\n", length));
  Memcpy(&cmd, &tpm_get_random_cmd, sizeof(cmd));
  ToTpmUint32(cmd.buffer + tpm_get_random_cmd.bytesRequested, length);
  /* There must be room in the response buffer for the bytes. */
  if (length > TPM_LARGE_ENOUGH_COMMAND_SIZE - kTpmResponseHeaderLength
               - sizeof(uint32_t)) {
    return TPM_E_IOERROR;
  }

  result = TlclSendReceive(cmd.buffer, response, sizeof(response));
  if (result == TPM_SUCCESS) {
    uint8_t* get_random_cursor;
    FromTpmUint32(response + kTpmResponseHeaderLength, size);

    /* There must be room in the target buffer for the bytes. */
    if (*size > length) {
      return TPM_E_RESPONSE_TOO_LARGE;
    }
    get_random_cursor = response + kTpmResponseHeaderLength
                                 + sizeof(uint32_t);
    Memcpy(data, get_random_cursor, *size);
  }

  return result;
}
