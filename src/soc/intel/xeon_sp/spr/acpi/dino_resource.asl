/* SPDX-License-Identifier: GPL-2.0-or-later */

#undef PRTID
#define PRTID      AR12

#undef DEVPREFIX
#define DEVPREFIX  DI
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  PM
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  HQ
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  PN
#include "pci_resource.asl"

#undef DEVPREFIX
#define DEVPREFIX  HR
#include "pci_resource.asl"
