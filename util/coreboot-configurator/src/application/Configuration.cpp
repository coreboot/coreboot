/* SPDX-License-Identifier: GPL-2.0-only */

#include <QFile>
#include <QString>
#include <QTextStream>

#include "Configuration.h"
#include "Util.h"

QMap<QString, QString> Configuration::fromFile(const QString &curr_path)
{
	QFile curr_file(curr_path);

	if ( !curr_file.open(QFile::ReadOnly)
		|| !curr_file.isReadable()
		|| curr_file.atEnd()) {
		return {};
	}

	auto result = Util::parseParameters(curr_file);

	curr_file.close();
	return result;
}


bool Configuration::toFile(const QString &curr_path, const Parameters &params)
{
	QFile output(curr_path);

	if(!output.open(QFile::WriteOnly|QFile::Truncate)){
		return false;
	}
	QTextStream outStream(&output);
	for(auto it = params.begin(); it != params.end(); ++it){
		outStream << it.key() << " = " << it.value() << "\n";
	}

	output.close();
	return true;
}
