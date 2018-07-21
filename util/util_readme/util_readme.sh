#!/bin/bash

# Creates `./util/README.md` and `./Documentation/Util.md` of description files
# in `./util` subdirectories
#
# Execute from root of project.  Example:
# `sh util/util_readme/util_readme.sh`

UTIL_README_DIR="$(cd "$(dirname "$0")" || exit; pwd -P)"
UTIL_DIR=$(dirname "$UTIL_README_DIR")
COREBOOT_ROOT_DIR=$(dirname "$UTIL_DIR")
DOCMENTATION_DIR="$COREBOOT_ROOT_DIR/Documentation"

DESCRIPTION_FILES=$(find "$UTIL_DIR" -name "description.md" | sort)

echo -n "" > "$UTIL_DIR/README.md"
echo "# Utilities" > "$DOCMENTATION_DIR/util.md"

for DESC_FILE in $DESCRIPTION_FILES; do
  UTIL_NAME=$(echo "$DESC_FILE" | rev | cut -d '/' -f2 | rev)
  DESC=$(cat "$DESC_FILE")

  if [[ $DESC == "__"${UTIL_NAME}* || $DESC == "__["${UTIL_NAME}* ]]; then
    DESC="* $DESC"
  else
    DESC="* __${UTIL_NAME}__ - $DESC"
  fi

  # format description to under 72 characters per line and only
  # breaking on whitespace
  DESC=$(echo "$DESC" \
      | tr '\r\n' ' '  \
      | sed 's/  [*]\+/\n\t\*/g' \
      | sed 's/  \+/ /g' \
      | fold -s -w72 \
      | sed 's/\s*$//')

  echo "$DESC" >> "$UTIL_DIR/README.md"
  echo "$DESC" >> "$DOCMENTATION_DIR/util.md"
done
