#!/usr/bin/env sh

SCRIPTS_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR="${SCRIPTS_DIR}/../.."

VERSION=$(cat ${PROJECT_DIR}/version.txt)
LAST_COMMIT=$(git log --oneline -n 1 --pretty=format:%h ${PROJECT_DIR})
DIFF=$(git diff ${PROJECT_DIR})

if [ -z "$DIFF" ]; then
    echo "${VERSION}-${LAST_COMMIT}"
else
    echo "${VERSION}-${LAST_COMMIT}-dirty"
fi
