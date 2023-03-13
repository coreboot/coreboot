# SPDX-License-Identifier: GPL-2.0-only

{
	imports = [
		./common.nix
	];

	services.getty.autologinUser = "user";
}
