/********************************************************************************
** Form generated from reading UI file 'colordialog.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORDIALOG_H
#define UI_COLORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ColorDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *chooseColorLabel;
    QHBoxLayout *horizontalLayout_2;
    QListView *colorList;
    QVBoxLayout *verticalLayout;
    QPushButton *changeColorButton;
    QPushButton *restoreDefaultColorsButton;
    QPushButton *savePaletteButton;
    QPushButton *loadPaletteButton;
    QHBoxLayout *horizontalLayout;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *ColorDialog)
    {
        if (ColorDialog->objectName().isEmpty())
            ColorDialog->setObjectName(QString::fromUtf8("ColorDialog"));
        ColorDialog->resize(365, 200);
        ColorDialog->setMinimumSize(QSize(365, 200));
        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        ColorDialog->setFont(font);
        ColorDialog->setModal(true);
        gridLayout = new QGridLayout(ColorDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        chooseColorLabel = new QLabel(ColorDialog);
        chooseColorLabel->setObjectName(QString::fromUtf8("chooseColorLabel"));

        gridLayout->addWidget(chooseColorLabel, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        colorList = new QListView(ColorDialog);
        colorList->setObjectName(QString::fromUtf8("colorList"));
        colorList->setMinimumSize(QSize(150, 0));

        horizontalLayout_2->addWidget(colorList);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        changeColorButton = new QPushButton(ColorDialog);
        changeColorButton->setObjectName(QString::fromUtf8("changeColorButton"));
        changeColorButton->setMinimumSize(QSize(170, 0));
        changeColorButton->setAutoDefault(false);

        verticalLayout->addWidget(changeColorButton);

        restoreDefaultColorsButton = new QPushButton(ColorDialog);
        restoreDefaultColorsButton->setObjectName(QString::fromUtf8("restoreDefaultColorsButton"));
        restoreDefaultColorsButton->setMinimumSize(QSize(170, 0));
        restoreDefaultColorsButton->setAutoDefault(false);

        verticalLayout->addWidget(restoreDefaultColorsButton);

        savePaletteButton = new QPushButton(ColorDialog);
        savePaletteButton->setObjectName(QString::fromUtf8("savePaletteButton"));
        savePaletteButton->setMinimumSize(QSize(170, 0));
        savePaletteButton->setAutoDefault(false);

        verticalLayout->addWidget(savePaletteButton);

        loadPaletteButton = new QPushButton(ColorDialog);
        loadPaletteButton->setObjectName(QString::fromUtf8("loadPaletteButton"));
        loadPaletteButton->setMinimumSize(QSize(170, 0));
        loadPaletteButton->setAutoDefault(false);

        verticalLayout->addWidget(loadPaletteButton);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cancelButton = new QPushButton(ColorDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMinimumSize(QSize(70, 0));
        cancelButton->setAutoDefault(false);

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(ColorDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setMinimumSize(QSize(70, 0));
        okButton->setAutoDefault(false);
        okButton->setDefault(true);

        horizontalLayout->addWidget(okButton);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 6);
        horizontalLayout->setStretch(2, 1);

        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        QWidget::setTabOrder(okButton, colorList);
        QWidget::setTabOrder(colorList, changeColorButton);
        QWidget::setTabOrder(changeColorButton, restoreDefaultColorsButton);
        QWidget::setTabOrder(restoreDefaultColorsButton, savePaletteButton);
        QWidget::setTabOrder(savePaletteButton, loadPaletteButton);
        QWidget::setTabOrder(loadPaletteButton, cancelButton);

        retranslateUi(ColorDialog);

        QMetaObject::connectSlotsByName(ColorDialog);
    } // setupUi

    void retranslateUi(QDialog *ColorDialog)
    {
        ColorDialog->setWindowTitle(QApplication::translate("ColorDialog", "Choose Colors", 0, QApplication::UnicodeUTF8));
        chooseColorLabel->setText(QApplication::translate("ColorDialog", "Choose Color", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        changeColorButton->setToolTip(QApplication::translate("ColorDialog", "Edit the selected color", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        changeColorButton->setText(QApplication::translate("ColorDialog", "&Change Color", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        restoreDefaultColorsButton->setToolTip(QApplication::translate("ColorDialog", "Revert to default colors", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        restoreDefaultColorsButton->setText(QApplication::translate("ColorDialog", "&Restore default Colors", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        savePaletteButton->setToolTip(QApplication::translate("ColorDialog", "Save the current palette settings in an INI file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        savePaletteButton->setText(QApplication::translate("ColorDialog", "&Save Palette", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        loadPaletteButton->setToolTip(QApplication::translate("ColorDialog", "Load palette settings from an INI file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        loadPaletteButton->setText(QApplication::translate("ColorDialog", "&Load Palette", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("ColorDialog", "Lose changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("ColorDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        okButton->setToolTip(QApplication::translate("ColorDialog", "Apply changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        okButton->setText(QApplication::translate("ColorDialog", "&Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ColorDialog: public Ui_ColorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORDIALOG_H
