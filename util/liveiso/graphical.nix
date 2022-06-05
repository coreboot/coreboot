# SPDX-License-Identifier: GPL-2.0-only

{ pkgs, lib, ... }:

{
	imports = [
		./common.nix
	];

	hardware.pulseaudio = {
		enable = true;
		package = pkgs.pulseaudioFull;
	};

	security.polkit = {
		enable = true;
		extraConfig = ''
			polkit.addRule(function(action, subject) {
				if (subject.isInGroup("wheel")) {
					return polkit.Result.YES;
				}
			});
		'';
	};

	services.xserver = {
		enable = true;
		displayManager = {
			gdm = {
				enable = true;
				autoSuspend = false;
			};
			autoLogin = {
				enable = true;
				user = "user";
			};
		};
		desktopManager.gnome.enable = true;
	};

	users.users.user.extraGroups = [ "audio" "video" "input" ];

	environment.systemPackages = with pkgs; [
		firefox
		gparted
		pavucontrol
	];

	# Remove unnecessary stuff
	services = {
		gnome = {
			evolution-data-server.enable = lib.mkForce false;
			gnome-online-accounts.enable = lib.mkForce false;
			gnome-online-miners.enable = lib.mkForce false;
			gnome-initial-setup.enable = lib.mkForce false;
			chrome-gnome-shell.enable = lib.mkForce false;
		};
		telepathy.enable = lib.mkForce false;
		dleyna-renderer.enable = lib.mkForce false;
		dleyna-server.enable = lib.mkForce false;
	};

	programs.geary.enable = lib.mkForce false;

	environment.gnome.excludePackages = with pkgs; [
		gnome.gnome-weather
		gnome.epiphany
		gnome.gnome-contacts
		gnome-photos
		gnome.gnome-music
		yelp
	];
}
