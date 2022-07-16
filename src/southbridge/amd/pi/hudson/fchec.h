/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_HUDSON_FCHEC__
#define __AMD_HUDSON_FCHEC__

#include "imc.h"
#include <Porting.h>
#include <AGESA.h>
#include "FchCommonCfg.h"

extern VOID FchECfancontrolservice(IN VOID *FchDataPtr);
void agesawrapper_fchecfancontrolservice(void);

#endif
