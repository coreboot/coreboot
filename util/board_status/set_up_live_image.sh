#!/usr/bin/env bash

# This script is used to set up a ubuntu-based live image to be used
# with coreboot's board_status script.  It modifies the system so that
# board_status can connect over SSH and run cbmem.  This script is NOT
# meant to be run on an installed system, and changes the configuration
# in ways that would be dangerous to security on an installed system.

# Make sure we're on a ubuntu live image
if [ ! -e /usr/bin/ubiquity ]; then
	echo "Error: This doesn't appear to be a ubuntu based live image.  Exiting."
	exit 1
fi

RED='\033[1;31m'
GREEN='\033[1;32m'
NC='\033[0m' # No Color

# shellcheck disable=SC2059
error ()
{
	printf "${RED}ERROR: $1 ${NC}\n" >&2
	if [ -n "$2" ]; then printf "${RED}Removing $2 ${NC}\n"; rm -rf "./$2"; fi
	exit 1
}

# shellcheck disable=SC2059
status ()
{
	printf "${GREEN}${1}${NC}"
}

# Install and configure the ssh server for the board-status script to connect to.
status "Installing and configuring ssh server\n"
sudo rm -f /etc/apt/sources.list
sudo apt-get update || \
	error "Could not update packages"
sudo apt-get install -y openssh-server || \
	error "Could not install openssh-server"
sudo sed -i.bak 's/PermitRootLogin.*/PermitRootLogin yes/' /etc/ssh/sshd_config || \
	error "Could not update sshd.config to allow root login."
sudo sudo service ssh restart || \
	error "Could not restart ssh server"

# Set the root password so it can be used to log in
status "Setting root password to 'coreboot'\n"
echo -e "root:coreboot" | sudo chpasswd || \
	error "Could not reset root password"

# Download the coreboot tree
status "Installing git and clone the coreboot repo\n"
sudo apt-get install -y git build-essential || \
	error "Could not install git"
git clone --depth 1 https://review.coreboot.org/coreboot || \
	error "Could not download coreboot repository"

# Build and install cbmem
status "Building and installing cbmem\n"
make -C coreboot/util/cbmem || \
	error "Could not build cbmem"
sudo make -C coreboot/util/cbmem install || \
	error "Could not instll cbmem"
sudo cbmem || \
	error "cbmem did not run correctly"
rm -rf coreboot

# Identify the ip address that board-status will connect to
IP_ADDR=$(ifconfig -a | grep 'inet addr' | grep -v '127.0.0.1' | \
	sed 's|.*inet addr:||' | sed 's|Bcast.*||')
status "\nAddress or addresses for this board:  $IP_ADDR\n"
