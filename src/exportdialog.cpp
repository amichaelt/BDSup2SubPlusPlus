/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
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

#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "Subtitles/subtitleprocessor.h"
#include <QFileInfo>
#include <QFileDialog>

ExportDialog::ExportDialog(QWidget *parent, QString filePath, SubtitleProcessor* subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    saveFileName(filePath)
{
    this->subtitleProcessor = subtitleProcessor;
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    ui->fileNameLineEdit->setText(QDir::toNativeSeparators(saveFileName));
    languageIdx = subtitleProcessor->getLanguageIdx();
    exportForced = subtitleProcessor->getNumForcedFrames() > 0 && subtitleProcessor->getExportForced();
    writePGCPal = subtitleProcessor->getWritePGCEditPal();

    for (auto language : subtitleProcessor->getLanguages())
    {
        int n = subtitleProcessor->getOutputMode() == OutputMode::XML ? 2 : 1;
        ui->languageComboBox->addItem(QString(language[0] + " (" + language[n] + ")"));
    }
    ui->languageComboBox->setCurrentIndex(languageIdx);

    if (subtitleProcessor->getOutputMode() == OutputMode::BDSUP)
    {
        ui->languageComboBox->setEnabled(false);
    }

    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getOutputMode() == OutputMode::SUPIFO)
    {
        ui->exportPGCEditFormatCheckBox->setEnabled(true);
        ui->exportPGCEditFormatCheckBox->setChecked(writePGCPal);
    }
    else
    {
        ui->exportPGCEditFormatCheckBox->setEnabled(false);
    }

    ui->exportForcedOnlyCheckBox->setEnabled(subtitleProcessor->getNumForcedFrames() > 0);
    ui->exportForcedOnlyCheckBox->setChecked(exportForced);

    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB)
    {
        this->setWindowTitle("Export SUB/IDX (VobSub)");
    }
    else if (subtitleProcessor->getOutputMode() == OutputMode::SUPIFO)
    {
        this->setWindowTitle("Export SUP/IFO (SUP DVD)");
    }
    else if (subtitleProcessor->getOutputMode() == OutputMode::BDSUP)
    {
        this->setWindowTitle("Export BD SUP");
    }
    else
    {
        this->setWindowTitle("Export XML (SONY BDN)");
    }
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::on_cancelButton_clicked()
{
    reject();
}

void ExportDialog::on_saveButton_clicked()
{
    accept();
    subtitleProcessor->setExportForced(exportForced);
    subtitleProcessor->setLanguageIdx(languageIdx);
    if (subtitleProcessor->getOutputMode() == OutputMode::VOBSUB || subtitleProcessor->getOutputMode() == OutputMode::SUPIFO)
    {
        subtitleProcessor->setWritePGCEditPal(writePGCPal);
    }
}

void ExportDialog::on_browseButton_clicked()
{
    QString extension = QFileInfo(ui->fileNameLineEdit->text()).suffix();
    filter += QString(extension + ")");
    selectedFilter += QString(extension + ")");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save"),
                                                    ui->fileNameLineEdit->text(),
                                                    filter,
                                                    &selectedFilter);

    if (fileName.isNull() || fileName.isEmpty()) return;

    ui->fileNameLineEdit->setText(QDir::toNativeSeparators(fileName));
    saveFileName = fileName;
}

void ExportDialog::on_fileNameLineEdit_textChanged(const QString &inFileName)
{
    saveFileName = inFileName;
}

void ExportDialog::on_exportPGCEditFormatCheckBox_toggled(bool checked)
{
    writePGCPal = checked;
}

void ExportDialog::on_exportForcedOnlyCheckBox_toggled(bool checked)
{
    exportForced = checked;
}

void ExportDialog::on_languageComboBox_currentIndexChanged(int index)
{
    languageIdx = index;
}
