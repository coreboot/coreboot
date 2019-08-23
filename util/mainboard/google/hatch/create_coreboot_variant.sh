#!/bin/bash
#
# This file is part of the coreboot project.
#
# Copyright 2019 Google LLC.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

if [[ "$#" -ne 1 ]]; then
  echo "Usage: $0 variant_name"
  echo "e.g. $0 kohaku"
  echo "Adds a new variant of Hatch to Kconfig and Kconfig.name, creates the"
  echo "skeleton files for acpi, ec, and gpio, copies the makefile for"
  echo "SPD sources, and sets up a basic overridetree"
  exit 1
fi

# Note that this script is specific to Hatch, and so it does not allow
# you to specify the baseboard as one of the cmdline arguments.
#
# This is the name of the base board that we're cloning to make the variant.
base="hatch"
# This is the name of the variant that is being cloned
# ${var,,} converts to all lowercase
variant="${1,,}"

# This script and the templates live in util/mainboard/google/hatch
# We need to create files in src/mainboard/google/hatch
pushd "${BASH_SOURCE%/*}" || exit
SRC=$(pwd)
popd || exit
pushd "${SRC}/../../../../src/mainboard/google/${base}" || {
  echo "The baseboard directory for ${base} does not exist.";
  exit; }

# Make sure the variant doesn't already exist
if [[ -e variants/${variant} ]]; then
  echo "variants/${variant} already exists."
  echo "Have you already created this variant?"
  popd || exit
  exit 2
fi

# Start a branch. Use YMD timestamp to avoid collisions.
DATE=$(date +%Y%m%d)
git checkout -b "create_${variant}_${DATE}"

# Copy the template tree to the target
cp -r "${SRC}/template/*" "variants/${variant}/"
git add "variants/${variant}/"

# Now add the new variant to Kconfig and Kconfig.name
# These files are in the current directory, e.g. src/mainboard/google/hatch
"${SRC}/kconfig.py" --name "${variant}"

mv Kconfig.new Kconfig
mv Kconfig.name.new Kconfig.name

git add Kconfig Kconfig.name

# Now commit the files
git commit -sm "${base}: Create ${variant} variant

BUG=none
TEST=util/abuild/abuild -p none -t google/${base} -x -a
make sure the build includes GOOGLE_${variant^^}"

popd || exit

echo "Please check all the files (git show), make any changes you want,"
echo "and then push to coreboot HEAD:refs/for/master"
