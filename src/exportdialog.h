/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
 * and Copyright 2012 Miklos Juhasz (mjuhasz)
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

class SubtitleProcessor;

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget *parent = 0, QString filePath = "", SubtitleProcessor* subtitleProcessor = 0);
    ~ExportDialog();
    QString getFileName() { return saveFileName; }
    
private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_browseButton_clicked();
    void on_fileNameLineEdit_textChanged(const QString &inFileName);
    void on_exportPGCEditFormatCheckBox_toggled(bool checked);
    void on_exportForcedOnlyCheckBox_toggled(bool checked);

    void on_languageComboBox_currentIndexChanged(int index);

private:
    Ui::ExportDialog *ui;
    QString filter = tr("All Files (*.*);;(*.");
    QString selectedFilter = tr("(*.");
    QString saveFileName;
    SubtitleProcessor* subtitleProcessor;
    int languageIdx;
    bool exportForced;
    bool writePGCPal;
};

#endif // EXPORTDIALOG_H
