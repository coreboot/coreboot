#!/usr/bin/env bash

if [ "$1" == "livehtml" ]; then
  echo "Starting live documentation build"
  cd /data-in/Documentation && make livesphinx BUILDDIR=/tmp/build
else
  echo "Starting production documentation build"
  cd /data-in/Documentation \
    && make sphinx BUILDDIR=/tmp/build \
    && rm -rf /data-out/* \
    && mv /tmp/build/html/* /data-out/
fi
