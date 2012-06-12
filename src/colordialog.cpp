#include "colordialog.h"
#include "ui_colordialog.h"
#include "Subtitles/subtitleprocessor.h"

#include <QColorDialog>
#include <QPixmap>
#include <QPainter>
#include <QStandardItemModel>
#include <QFileDialog>

ColorDialog::ColorDialog(QWidget *parent, SubtitleProcessor* subtitleProcessor) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    this->subtitleProcessor = subtitleProcessor;
}

ColorDialog::~ColorDialog()
{
    delete ui;
}

void ColorDialog::setParameters(QStringList names, QVector<QColor> currentColor, QVector<QColor> defaultColor)
{
    QPixmap* pixmap;
    colorNames = names;
    defaultColors = defaultColor;

    QStandardItemModel* model = new QStandardItemModel(colorNames.size(), 2);
    for (int i = 0; i < colorNames.size(); ++i)
    {
        selectedColors.insert(i, QColor(currentColor[i].rgb()));
        pixmap = new QPixmap(12, 12);
        pixmap->fill(selectedColors[i]);
        colorIcons.insert(i, QIcon(*pixmap));
        QStandardItem* item = new QStandardItem(colorIcons[i], colorNames[i]);
        model->setItem(i, item);
    }
    ui->colorList->setModel(model);
    QModelIndex index = ui->colorList->model()->index(0, 0);
    ui->colorList->setCurrentIndex(index);
    ui->colorList->update();
}

void ColorDialog::on_colorList_doubleClicked(const QModelIndex &index)
{
    changeColor(index);
}

void ColorDialog::on_changeColorButton_clicked()
{
    changeColor(ui->colorList->currentIndex());
}

void ColorDialog::on_restoreDefaultColorsButton_clicked()
{
    QPixmap* pixmap;
    QStandardItemModel* model = new QStandardItemModel(colorNames.size(), 2);

    for (int i = 0; i < colorNames.size(); ++i)
    {
        selectedColors.replace(i, QColor(defaultColors[i].rgb()));
        pixmap = new QPixmap(12, 12);
        pixmap->fill(selectedColors[i]);
        colorIcons.insert(i, QIcon(*pixmap));
        QStandardItem* item = new QStandardItem(colorIcons[i], colorNames[i]);
        model->setItem(i, item);
    }
    ui->colorList->setModel(model);
    QModelIndex index = ui->colorList->model()->index(0, 0);
    ui->colorList->setCurrentIndex(index);
    ui->colorList->update();
}

void ColorDialog::on_savePaletteButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save"),
                                                    colorPath == "" ? QDir::currentPath() :
                                                                      QFileInfo(colorPath).absolutePath(),
                                                    filter,
                                                    &selectedFilter);

    if (filePath.isEmpty() || filePath.isNull()) return;

    colorPath = filePath;
    //TODO: implement props
}

void ColorDialog::on_loadPaletteButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Save"),
                                                    colorPath == "" ? QDir::currentPath() :
                                                                      QFileInfo(colorPath).absolutePath(),
                                                    filter,
                                                    &selectedFilter);

    if (filePath.isNull() || filePath.isEmpty()) return;

    colorPath = filePath;
    //TODO: implement props
}

void ColorDialog::on_cancelButton_clicked()
{
    selectedColors = defaultColors;
    reject();
}

void ColorDialog::on_okButton_clicked()
{
    accept();
}

void ColorDialog::changeColor(const QModelIndex &index)
{
    int idx = index.row();
    QColor color = QColorDialog::getColor(selectedColors[idx], this);
    if (color.isValid())
    {
        selectedColors.replace(idx, color);
    }
    QPixmap* pixmap = new QPixmap(12, 12);
    pixmap->fill(QColor(selectedColors[idx].rgb()));
    colorIcons.replace(idx, QIcon(*pixmap));
    QStandardItem* item = new QStandardItem(colorIcons[idx], colorNames[idx]);
    ((QStandardItemModel*)ui->colorList->model())->setItem(idx, item);
    ui->colorList->update();
}
