#include "helpdialog.h"
#include "ui_helpdialog.h"
#include "types.h"

#include <QFile>

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    setWindowTitle(progNameVer + " Help");
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    QFile file(":/new/resources/help.htm");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    ui->textBrowser->setHtml(file.readAll());
    file.close();
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
