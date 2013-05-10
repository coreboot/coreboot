#!/bin/sh

BL1_NAME="E5250.nbl1.bin"
BL1_PATH="3rdparty/cpu/samsung/exynos5250/"
BL1_URL="http://commondatastorage.googleapis.com/chromeos-localmirror/distfiles/exynos-pre-boot-0.0.2-r8.tbz2"

get_bl1() {
  mkdir -p "${BL1_PATH}"
  cd "${BL1_PATH}"
  wget "${BL1_URL}" -O bl1.tbz2
  tar jxvf bl1.tbz2
  mv "exynos-pre-boot/firmware/${BL1_NAME}" .
  rm -rf exynos-pre-boot
  if [ ! -e "${BL1_NAME}" ]; then
    echo "Error getting BL1"
  fi
}

main() {
  if [ ! -e ${BL1_PATH}/${BL1_NAME} ]; then
    get_bl1
  fi
}

set -e
main "$@"
