#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

class SubtitleProcessor;

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportDialog(QWidget *parent = 0, QString filePath = "", SubtitleProcessor* subtitleProcessor = 0);
    ~ExportDialog();
    QString getFileName() { return saveFileName; }
    
private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_browseButton_clicked();
    void on_fileNameLineEdit_textChanged(const QString &inFileName);
    void on_exportPGCEditFormatCheckBox_toggled(bool checked);
    void on_exportForcedOnlyCheckBox_toggled(bool checked);

    void on_languageComboBox_currentIndexChanged(int index);

private:
    Ui::ExportDialog *ui;
    QString filter = tr("All Files (*.*);;(*.");
    QString selectedFilter = tr("(*.");
    QString saveFileName;
    SubtitleProcessor* subtitleProcessor;
    int languageIdx;
    bool exportForced;
    bool writePGCPal;
};

#endif // EXPORTDIALOG_H
