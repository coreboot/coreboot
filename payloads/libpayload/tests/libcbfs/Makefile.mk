# SPDX-License-Identifier: GPL-2.0-only

tests-y += cbfs-lookup-no-fallback-test
tests-y += cbfs-lookup-has-fallback-test
tests-y += cbfs-verification-no-sha512-test
tests-y += cbfs-verification-has-sha512-test
tests-y += cbfs-no-verification-no-sha512-test
tests-y += cbfs-no-verification-has-sha512-test


cbfs-lookup-no-fallback-test-srcs += tests/libcbfs/cbfs-lookup-test.c
cbfs-lookup-no-fallback-test-srcs += tests/mocks/cbfs_file_mock.c
cbfs-lookup-no-fallback-test-config += CONFIG_LP_ENABLE_CBFS_FALLBACK=0
cbfs-lookup-no-fallback-test-config += CONFIG_LP_LZ4=1
cbfs-lookup-no-fallback-test-config += CONFIG_LP_LZMA=1

$(call copy-test,cbfs-lookup-no-fallback-test,cbfs-lookup-has-fallback-test)
cbfs-lookup-has-fallback-test-config += CONFIG_LP_ENABLE_CBFS_FALLBACK=1

cbfs-verification-no-sha512-test-srcs += tests/libcbfs/cbfs-verification-test.c
cbfs-verification-no-sha512-test-srcs += tests/mocks/cbfs_file_mock.c
cbfs-verification-no-sha512-test-config += CONFIG_LP_CBFS_VERIFICATION=1
cbfs-verification-no-sha512-test-config += VB2_SUPPORT_SHA512=0

$(call copy-test,cbfs-verification-no-sha512-test,cbfs-verification-has-sha512-test)
cbfs-verification-has-sha512-test-config += VB2_SUPPORT_SHA512=1

$(call copy-test,cbfs-verification-no-sha512-test,cbfs-no-verification-no-sha512-test)
cbfs-verification-has-sha512-test-config += CONFIG_LP_CBFS_VERIFICATION=0

$(call copy-test,cbfs-verification-no-sha512-test,cbfs-no-verification-has-sha512-test)
cbfs-verification-has-sha512-test-config += CONFIG_LP_CBFS_VERIFICATION=0
cbfs-verification-has-sha512-test-config += VB2_SUPPORT_SHA512=1
