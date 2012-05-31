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

    void on_fileNameLineEdit_textChanged(const QString &arg1);

private:
    Ui::ExportDialog *ui;
    QString filter = tr("All Files (*.*);(.");
    QString selectedFilter = tr("(.");
    QString saveFileName;
    SubtitleProcessor* subtitleProcessor;
    int languageIdx;
};

#endif // EXPORTDIALOG_H
