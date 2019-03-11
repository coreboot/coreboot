# PEIM to PEIM Interface (PPI)

This section is intended to document the purpose of creating PPI service
inside coreboot space to perform some specific operation related to CPU,
chipset using Intel FSP. This feature is added into FSP specification 2.1
where FSP should be able to locate PPI, published by boot firmware and
able to execute the same in FSP's context.

* [What is PPI](https://www.intel.com/content/dam/www/public/us/en/documents/reference-guides/efi-pei-cis-v09.pdf)

## List of PPI service

### Publish MP Service PPI from boot firmware (coreboot) to initialize CPU
- [MP Service PPI](mp_service_ppi.md)
