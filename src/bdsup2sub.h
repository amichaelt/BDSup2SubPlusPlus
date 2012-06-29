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

#ifndef BDSUP2SUB_H
#define BDSUP2SUB_H

#include <QMainWindow>
#include <QColor>
#include <QIntValidator>

class SubtitleProcessor;
class ProgressDialog;
class QSettings;
class QxtCommandOptions;
class QTextStream;

namespace Ui {
class BDSup2Sub;
}

class BDSup2Sub : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BDSup2Sub(QWidget *parent = 0);
    ~BDSup2Sub();
    void closeEvent(QCloseEvent *event);

    bool execCLI(int argc, char** argv);

public slots:
    void changeWindowTitle(QString newTitle);
    void onLoadingSubtitleFileFinished(const QString& errorString);
    void onWritingSubtitleFileFinished(const QString& errorString);
    void onMoveAllFinished(const QString& errorString);
    void convertSup();
    
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void showEvent(QShowEvent *event);
    void resizeEvent (QResizeEvent * event);

private:
    Ui::BDSup2Sub *ui;
    ProgressDialog *progressDialog = 0;
    QString loadPath = "";
    QString saveFileName = "";
    QString savePath = "";
    QString colorPath = "";
    SubtitleProcessor *subtitleProcessor = 0;
    int subIndex = 0;
    QIntValidator *alphaThresholdValidator = new QIntValidator(0, 255, this);
    QIntValidator *medLowThresholdValidator = new QIntValidator(0, 255, this);
    QIntValidator *hiMedThresholdValidator = new QIntValidator(0, 255, this);
    QIntValidator *subtitleNumberValidator;
    QPalette* errorBackground;
    QPalette* okBackground;

    QxtCommandOptions* options;

    QSettings* settings;
    bool fromCLI = false;

    QString filter = tr("All Files (*.*);;Subtitle Files (*.idx *.ifo *.sub *.sup *.xml)");
    QString ifoFilter = tr("IFO File (*.ifo)");
    QString selectedFilter = tr("Subtitle Files (*.idx *.ifo *.sub *.sup *.xml)");

    void fillComboBoxes();
    void enableCoreComponents(bool enable);
    void enableVobSubComponents(bool enable);
    void enableVobSubMenuCombo();
    void closeSubtitle();
    void updateRecentMenu();
    void refreshSrcFrame(int index);
    void refreshTrgFrame(int index);
    void connectSubtitleProcessor();
    void loadSubtitleFile();
    QString getWarningMessage();
    void warningDialog();
    void errorDialog(const QString &errorMessage);
    void printWarnings();
    void Redirect_console();
    void addCLIOptions();
    void showUsage(QTextStream& outStream);

private slots:
    void init();
    void loadSettings();
    void showAboutQt();
    void openFile();
    void saveFile();
    void closeFile();
    void print(const QString &message);
    void onRecentItemClicked();
    void onEditPaneClicked(QMouseEvent *event);
    void loadEditPane();
    void loadHelpDialog();
    void swapCrCb_toggled(bool checked);
    void fixInvisibleFrames_toggled(bool checked);
    void verbatimOutput_toggled(bool checked);
    void editDefaultDVDPalette_triggered();
    void editImportedDVDPalette_triggered();
    void editDVDFramePalette_triggered();
    void moveAllCaptions_triggered();
    void resetCropOffset_triggered();
    void on_subtitleNumberComboBox_currentIndexChanged(int index);
    void on_subtitleNumberComboBox_editTextChanged(const QString &index);
    void openConversionSettings();
    void on_paletteComboBox_currentIndexChanged(int index);
    void on_filterComboBox_currentIndexChanged(int index);
    void on_hiMedThresholdComboBox_currentIndexChanged(int index);
    void on_hiMedThresholdComboBox_editTextChanged(const QString &arg1);
    void on_medLowThresholdComboBox_currentIndexChanged(int index);
    void on_medLowThresholdComboBox_editTextChanged(const QString &arg1);
    void on_alphaThresholdComboBox_currentIndexChanged(int index);
    void on_alphaThresholdComboBox_editTextChanged(const QString &arg1);
    void on_outputFormatComboBox_currentIndexChanged(int index);
};

#endif // BDSUP2SUB_H
