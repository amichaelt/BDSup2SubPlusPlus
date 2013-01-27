/********************************************************************************
** Form generated from reading UI file 'framepalettedialog.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRAMEPALETTEDIALOG_H
#define UI_FRAMEPALETTEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FramePaletteDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *color1Label;
    QComboBox *color1ComboBox;
    QFormLayout *formLayout_2;
    QLabel *color2Label;
    QComboBox *color2ComboBox;
    QFormLayout *formLayout_3;
    QLabel *color3Label;
    QComboBox *color3ComboBox;
    QFormLayout *formLayout_4;
    QLabel *color4Label;
    QComboBox *color4ComboBox;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout_5;
    QLabel *alpha1Label;
    QComboBox *alpha1ComboBox;
    QFormLayout *formLayout_6;
    QLabel *alpha2Label;
    QComboBox *alpha2ComboBox;
    QFormLayout *formLayout_7;
    QLabel *alpha3Label;
    QComboBox *alpha3ComboBox;
    QFormLayout *formLayout_8;
    QLabel *alpha4Label;
    QComboBox *alpha4ComboBox;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *resetAllButton;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *setAllButton;
    QHBoxLayout *horizontalLayout;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *resetButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *okButton;

    void setupUi(QDialog *FramePaletteDialog)
    {
        if (FramePaletteDialog->objectName().isEmpty())
            FramePaletteDialog->setObjectName(QString::fromUtf8("FramePaletteDialog"));
        FramePaletteDialog->resize(290, 185);
        FramePaletteDialog->setMinimumSize(QSize(290, 185));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/resources/bdsup2sub.ico"), QSize(), QIcon::Normal, QIcon::Off);
        FramePaletteDialog->setWindowIcon(icon);
        FramePaletteDialog->setModal(true);
        gridLayout = new QGridLayout(FramePaletteDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        color1Label = new QLabel(FramePaletteDialog);
        color1Label->setObjectName(QString::fromUtf8("color1Label"));
        color1Label->setMinimumSize(QSize(46, 0));

        formLayout->setWidget(0, QFormLayout::LabelRole, color1Label);

        color1ComboBox = new QComboBox(FramePaletteDialog);
        color1ComboBox->setObjectName(QString::fromUtf8("color1ComboBox"));
        color1ComboBox->setMinimumSize(QSize(60, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, color1ComboBox);


        verticalLayout_2->addLayout(formLayout);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        color2Label = new QLabel(FramePaletteDialog);
        color2Label->setObjectName(QString::fromUtf8("color2Label"));
        color2Label->setMinimumSize(QSize(46, 0));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, color2Label);

        color2ComboBox = new QComboBox(FramePaletteDialog);
        color2ComboBox->setObjectName(QString::fromUtf8("color2ComboBox"));
        color2ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, color2ComboBox);


        verticalLayout_2->addLayout(formLayout_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        color3Label = new QLabel(FramePaletteDialog);
        color3Label->setObjectName(QString::fromUtf8("color3Label"));
        color3Label->setMinimumSize(QSize(46, 0));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, color3Label);

        color3ComboBox = new QComboBox(FramePaletteDialog);
        color3ComboBox->setObjectName(QString::fromUtf8("color3ComboBox"));
        color3ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, color3ComboBox);


        verticalLayout_2->addLayout(formLayout_3);

        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        formLayout_4->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        color4Label = new QLabel(FramePaletteDialog);
        color4Label->setObjectName(QString::fromUtf8("color4Label"));
        color4Label->setMinimumSize(QSize(46, 0));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, color4Label);

        color4ComboBox = new QComboBox(FramePaletteDialog);
        color4ComboBox->setObjectName(QString::fromUtf8("color4ComboBox"));
        color4ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, color4ComboBox);


        verticalLayout_2->addLayout(formLayout_4);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout_5 = new QFormLayout();
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        alpha1Label = new QLabel(FramePaletteDialog);
        alpha1Label->setObjectName(QString::fromUtf8("alpha1Label"));
        alpha1Label->setMinimumSize(QSize(46, 0));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, alpha1Label);

        alpha1ComboBox = new QComboBox(FramePaletteDialog);
        alpha1ComboBox->setObjectName(QString::fromUtf8("alpha1ComboBox"));
        alpha1ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, alpha1ComboBox);


        verticalLayout->addLayout(formLayout_5);

        formLayout_6 = new QFormLayout();
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        alpha2Label = new QLabel(FramePaletteDialog);
        alpha2Label->setObjectName(QString::fromUtf8("alpha2Label"));
        alpha2Label->setMinimumSize(QSize(46, 0));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, alpha2Label);

        alpha2ComboBox = new QComboBox(FramePaletteDialog);
        alpha2ComboBox->setObjectName(QString::fromUtf8("alpha2ComboBox"));
        alpha2ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, alpha2ComboBox);


        verticalLayout->addLayout(formLayout_6);

        formLayout_7 = new QFormLayout();
        formLayout_7->setObjectName(QString::fromUtf8("formLayout_7"));
        alpha3Label = new QLabel(FramePaletteDialog);
        alpha3Label->setObjectName(QString::fromUtf8("alpha3Label"));
        alpha3Label->setMinimumSize(QSize(46, 0));

        formLayout_7->setWidget(0, QFormLayout::LabelRole, alpha3Label);

        alpha3ComboBox = new QComboBox(FramePaletteDialog);
        alpha3ComboBox->setObjectName(QString::fromUtf8("alpha3ComboBox"));
        alpha3ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_7->setWidget(0, QFormLayout::FieldRole, alpha3ComboBox);


        verticalLayout->addLayout(formLayout_7);

        formLayout_8 = new QFormLayout();
        formLayout_8->setObjectName(QString::fromUtf8("formLayout_8"));
        alpha4Label = new QLabel(FramePaletteDialog);
        alpha4Label->setObjectName(QString::fromUtf8("alpha4Label"));
        alpha4Label->setMinimumSize(QSize(46, 0));

        formLayout_8->setWidget(0, QFormLayout::LabelRole, alpha4Label);

        alpha4ComboBox = new QComboBox(FramePaletteDialog);
        alpha4ComboBox->setObjectName(QString::fromUtf8("alpha4ComboBox"));
        alpha4ComboBox->setMinimumSize(QSize(60, 0));

        formLayout_8->setWidget(0, QFormLayout::FieldRole, alpha4ComboBox);


        verticalLayout->addLayout(formLayout_8);


        gridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(98, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        resetAllButton = new QPushButton(FramePaletteDialog);
        resetAllButton->setObjectName(QString::fromUtf8("resetAllButton"));
        resetAllButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(resetAllButton);

        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        setAllButton = new QPushButton(FramePaletteDialog);
        setAllButton->setObjectName(QString::fromUtf8("setAllButton"));
        setAllButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(setAllButton);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cancelButton = new QPushButton(FramePaletteDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setAutoDefault(false);

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        resetButton = new QPushButton(FramePaletteDialog);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setAutoDefault(false);

        horizontalLayout->addWidget(resetButton);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        okButton = new QPushButton(FramePaletteDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setAutoDefault(false);
        okButton->setDefault(true);

        horizontalLayout->addWidget(okButton);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 2);

        QWidget::setTabOrder(okButton, color1ComboBox);
        QWidget::setTabOrder(color1ComboBox, color2ComboBox);
        QWidget::setTabOrder(color2ComboBox, color3ComboBox);
        QWidget::setTabOrder(color3ComboBox, color4ComboBox);
        QWidget::setTabOrder(color4ComboBox, alpha1ComboBox);
        QWidget::setTabOrder(alpha1ComboBox, alpha2ComboBox);
        QWidget::setTabOrder(alpha2ComboBox, alpha3ComboBox);
        QWidget::setTabOrder(alpha3ComboBox, alpha4ComboBox);
        QWidget::setTabOrder(alpha4ComboBox, resetAllButton);
        QWidget::setTabOrder(resetAllButton, setAllButton);
        QWidget::setTabOrder(setAllButton, cancelButton);
        QWidget::setTabOrder(cancelButton, resetButton);

        retranslateUi(FramePaletteDialog);

        QMetaObject::connectSlotsByName(FramePaletteDialog);
    } // setupUi

    void retranslateUi(QDialog *FramePaletteDialog)
    {
        FramePaletteDialog->setWindowTitle(QApplication::translate("FramePaletteDialog", "Edit Frame Palette", 0, QApplication::UnicodeUTF8));
        color1Label->setText(QApplication::translate("FramePaletteDialog", "Color 1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        color1ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set palette index of frame color 1", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        color2Label->setText(QApplication::translate("FramePaletteDialog", "Color 2", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        color2ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set palette index of frame color 2", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        color3Label->setText(QApplication::translate("FramePaletteDialog", "Color 3", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        color3ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set palette index of frame color 3", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        color4Label->setText(QApplication::translate("FramePaletteDialog", "Color 4", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        color4ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set palette index of frame color 4", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        alpha1Label->setText(QApplication::translate("FramePaletteDialog", "Alpha 1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        alpha1ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set alpha value of frame color 1", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        alpha2Label->setText(QApplication::translate("FramePaletteDialog", "Alpha 2", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        alpha2ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set alpha value of frame color 2", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        alpha3Label->setText(QApplication::translate("FramePaletteDialog", "Alpha 3", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        alpha3ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set alpha value of frame color 3", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        alpha4Label->setText(QApplication::translate("FramePaletteDialog", "Alpha 4", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        alpha4ComboBox->setToolTip(QApplication::translate("FramePaletteDialog", "Set alpha value of frame color 4", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        resetAllButton->setToolTip(QApplication::translate("FramePaletteDialog", "Revert to original frame palettes for whole stream and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetAllButton->setText(QApplication::translate("FramePaletteDialog", "Reset &All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        setAllButton->setToolTip(QApplication::translate("FramePaletteDialog", "Apply these settings for whole stream and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        setAllButton->setText(QApplication::translate("FramePaletteDialog", "&Set All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("FramePaletteDialog", "Lose changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("FramePaletteDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resetButton->setToolTip(QApplication::translate("FramePaletteDialog", "Revert to original frame palette", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetButton->setText(QApplication::translate("FramePaletteDialog", "&Reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        okButton->setToolTip(QApplication::translate("FramePaletteDialog", "Use current settings and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        okButton->setText(QApplication::translate("FramePaletteDialog", "&Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FramePaletteDialog: public Ui_FramePaletteDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRAMEPALETTEDIALOG_H
