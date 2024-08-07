{ ... }:

{
	perSystem = { inputs', pkgs, lib, ... }: {
		mission-control.scripts = {
			hello = {
				description = "Hello";
				category = "Tests";
				exec = "echo hello";
			};
		};
	};
}
