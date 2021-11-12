/* SPDX-License-Identifier: GPL-2.0-only */

#include <QApplication>
#include <QTranslator>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	if (translator.load(QLocale(), QLatin1String("corebootconfigurator"), QLatin1String("_"), QLatin1String(":/lang/i18n"))){
		a.installTranslator(&translator);
	}

	MainWindow w;
	w.show();
	return a.exec();
}
