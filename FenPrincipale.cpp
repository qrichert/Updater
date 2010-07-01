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

#include "FenPrincipale.h"

FenPrincipale::FenPrincipale()
{
	setWindowIcon(QIcon(":/icones/other/mainIcon.png"));
	setWindowTitle("Multiuso - Updater");
	setFixedSize(500, 350);
	
	m_open = new QPushButton("Ouvrir...");
		m_open->setIcon(QIcon(":/icones/other/open.png"));
		connect(m_open, SIGNAL(clicked()), this, SLOT(slotSelectFile()));

	m_showFile = new QLabel("Sélectionnez un dossier...");

	m_file = "";

	m_update = new QPushButton("Lancer la mise à jour");
		m_update->setIcon(QIcon(":/icones/other/start.png"));
		m_update->setDisabled(true);
		connect(m_update, SIGNAL(clicked()), this, SLOT(slotStartUpdate()));

	m_progress = new QProgressBar;

	QLabel *hr = new QLabel("<hr />");

	QStringList headers;
		headers << "Fichier" << "Action";

	m_progressView = new QTableWidget(0, 2);
		m_progressView->setShowGrid(false);
		m_progressView->verticalHeader()->hide();
		m_progressView->horizontalHeader()->setStretchLastSection(true);
		m_progressView->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_progressView->setHorizontalHeaderLabels(headers);

	QGridLayout *mainLayout = new QGridLayout;
		mainLayout->addWidget(m_open, 0, 0, 1, 1);
		mainLayout->addWidget(m_update, 0, 2, 1, 3);
		mainLayout->addWidget(m_showFile, 1, 0, 1, 1);
		mainLayout->addWidget(hr, 1, 2, 1, 3);
		mainLayout->addWidget(m_progress, 2, 0, 1, 5);
		mainLayout->addWidget(m_progressView, 3, 0, 1, 5);

	QWidget *mainWidget = new QWidget;
		mainWidget->setLayout(mainLayout);

	setCentralWidget(mainWidget);
	
	updateInProgress = false;
}

void FenPrincipale::closeEvent(QCloseEvent *event)
{
	if (updateInProgress)
		event->ignore();

	else
		qApp->quit();
}

void FenPrincipale::slotSelectFile()
{
	QString fileName = QFileDialog::getExistingDirectory(this, "Sélectionnez le dossier contenant Multiuso.",
			QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (fileName.isEmpty())
		return;

	if (!fileName.endsWith("/"))
		fileName += "/";

	QFile updateFile(fileName + "UPDATE");

		if (!updateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QMessageBox::critical(this, "Multiuso - Updater", "Le fichier \"UPDATE\" est introuvable !<br />"
					"Impossible d'effectuer la mise à jour.");

			return;
		}
	
		QString updateActions = updateFile.readAll();

		updateActionsList = updateActions.split("\n");
			updateActionsList.removeOne(".");
			updateActionsList.removeOne("..");

		updateFile.close();

	m_file = fileName;

	if (fileName.size() > 35)
		fileName = fileName.left(32).append("...");

	m_showFile->setText(fileName);
	m_update->setDisabled(false);
}

void FenPrincipale::slotStartUpdate()
{
	updateInProgress = true;

	m_progress->setMaximum(updateActionsList.size());

	foreach (QString command, updateActionsList)
	{
		if (command.isEmpty())
			continue;

		QString currentFile = command;
			currentFile = currentFile.replace(QRegExp("(.+) : (.+)"), "\\2");
			currentFile = currentFile.replace("${SUFFIX}", Multiuso::currentSuffix());

		QString commandType = "";
		QString fileType = "";

		if (command.startsWith("REMOVE_FILE : "))
		{
			currentFile = currentFile.replace("${PATH}", Multiuso::appDirPath());
			QFile(currentFile).remove();
		
			commandType = "Supprimer fichier";
			fileType = "Fichier";
		}

		else if (command.startsWith("REMOVE_DIR : "))
		{
			currentFile = currentFile.replace("${PATH}", Multiuso::appDirPath());
			Multiuso::removeDirectory(QDir(currentFile));
			
			commandType = "Supprimer dossier";
			fileType = "Dossier";
		}

		else if (command.startsWith("ADD_FILE : "))
		{
			QString firstFile = currentFile;
				firstFile = firstFile.replace("${PATH}", m_file);
				
			QString secondFile = currentFile;
				secondFile = secondFile.replace("${PATH}", Multiuso::appDirPath());

			QFile::copy(firstFile, secondFile);			

			commandType = "Ajouter fichier";
			fileType = "Fichier";
		}

		else if (command.startsWith("ADD_DIR : "))
		{
			currentFile = currentFile.replace("${PATH}", Multiuso::appDirPath());

			QDir dir(currentFile);
				dir.mkpath(currentFile);
			
			commandType = "Ajouter dossier";
			fileType = "Dossier";
		}

		QTableWidgetItem *name = new QTableWidgetItem(QFileInfo(currentFile).fileName());
			name->setFlags(name->flags() & ~Qt::ItemIsEditable);
			name->setIcon(Multiuso::iconForFile(currentFile, fileType));

		QTableWidgetItem *type = new QTableWidgetItem(commandType);
			type->setFlags(type->flags() & ~Qt::ItemIsEditable);
			
		int row = m_progressView->rowCount();

		m_progressView->setRowCount(m_progressView->rowCount() + 1);
		m_progressView->setItem(row, 0, name);
		m_progressView->setItem(row, 1, type);
		
		m_progressView->resizeColumnsToContents();
		m_progressView->horizontalHeader()->setStretchLastSection(true);

		m_progress->setValue(m_progress->value() + 1);
	}

	m_progress->setValue(m_progress->maximum());
	updateInProgress = false;

	int reponse = QMessageBox::question(this, "Multiuso - Updater", "Mise à jour terminée, voulez-vous "
			"supprimer les données de mise à jour et lancer Multiuso ?",
			QMessageBox::Yes | QMessageBox::No);

	if (reponse == QMessageBox::Yes)
	{
		Multiuso::removeDirectory(m_file);

		QProcess::startDetached(Multiuso::openCommand() + Multiuso::appDirPath() + "/Multiuso" + Multiuso::currentSuffix());

		qApp->quit();
	}
}
