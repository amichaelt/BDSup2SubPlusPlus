/********************************************************************************
** Form generated from reading UI file 'exportdialog.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPORTDIALOG_H
#define UI_EXPORTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ExportDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QFormLayout *formLayout;
    QLabel *fileNameLabel;
    QLineEdit *fileNameLineEdit;
    QPushButton *browseButton;
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout_2;
    QLabel *languageLabel;
    QComboBox *languageComboBox;
    QSpacerItem *horizontalSpacer;
    QCheckBox *exportForcedOnlyCheckBox;
    QCheckBox *exportPGCEditFormatCheckBox;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *saveButton;

    void setupUi(QDialog *ExportDialog)
    {
        if (ExportDialog->objectName().isEmpty())
            ExportDialog->setObjectName(QString::fromUtf8("ExportDialog"));
        ExportDialog->resize(350, 150);
        ExportDialog->setMinimumSize(QSize(350, 150));
        ExportDialog->setModal(true);
        verticalLayout = new QVBoxLayout(ExportDialog);
#ifndef Q_OS_MAC
        verticalLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        verticalLayout->setContentsMargins(9, 9, 9, 9);
#endif
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        fileNameLabel = new QLabel(ExportDialog);
        fileNameLabel->setObjectName(QString::fromUtf8("fileNameLabel"));
        fileNameLabel->setMinimumSize(QSize(60, 0));

        formLayout->setWidget(0, QFormLayout::LabelRole, fileNameLabel);

        fileNameLineEdit = new QLineEdit(ExportDialog);
        fileNameLineEdit->setObjectName(QString::fromUtf8("fileNameLineEdit"));
        fileNameLineEdit->setMinimumSize(QSize(180, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, fileNameLineEdit);


        horizontalLayout_3->addLayout(formLayout);

        browseButton = new QPushButton(ExportDialog);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setAutoDefault(false);

        horizontalLayout_3->addWidget(browseButton);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        languageLabel = new QLabel(ExportDialog);
        languageLabel->setObjectName(QString::fromUtf8("languageLabel"));
        languageLabel->setMinimumSize(QSize(60, 0));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, languageLabel);

        languageComboBox = new QComboBox(ExportDialog);
        languageComboBox->setObjectName(QString::fromUtf8("languageComboBox"));
        languageComboBox->setMinimumSize(QSize(110, 0));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, languageComboBox);


        horizontalLayout->addLayout(formLayout_2);

        horizontalSpacer = new QSpacerItem(128, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        exportForcedOnlyCheckBox = new QCheckBox(ExportDialog);
        exportForcedOnlyCheckBox->setObjectName(QString::fromUtf8("exportForcedOnlyCheckBox"));
        exportForcedOnlyCheckBox->setEnabled(false);

        verticalLayout->addWidget(exportForcedOnlyCheckBox);

        exportPGCEditFormatCheckBox = new QCheckBox(ExportDialog);
        exportPGCEditFormatCheckBox->setObjectName(QString::fromUtf8("exportPGCEditFormatCheckBox"));

        verticalLayout->addWidget(exportPGCEditFormatCheckBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        cancelButton = new QPushButton(ExportDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(cancelButton);

        horizontalSpacer_2 = new QSpacerItem(158, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        saveButton = new QPushButton(ExportDialog);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setAutoDefault(false);
        saveButton->setDefault(true);

        horizontalLayout_2->addWidget(saveButton);


        verticalLayout->addLayout(horizontalLayout_2);

        QWidget::setTabOrder(saveButton, fileNameLineEdit);
        QWidget::setTabOrder(fileNameLineEdit, browseButton);
        QWidget::setTabOrder(browseButton, languageComboBox);
        QWidget::setTabOrder(languageComboBox, exportForcedOnlyCheckBox);
        QWidget::setTabOrder(exportForcedOnlyCheckBox, exportPGCEditFormatCheckBox);
        QWidget::setTabOrder(exportPGCEditFormatCheckBox, cancelButton);

        retranslateUi(ExportDialog);

        QMetaObject::connectSlotsByName(ExportDialog);
    } // setupUi

    void retranslateUi(QDialog *ExportDialog)
    {
        ExportDialog->setWindowTitle(QApplication::translate("ExportDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        fileNameLabel->setText(QApplication::translate("ExportDialog", "Filename", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        fileNameLineEdit->setToolTip(QApplication::translate("ExportDialog", "Set file name for export", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        browseButton->setToolTip(QApplication::translate("ExportDialog", "Open file dialog to select file name for export", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        browseButton->setText(QApplication::translate("ExportDialog", "&Browse", 0, QApplication::UnicodeUTF8));
        languageLabel->setText(QApplication::translate("ExportDialog", "Language", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        languageComboBox->setToolTip(QApplication::translate("ExportDialog", "Set language identifier", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        exportForcedOnlyCheckBox->setToolTip(QApplication::translate("ExportDialog", "Export only subpictures marked as 'forced'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        exportForcedOnlyCheckBox->setText(QApplication::translate("ExportDialog", "                 Export only &forced", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        exportPGCEditFormatCheckBox->setToolTip(QApplication::translate("ExportDialog", "Export palette in PGCEdit text format (RGB, 0..255)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        exportPGCEditFormatCheckBox->setText(QApplication::translate("ExportDialog", "                 Export &palette in PGCEdit text format", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("ExportDialog", "Cancel export and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("ExportDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        saveButton->setToolTip(QApplication::translate("ExportDialog", "Start creation of export stream", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveButton->setText(QApplication::translate("ExportDialog", "&Save", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ExportDialog: public Ui_ExportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPORTDIALOG_H
