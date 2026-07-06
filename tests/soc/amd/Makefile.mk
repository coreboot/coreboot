# SPDX-License-Identifier: GPL-2.0-only

tests-y += psp_fw-test

psp_fw-test-srcs += src/commonlib/region.c
psp_fw-test-srcs += tests/soc/amd/psp_fw-test.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_parser.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_util.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_bhd.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_cmb.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_efs.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_ish.c
psp_fw-test-srcs += src/vendorcode/amd/psp/psp_fw_psp.c

psp_fw-test-syssrcs += tests/helpers/file.c
