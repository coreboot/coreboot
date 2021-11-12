## SPDX-License-Identifier: GPL-2.0-only

# Polkit Files
polkit_dir = join_paths(get_option('datadir'), 'polkit-1', 'actions')
polkit_sources = [
	'org.coreboot.nvramtool.policy',
	'org.coreboot.reboot.policy',
]

install_data(polkit_sources,
	     install_dir: polkit_dir)

# Desktop Entry
desktop_dir = join_paths(get_option('datadir'), 'applications')
desktop_sources = [
	'coreboot-configurator.desktop',
]

install_data(desktop_sources,
	     install_dir: desktop_dir)

# Icon
inkscape = find_program('inkscape')
icon_dir = join_paths(get_option('datadir'),'icons', 'hicolor')
foreach size: get_option('sizes')
	target_temp_name = '@0@'.format(size)
	dpi=size.to_int() * 2
	png = configure_file(
		input: 'coreboot_configurator.svg',
		output: target_temp_name + '.png',
		command: [
			inkscape,
			'--export-height=@0@'.format(size),
			'--export-width=@0@'.format(size),
			'--export-png=@OUTPUT@',
			'@INPUT@',
		]
	)

	install_data(png,
		     rename: meson.project_name() + '.png',
		     install_dir: join_paths(icon_dir, '@0@x@1@'.format(size, size), 'apps'))
endforeach
