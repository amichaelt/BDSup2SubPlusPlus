#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

class SubtitleProcessor;
class SubPicture;
class QImage;
class QDoubleValidator;
class QIntValidator;
class QRegExpValidator;
class QPalette;

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EditDialog(QWidget *parent = 0, SubtitleProcessor* subtitleProcessor = 0);
    ~EditDialog();
    
    void setIndex(int value);
    int getIndex() { return index; }
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_minButton_clicked();
    void on_maxButton_clicked();
    void on_centerButton_clicked();
    void on_topButton_clicked();
    void on_bottomButton_clicked();
    void on_addErasePatchButton_clicked();
    void on_undoErasePatchButton_clicked();
    void on_undoAllErasePatchesButton_clicked();
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_okButton_clicked();

    void on_excludeCheckBox_toggled(bool checked);
    void on_forcedCaptionCheckBox_toggled(bool checked);

    void on_verticalSlider_valueChanged(int value);
    void on_horizontalSlider_valueChanged(int value);

    void on_startTimeLineEdit_textChanged(const QString &arg1);
    void on_endTimeLineEdit_textChanged(const QString &arg1);
    void on_durationLineEdit_textChanged(const QString &arg1);
    void on_xOffsetLineEdit_textChanged(const QString &arg1);
    void on_yOffsetLineEdit_textChanged(const QString &arg1);

    void on_startTimeLineEdit_editingFinished();
    void on_endTimeLineEdit_editingFinished();
    void on_durationLineEdit_editingFinished();
    void on_xOffsetLineEdit_editingFinished();
    void on_yOffsetLineEdit_editingFinished();

    void onSelectionPerformed(bool validSelection);

private:
    Ui::EditDialog *ui;
    QImage* image = 0;
    SubtitleProcessor* subtitleProcessor = 0;
    SubPicture* subPicture;
    SubPicture* subPictureNext = 0;
    SubPicture* subPicturePrevious = 0;
    QDoubleValidator* durationValidator;
    QIntValidator* xOffsetValidator;
    QIntValidator* yOffsetValidator;
    QRegExpValidator* startTimeValidator;
    QRegExpValidator* endTimeValidator;
    QPalette* errorBackground;
    QPalette* warnBackground;
    QPalette* okBackground;
    int frameTime = 0;
    int index = 0;
    int minimumWidth = 768;
    int minimumHeight = 432;
    bool enableSliders = false;
    bool edited = false;
    bool isReady = false;

    void setEdited(bool edited);
    void store();
};

#endif // EDITDIALOG_H
