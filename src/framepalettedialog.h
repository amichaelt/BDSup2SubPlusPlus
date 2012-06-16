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

#ifndef FRAMEPALETTEDIALOG_H
#define FRAMEPALETTEDIALOG_H

#include <QDialog>

class SubtitleProcessor;

namespace Ui {
class FramePaletteDialog;
}

class FramePaletteDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FramePaletteDialog(QWidget *parent = 0, SubtitleProcessor* subtitleProcessor = 0);
    ~FramePaletteDialog();

    void setIndex(int idx);
    
private slots:

    void on_color1ComboBox_currentIndexChanged(int index);
    void on_color2ComboBox_currentIndexChanged(int index);
    void on_color3ComboBox_currentIndexChanged(int index);
    void on_color4ComboBox_currentIndexChanged(int index);
    void on_alpha1ComboBox_currentIndexChanged(int index);
    void on_alpha2ComboBox_currentIndexChanged(int index);
    void on_alpha3ComboBox_currentIndexChanged(int index);
    void on_alpha4ComboBox_currentIndexChanged(int index);

    void on_resetAllButton_clicked();
    void on_setAllButton_clicked();
    void on_cancelButton_clicked();
    void on_resetButton_clicked();
    void on_okButton_clicked();

private:
    Ui::FramePaletteDialog *ui;
    SubtitleProcessor* subtitleProcessor = 0;
    QStringList colorNames = {	"00", "01", "02", "03", "04", "05", "06" ,"07",
                                "08", "09", "10", "11", "12", "13", "14", "15"
                             };
    QVector<QIcon> colorIcons;
    int index;
    QVector<int> alpha;
    QVector<int> pal;
};

#endif // FRAMEPALETTEDIALOG_H
