FROM coreboot/coreboot-jenkins-node:{{SDK_VERSION}}

# Test the built image
RUN mkdir -p /tmp/work && \
  cd /tmp/work && \
  git clone https://review.coreboot.org/bios_extract.git && \
  make -C bios_extract && \
  git clone https://review.coreboot.org/memtest86plus.git && \
  make -C memtest86plus && \
  git clone https://review.coreboot.org/flashrom.git && \
  CONFIG_EVERYTHING=yes make -C flashrom && \
  git clone https://review.coreboot.org/em100.git && \
  make -C em100 && \
  git clone https://review.coreboot.org/coreboot.git && \
  (cd coreboot && git submodule update --init --checkout ) && \
  make -C coreboot CPUS=$(nproc) test-abuild

RUN \
  cd /tmp/work && \
  make -C coreboot olddefconfig && \
  make -C coreboot all -j && \
  make -C coreboot printall && \
  make -C coreboot filelist && \
  make -C coreboot ctags-project && \
  make -C coreboot cscope-project

RUN \
  cd /tmp/work && \
  make -C coreboot test-payloads && \
  make -C coreboot test-tools -j && \
  make -C coreboot test-lint -j && \
  make -C coreboot test-cleanup -j && \
  cd && \
  rm -rf /tmp/work/
