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

#ifndef CONVERSIONDIALOG_H
#define CONVERSIONDIALOG_H

#include "types.h"

#include <QDialog>

class SubtitleProcessor;
class QDoubleValidator;
class QRegExpValidator;

namespace Ui {
class ConversionDialog;
}

class ConversionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConversionDialog(QWidget *parent = 0, SubtitleProcessor *subtitleProcessor = 0);
    ~ConversionDialog();

    void enableOptionMove(bool enable);
    
private slots:
    void on_cancelButton_clicked();
    void on_okButton_clicked();
    void on_scaleCheckBox_toggled(bool checked);
    void on_fixTooShortFramesCheckBox_toggled(bool checked);
    void on_changeFrameRateCheckBox_toggled(bool checked);
    void on_resetButton_clicked();
    void on_convertResolutionCheckBox_toggled(bool checked);
    void on_applyMoveAllSettingsCheckBox_toggled(bool checked);
    void on_resolutionComboBox_currentIndexChanged(int index);
    void on_sourceFramerateComboBox_editTextChanged(const QString &arg1);
    void on_targetFramerateComboBox_editTextChanged(const QString &arg1);
    void on_scaleXLineEdit_textChanged(const QString &arg1);
    void on_scaleYLineEdit_textChanged(const QString &arg1);
    void on_delayLineEdit_textChanged(const QString &arg1);
    void on_minTimeLineEdit_textChanged(const QString &arg1);

private:
    Ui::ConversionDialog *ui;
    SubtitleProcessor *subtitleProcessor;

    QColor errBackground = QColor(0xffe1acac);
    QColor warnBackground = QColor(0xffffffc0);
    QColor okBackground = QColor(0xff000000);
    QRegExpValidator* fpsSrcValidator;
    QRegExpValidator* fpsTrgValidator;
    QDoubleValidator* scaleXValidator;
    QDoubleValidator* scaleYValidator;
    QDoubleValidator* delayPTSValidator;
    QDoubleValidator* minTimePTSValidator;

    Resolution resolution;
    int delayPTS;
    int minTimePTS = 520;
    bool changeFPS;
    bool changeResolution;
    bool fixShortFrames;
    double fpsSrc;
    double fpsTrg;
    bool isReady = false;
    bool changeScale;
    double scaleX = 1;
    double scaleY = 1;
    bool fpsSrcCertain;
    SetState forcedState = SetState::KEEP;
    bool moveCaptions;

    void fillDialog();
};

#endif // CONVERSIONDIALOG_H
