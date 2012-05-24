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

#ifndef BDSUP2SUB_H
#define BDSUP2SUB_H

#include <QMainWindow>

class SubtitleProcessor;
class ProgressDialog;
class ConversionDialog;

namespace Ui {
class BDSup2Sub;
}

class BDSup2Sub : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BDSup2Sub(QWidget *parent = 0);
    ~BDSup2Sub();

public slots:
    void changeWindowTitle(QString newTitle);
    void onLoadingSubtitleFileFinished();
    void convertSup();
    
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Ui::BDSup2Sub *ui;
    ProgressDialog *progressDialog = 0;
    ConversionDialog *conversionDialog = 0;
    QString loadPath = "";
    QString saveFileName = "";
    QString savePath = "";
    SubtitleProcessor *subtitleProcessor = 0;
    int subIndex = 0;

    QString filter = tr("All Files (*.*);Subtitle Files (*.idx *.ifo *.sub *.sup *.xml)");
    QString selectedFilter = tr("Subtitle Files (*.idx *.ifo *.sub *.sup *.xml)");

    void fillComboBoxes();
    void enableCoreComponents(bool enable);
    void enableVobSubComponents(bool enable);
    void enableVobSubMenuCombo();
    void loadSubtitleFile();
    void closeSubtitle();
    void updateRecentMenu();
    void refreshSrcFrame(int index);
    void refreshTrgFrame(int index);

private slots:
    void openFile();
};

#endif // BDSUP2SUB_H
