# Intel Authenticated Code Modules

The Authenticated Code Modules (ACMs) are Intel digitally signed modules
that contain code to be run before the traditional x86 CPU reset vector.
The ACMs can be invoked at runtime through the GETSEC instruction, too.

A platform that wants to use Intel TXT must use two ACMs:
1. BIOS ACM
   * The BIOS ACM must be present in the boot flash.
   * The BIOS ACM must be referenced by the [FIT].
2. SINIT ACM
   * The SINIT ACM isn't referenced by the [FIT].
   * The SINIT ACM should be provided by the boot firmware, but bootloaders
     like [TBOOT] are able to load them from the filesystem as well.

## Retrieving ACMs

The ACMs can be downloaded on Intel's website:
[Intel Trusted Execution Technology](https://software.intel.com/en-us/articles/intel-trusted-execution-technology)

If you want to extract the BLOB from vendor firmware you can search for the
string ``LCP_POLICY_DATA`` or ``TXT``.

## Header

Every ACM has a fixed size header:

```c
/*
 * ACM Header v0.0 without dynamic part
 * Chapter A.1
 * Intel TXT Software Development Guide (Document: 315168-015)
 */
struct acm_header_v0 {
	uint16_t module_type;
	uint16_t module_sub_type;
	uint32_t header_len;
	uint16_t header_version[2];
	uint16_t chipset_id;
	uint16_t flags;
	uint32_t module_vendor;
	uint32_t date;
	uint32_t size;
	uint16_t txt_svn;
	uint16_t se_svn;
	uint32_t code_control;
	uint32_t error_entry_point;
	uint32_t gdt_limit;
	uint32_t gdt_ptr;
	uint32_t seg_sel;
	uint32_t entry_point;
	uint8_t reserved2[63];
} __packed;
```

[FIT]: ../../soc/intel/fit.md
[TBOOT]: https://sourceforge.net/p/tboot/wiki/Home/
