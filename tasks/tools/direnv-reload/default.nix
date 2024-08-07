{ ... }:

# The BUILD Task

{
	perSystem = { pkgs, ... }: {
		mission-control.scripts = {
			"r" = {
				description = "Reload the development environment";
				category = "tools";

				exec = pkgs.writeShellApplication {
					name = "direnv-reload-build";

					runtimeInputs = [
						pkgs.direnv
					];

					# FIXME(Krey): This should use flake-root to set absolute path
					text = builtins.readFile ./tasks-direnv-reload.sh;
				};
			};
		};
	};
}
