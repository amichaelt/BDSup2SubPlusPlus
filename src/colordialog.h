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

    void changeColor(const QModelIndex &index);
};

#endif // COLORDIALOG_H
