/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SMMSTORETOOL__UDK2017_H__
#define SMMSTORETOOL__UDK2017_H__

#include <UDK2017/MdePkg/Include/Uefi/UefiBaseType.h>
#include <UDK2017/MdePkg/Include/Uefi/UefiMultiPhase.h>
#include <UDK2017/MdePkg/Include/Pi/PiFirmwareVolume.h>
#include <UDK2017/MdeModulePkg/Include/Guid/VariableFormat.h>

/*
 * ProcessorBind.h contains `#pragma GCC visibility push(hidden)` guarded by an
 * identical condition, but there is no corresponding `pop` pragma. This can
 * cause trouble for code following headers above including libc headers because
 * linker suddenly considers declarations from them (e.g., `strcmp()`) to be
 * hidden.
 *
 * In order to address this situation all UDK2017 headers used by this tool
 * must be listed above and included indirectly through this header which
 * restores default visibility.
 *
 * Mind that this issue appears only if the following conditions are satisfied
 * and not all toolchains are configured to build position-independent code by
 * default (as if -fpic or -fpie appears on compilation command-line).
 */
#if defined(__GNUC__) && defined(__pic__) && !defined(USING_LTO)
#pragma GCC visibility pop
#endif

#endif // SMMSTORETOOL__UDK2017_H__
