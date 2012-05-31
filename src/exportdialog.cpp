#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "Subtitles/subtitleprocessor.h"
#include <QFileInfo>
#include <QFileDialog>

ExportDialog::ExportDialog(QWidget *parent, QString filePath, SubtitleProcessor* subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    saveFileName(filePath)
{
    this->subtitleProcessor = subtitleProcessor;
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    ui->fileNameLineEdit->setText(QDir::toNativeSeparators(saveFileName));
    languageIdx = subtitleProcessor->getLanguageIdx();
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::on_cancelButton_clicked()
{
    reject();
}

void ExportDialog::on_saveButton_clicked()
{
    //TODO: finish implement
    accept();
    subtitleProcessor->setLanguageIdx(languageIdx);
}

void ExportDialog::on_browseButton_clicked()
{
    QString extension = QFileInfo(ui->fileNameLineEdit->text()).suffix();
    filter += QString(extension + ")");
    selectedFilter += QString(extension + ")");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save"),
                                                    ui->fileNameLineEdit->text(),
                                                    filter,
                                                    &selectedFilter);
    ui->fileNameLineEdit->setText(QDir::toNativeSeparators(fileName));
    saveFileName = fileName;
}

void ExportDialog::on_fileNameLineEdit_textChanged(const QString &arg1)
{
    saveFileName = arg1;
}
