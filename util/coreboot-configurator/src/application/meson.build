## SPDX-License-Identifier: GPL-2.0-only

# Documentation: https://mesonbuild.com/Qt5-module.html
qt5 = import('qt5')
qt5_dep = dependency('qt5', modules : ['Core', 'Widgets', 'Svg'])
yamlcpp_dep = dependency('yaml-cpp', version: '>= 0.5.1', required: true)

# TODO: Translations
# lang_cpp = qt5.compile_translations(qresource: 'lang.qrc')

generated_files = qt5.preprocess(
	moc_headers	: ['MainWindow.h', 'AboutDialog.h', 'ToggleSwitch.h'],
	ui_files	: ['MainWindow.ui', 'AboutDialog.ui'],
	dependencies	: [qt5_dep],
	qresources	: ['resources.qrc'],
)

mock = get_option('mock')

if mock
	add_project_arguments('-DMOCK', language : 'cpp')
endif

executable('coreboot-configurator',
	   'main.cpp',
	   'MainWindow.cpp',
	   'AboutDialog.cpp',
	   'Configuration.cpp',
	   'ToggleSwitch.cpp',
	   'NvramToolCli.cpp',
#	   lang_cpp,
	   generated_files,
	   dependencies : [qt5_dep, yamlcpp_dep],
	   install : true
)
