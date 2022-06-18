# Multiple Processor (MP) Initialization

This section is intended to document the purpose of performing multiprocessor
initialization and its possible ways in coreboot space.

Entire CPU multiprocessor initialization can be divided into two parts
1. BSP (Boot Strap Processor) Initialization
2. AP (Application Processor) Initialization

* [Multiple Processor Init](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.pdf) - section 8.4

## Problem Statement

1. coreboot is capable enough to handle multiprocessor initialization on
IA platforms.

2. With restricted CPU programming logic, there might be some cases where
certain feature programming can't be open sourced at early development of SOC.

Platform code might need to compromise on those closed source nature of CPU
programming if we don't plan to provide an alternate interface which can be
used by coreboot to get rid of such close sourced CPU programming.

## Possible Solution Space

Considering these facts, there are 3 possible solutions to perform MP
initialization from coreboot + FSP space.

1. coreboot to perform complete MP initialization by its own. This includes
BSP and AP programming of CPU features mostly non-restricted one. This is
the default configuration. Most SoCs like SKL, KBL, APL are okay to make use
of this MP initialization method.

2. Alternatively, SoC users also can skip coreboot doing MP initialization
and make use of FSP binary to perform same task. This can be achieved by using
Kconfig name USE_INTEL_FSP_MP_INIT. As of 2019 all Google Chrome products are
using coreboot native MP initialization mechanism and some IOTG platforms
are using FSP MP Init solution as well.

3. Final option is to let coreboot publish PPI (PEIM to PEIM Interface) to
perform some restricted (closed source) CPU programming. In that case,
coreboot will use its native MP init and additionally publish MP service PPI
for FSP to consume. FSP will execute some CPU programming using same PPI
service from its own context. One can use
USE_INTEL_FSP_TO_CALL_COREBOOT_PUBLISH_MP_PPI Kconfig to perform this
operation.

For latest SoCs like CNL, WHL, ICL, etc, its recommended to make use of this
option in order to perform SGX and C6DRAM enabling.

Typically all platforms supported by FSP 2.1 specification will have
external PPI service feature implemented.

## References
- [PPI](../fsp/ppi/ppi.md)
- [MP Service PPI](../fsp/ppi/mp_service_ppi.md)
