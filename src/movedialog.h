#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>
#include "types.h"

class SubtitleProcessor;
class SubPicture;
class QImage;
class QButtonGroup;
class QIntValidator;
class QDoubleValidator;

namespace Ui {
class MoveDialog;
}

class MoveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MoveDialog(QWidget *parent = 0, SubtitleProcessor* subtitleProcessor = 0);
    ~MoveDialog();
    void keyPressEvent(QKeyEvent *event);
    void setIndex(int idx);
    int getIndex() { return index; }
    double getTrgRatio() { return screenRatioTrg; }
    
private slots:
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_cropBarsButton_clicked();
    void on_cancelButton_clicked();
    void on_moveAllButton_clicked();
    void on_aspectRatio1Button_clicked();
    void on_aspectRatio2Button_clicked();
    void on_aspectRatio3Button_clicked();

    void on_keepXPositionRadioButton_clicked();
    void on_moveLeftRadioButton_clicked();
    void on_moveRightRadioButton_clicked();
    void on_moveToCenterRadioButton_clicked();
    void on_keepYPositionRadioButton_clicked();
    void on_moveInsideBoundsRadioButton_clicked();
    void on_moveOutsideBoundsRadioButton_clicked();

    void on_xOffsetLineEdit_textChanged(const QString &arg1);
    void on_yOffsetLineEdit_textChanged(const QString &arg1);
    void on_aspectRatioLineEdit_textChanged(const QString &arg1);
    void on_cropOffsetYLineEdit_textChanged(const QString &arg1);

    void on_xOffsetLineEdit_editingFinished();
    void on_yOffsetLineEdit_editingFinished();
    void on_aspectRatioLineEdit_editingFinished();
    void on_cropOffsetYLineEdit_editingFinished();

private:
    Ui::MoveDialog *ui;
    SubtitleProcessor* subtitleProcessor;
    SubPicture* subPicture;
    QButtonGroup* xButtonGroup;
    QButtonGroup* yButtonGroup;
    int index;
    double screenRatioTrg = 21.0 / 9;
    double screenRatio = 16.0 / 9;
    double cineBarFactor = 5.0 / 42;
    QImage* image = 0;
    MoveModeX moveModeX = MoveModeX::KEEP;
    MoveModeY moveModeY = MoveModeY::KEEP;
    int originalX;
    int originalY;
    int offsetX;
    int offsetY;
    int cropOfsY;

    QIntValidator* xOffsetValidator;
    QIntValidator* yOffsetValiator;
    QIntValidator* cropOffsetValidator;
    QDoubleValidator* aspectRatioValidator;

    QPalette* errorBackground;
    QPalette* okBackground;

    void setRatio(double ratio);
    void move();
};

#endif // MOVEDIALOG_H
