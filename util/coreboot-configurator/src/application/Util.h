/* SPDX-License-Identifier: GPL-2.0-only */

#pragma once

#include <QIODevice>
#include <QMap>
#include <QString>

namespace Util {
	inline QMap<QString,QString> parseParameters(QIODevice& dev){
		QString curr_line;
		QMap<QString, QString> result;

		while (!dev.atEnd()) {
			curr_line = dev.readLine().trimmed();

			auto split = curr_line.split('=');
			if(split.size()!=2){
				continue;
			}

			result.insert(split[0].trimmed(), split[1].trimmed());
		}
		return result;
	}
}
