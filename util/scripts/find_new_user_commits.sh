#!/usr/bin/env bash
# shellcheck disable=SC2310,SC2312
# The above line must be directly after the shebang line.
# Disables these warnings:
# SC2310 - This function is invoked in an 'if' condition so set -e will be disabled.
#          Invoke separately if failures should cause the script to exit.
# SC2312 - Consider invoking this command separately to avoid masking its return value

#
# SPDX-License-Identifier: GPL-2.0-only
#

#
# Description:
# Identifies new users in gerrit so that they can be greeted and marked
# as new users.
#

VERSION="1.00"
PROGRAM=$0
PROGNAME="$(basename "${PROGRAM}")"

COMMIT_COUNT=5 # Consider a user to be new until they have this many commits
AGE="1week"
#AGE="3months"
KNOWN_AUTHOR_FILE="./authorlist.txt"
NEW_AUTHOR_FILE="./new_authors.txt"
GERRIT_USER="$1"
GERRIT_REPO="review.coreboot.org"

show_version() {
  echo "${PROGNAME} version ${VERSION}"
  echo
}

usage() {
  echo "Usage: ${PROGNAME} <gerrit username>"
  echo "example: ${PROGNAME} martinlroth"
}

main() {
  local commit_count
  local patchlist
  local owner
  local owner_email
  local commit_id
  local new_users=0

  show_version

  if ! jq --version >/dev/null 2>&1; then
    echo "Error: jq is not installed. Please install it with your package manager."
    exit 1
  fi

  if [[ -z ${GERRIT_USER} ]]; then
    echo "Error: Please specify gerrit username on the command line." >&2
  fi
  if [[ -z ${GERRIT_USER} || ${GERRIT_USER} == "--help" || ${GERRIT_USER} == "-h" ]]; then
    usage
    exit 1
  fi

  echo "List of known users with more than 5 commits: ${KNOWN_AUTHOR_FILE}"
  echo "List of new user's patches: ${NEW_AUTHOR_FILE}"
  echo "Key: . = author in known user list or new user commit already seen"
  echo "     : = author getting added to known user list"
  echo

  touch "${NEW_AUTHOR_FILE}" "${KNOWN_AUTHOR_FILE}"

  # Get all coreboot patches that aren't by known users, newer than the age set above
  patchlist="$(ssh -p 29418 "${GERRIT_USER}@${GERRIT_REPO}" gerrit query --format=JSON --no-limit --current-patch-set "repo:coreboot AND status:open NOT age:${AGE} NOT ownerin:Reviewers NOT ownerin:\\\"Core Developers\\\"" |
    jq -Mr '.currentPatchSet.uploader.name + ", " + .currentPatchSet.uploader.email + ", " + .currentPatchSet.revision + ", " + (.number | tostring)' |
    grep -v "null\|^,");"

  if [[ -z ${patchlist} ]]; then
    echo "Error: No patches returned." >&2
    exit 1
  else
    echo "$(wc -l <<<"${patchlist}") patches found."
  fi

  # Loop through all patches found, looking for any that are not by a known author.
  # If an author with more than 5 patches is found, add them to a list to filter out in the future.
  # If we find a new patch, print it, then add it to a list so that it isn't reported again.
  IFS=$'\n'
  for patch in ${patchlist}; do
    owner="$(echo "${patch}" | cut -f1 -d',')"
    owner_email="$(echo "${patch}" | cut -f2 -d',')"
    commit_id="$(echo "${patch}" | cut -f4 -d',')"

    if grep -qi "${owner}" "${KNOWN_AUTHOR_FILE}" || grep -qi "${owner_email}" "${KNOWN_AUTHOR_FILE}" || grep -q "${commit_id}" "${NEW_AUTHOR_FILE}"; then
      printf "."
      continue
    fi

    # Get the author's commit count
    # shellcheck disable=SC2126 # (Consider using grep -c instead of grep | wc)
    commit_count="$(ssh -p 29418 "${GERRIT_USER}@${GERRIT_REPO}" gerrit query --format=JSON "limit:6 AND repo:coreboot AND owner:${owner_email} and status:merged" |
      jq -Mr '.owner.name + ", " + .owner.email' |
      grep -v "null\|^," |
      wc -l)"
    if [[ ${commit_count} -ge ${COMMIT_COUNT} ]]; then
      printf ":"
      echo "${owner}, ${owner_email}" >>"${KNOWN_AUTHOR_FILE}"
      continue
    fi

    printf "\n%s looks to be a patch by a new author.\n" "${patch}"
    echo "${patch}" >>"${NEW_AUTHOR_FILE}"
    new_users+=1
  done
  printf "\n"
  if [[ ${new_users} -eq 0 ]]; then
    echo "No new patches by new users found."
  fi
}

main
