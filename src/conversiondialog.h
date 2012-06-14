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

class QPalette;
class SubtitleProcessor;
class QDoubleValidator;
class QRegExpValidator;
class QSettings;

namespace Ui {
class ConversionDialog;
}

class ConversionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConversionDialog(QWidget *parent = 0, SubtitleProcessor *subtitleProcessor = 0, QSettings* settings = 0);
    ~ConversionDialog();

    void enableOptionMove(bool enable);
    void keyPressEvent(QKeyEvent *event);
    
private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_storeButton_clicked();
    void on_restoreButton_clicked();
    void on_resetButton_clicked();

    void on_convertResolutionCheckBox_toggled(bool checked);
    void on_applyMoveAllSettingsCheckBox_toggled(bool checked);
    void on_changeFrameRateCheckBox_toggled(bool checked);
    void on_scaleCheckBox_toggled(bool checked);
    void on_fixTooShortFramesCheckBox_toggled(bool checked);

    void on_resolutionComboBox_currentIndexChanged(int index);
    void on_forceFlagsComboBox_currentIndexChanged(int index);
    void on_sourceFramerateComboBox_currentIndexChanged(const QString &arg1);
    void on_targetFramerateComboBox_currentIndexChanged(const QString &arg1);

    void on_scaleXLineEdit_textChanged(const QString &arg1);
    void on_scaleYLineEdit_textChanged(const QString &arg1);
    void on_delayLineEdit_textChanged(const QString &arg1);
    void on_minTimeLineEdit_textChanged(const QString &arg1);
    void on_sourceFramerateComboBox_editTextChanged(const QString &arg1);
    void on_targetFramerateComboBox_editTextChanged(const QString &arg1);

    void on_scaleXLineEdit_editingFinished();
    void on_scaleYLineEdit_editingFinished();
    void on_delayLineEdit_editingFinished();
    void on_minTimeLineEdit_editingFinished();

private:
    Ui::ConversionDialog *ui;
    SubtitleProcessor *subtitleProcessor;

    QRegExpValidator* fpsSrcValidator;
    QRegExpValidator* fpsTrgValidator;
    QDoubleValidator* scaleXValidator;
    QDoubleValidator* scaleYValidator;
    QDoubleValidator* delayPTSValidator;
    QDoubleValidator* minTimePTSValidator;

    QSettings* settings;

    QPalette* errorBackground;
    QPalette* warnBackground;
    QPalette* okBackground;

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
    void on_sourceFramerateComboBox_editingFinished();
    void on_targetFramerateComboBox_editingFinished();
};

#endif // CONVERSIONDIALOG_H
