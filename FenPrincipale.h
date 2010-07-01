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

#ifndef HEADER_FENPRINCIPALE
#define HEADER_FENPRINCIPALE

#include "../Multiuso/Multiuso.h"

class FenPrincipale : public QMainWindow
{
	Q_OBJECT

	public:
		FenPrincipale();

	protected:
		void closeEvent(QCloseEvent *event);

	public slots:
		void slotSelectFile();
		void slotStartUpdate();

	private:
		QPushButton *m_open;
		QLabel *m_showFile;
		QString m_file;
		QStringList updateActionsList;
		QPushButton *m_update;
		QProgressBar *m_progress;
		QTableWidget *m_progressView;
		bool updateInProgress;
};

#endif
