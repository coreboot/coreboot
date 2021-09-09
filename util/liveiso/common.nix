# SPDX-License-Identifier: GPL-2.0-only

{ config, options, pkgs, ... }:

{
	imports = [
		<nixpkgs/nixos/modules/installer/cd-dvd/channel.nix>
		<nixpkgs/nixos/modules/installer/cd-dvd/iso-image.nix>
	];

	system.stateVersion = "21.05";

	isoImage = {
		makeEfiBootable = true;
		makeUsbBootable = true;
		isoName = "${config.isoImage.isoBaseName}-${config.system.nixos.label}-${pkgs.stdenv.hostPlatform.system}.iso";
	};

	# Tell the Nix evaluator to garbage collect more aggressively.
	# This is desirable in memory-constrained environments that don't
	# (yet) have swap set up.
	environment.variables.GC_INITIAL_HEAP_SIZE = "1M";

	boot = {
		kernelParams = [
			"console=ttyS0,115200"
			"console=tty0"
			"iomem=relaxed"
		];
		# pkgs.linuxPackages == lts
		# pkgs.linuxPackages_latest == stable
		kernelPackages = pkgs.linuxPackages;
		extraModulePackages = with config.boot.kernelPackages; [
			acpi_call
			chipsec
			zfs
		];
		# Make programs more likely to work in low memory
		# environments. The kernel's overcommit heustistics bite us
		# fairly often, preventing processes from forking even if
		# there is plenty of free memory.
		kernel.sysctl."vm.overcommit_memory" = "1";
		loader.grub.memtest86.enable = true;
		postBootCommands = ''
			mkdir -p /mnt
		'';
	};

	console.packages = options.console.packages.default ++ [ pkgs.terminus_font ];

	nixpkgs.config.allowUnfree = true;
	hardware = {
		cpu.intel.updateMicrocode = true;
		cpu.amd.updateMicrocode = true;
		enableAllFirmware = true;
		bluetooth = {
			enable = true;
			powerOnBoot = false;
			package = pkgs.bluezFull;
		};
	};

	services = {
		fwupd.enable = true;
		udev.packages = with pkgs; [
			rfkill_udev
		];
		openssh = {
			enable = true;
			permitRootLogin = "yes";
		};
	};

	networking = {
		hostName = "devsystem";
		networkmanager.enable = true;
	};

	security.sudo.wheelNeedsPassword = false;

	users = {
		groups.user = {};
		users = {
			root.initialHashedPassword = "";
			user = {
				isNormalUser = true;
				group = "user";
				extraGroups = [ "users" "wheel" "networkmanager" "uucp" ];
				initialHashedPassword = "";
			};
		};
	};

	environment.systemPackages = with pkgs; [
		acpica-tools
		btrfs-progs
		bzip2
		ccrypt
		chipsec
		coreboot-utils
		cryptsetup
		curl
		ddrescue
		dmidecode
		dosfstools
		e2fsprogs
		efibootmgr
		efivar
		exfat
		f2fs-tools
		flashrom
		fuse
		fuse3
		fwts
		gptfdisk
		gitAndTools.gitFull
		gitAndTools.tig
		gzip
		hdparm
		hexdump
		htop
		i2c-tools
		iasl
		intel-gpu-tools
		inxi
		iotools
		jfsutils
		jq
		mdadm
		minicom
		mkpasswd
		ms-sys
		msr-tools
		neovim
		nixos-install-tools
		ntfsprogs
		openssl
		p7zip
		pacman
		parted
		pciutils
		phoronix-test-suite
		powertop
		psmisc
		python3Full
		rsync
		screen
		sdparm
		smartmontools
		socat
		sshfs-fuse
		testdisk
		tmate
		tmux
		unzip
		upterm
		usbutils
		wget
		xfsprogs.bin
		zfs
		zip
		zstd
	];
}
