/*

Copyright © 2009-2010 Quentin RICHERT

Multiuso is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Multiuso is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Multiuso.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtCore>
#include "../Multiuso/Multiuso.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	QString locale = QLocale::system().name();

	QTranslator translator;
		translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));

		app.installTranslator(&translator);

	if (argc <= 1)
		return EXIT_SUCCESS;

	QString path = argv[1];

		QFile::remove(path + "/Multiuso" + Multiuso::currentSuffix()); // Old version

		QFile::copy(Multiuso::appDirPath("Updater") + "/Multiuso" + Multiuso::currentSuffix(), // New version
				path + "/Multiuso" + Multiuso::currentSuffix());

		QProcess::startDetached(path + "/Multiuso" + Multiuso::currentSuffix());

	return EXIT_SUCCESS;;
}
