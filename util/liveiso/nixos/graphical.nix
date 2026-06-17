# SPDX-License-Identifier: GPL-2.0-only

{ pkgs, lib, ... }:

{
	imports = [
		./common.nix
	];

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

	programs.dconf = {
		enable = true;
		profiles = {
			user.databases = [{
				settings = {
					"org/gnome/settings-daemon/plugins/power" = {
						sleep-inactive-ac-type = "nothing";
					};
					"org/gnome/desktop/interface" = {
						show-battery-percentage = true;
						clock-show-weekday = true;
					};
					"org/gnome/desktop/calendar" = {
						show-weekdate = true;
					};
				};
			}];
		};
	};

	services.displayManager = {
		gdm = {
			enable = true;
			autoSuspend = false;
		};
	};
	services.desktopManager.gnome.enable = true;
	services.displayManager = {
		autoLogin = {
			enable = true;
			user = "user";
		};
	};

	services.pulseaudio.enable = false;
	services.pipewire = {
		enable = true;
		pulse.enable = true;
		alsa.enable = true;
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
			gnome-initial-setup.enable = lib.mkForce false;
			gnome-browser-connector.enable = lib.mkForce false;
		};
		telepathy.enable = lib.mkForce false;
		dleyna.enable = lib.mkForce false;
	};

	programs.geary.enable = lib.mkForce false;

	environment.gnome.excludePackages = with pkgs; [
		gnome-weather
		epiphany
		gnome-contacts
		gnome-photos
		gnome-music
		yelp
	];
}
