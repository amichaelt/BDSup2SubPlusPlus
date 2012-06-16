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

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>

class QModelIndex;
class SubtitleProcessor;

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ColorDialog(QWidget *parent = 0, SubtitleProcessor* subtitleProcessor = 0);
    ~ColorDialog();

    void setParameters(QStringList names, QVector<QColor> currentColor, QVector<QColor> defaultColor);
    void setPath(QString value) { colorPath = value; }
    QString getPath() { return colorPath; }
    QVector<QColor> getColors() { return selectedColors; }
    
private slots:
    void on_colorList_doubleClicked(const QModelIndex &index);

    void on_changeColorButton_clicked();
    void on_restoreDefaultColorsButton_clicked();
    void on_savePaletteButton_clicked();
    void on_loadPaletteButton_clicked();
    void on_cancelButton_clicked();
    void on_okButton_clicked();

private:
    Ui::ColorDialog *ui;
    SubtitleProcessor* subtitleProcessor = 0;
    QVector<QIcon> colorIcons;
    QVector<QColor> selectedColors;
    QVector<QColor> defaultColors;
    QStringList colorNames;
    QString colorPath;

    QString filter = tr("All Files (*.*);;(*.ini)");
    QString selectedFilter = tr("(*.ini)");

    void changeColor(const QModelIndex &index);
};

#endif // COLORDIALOG_H
