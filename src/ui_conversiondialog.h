/********************************************************************************
** Form generated from reading UI file 'conversiondialog.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONVERSIONDIALOG_H
#define UI_CONVERSIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConversionDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *resolutionGroupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *convertResolutionCheckBox;
    QFormLayout *formLayout_6;
    QLabel *resolutionLabel;
    QComboBox *resolutionComboBox;
    QGroupBox *moveGroupBox;
    QGridLayout *gridLayout_2;
    QCheckBox *applyMoveAllSettingsCheckBox;
    QGroupBox *framerateGroupBox;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *changeFrameRateCheckBox;
    QFormLayout *formLayout_2;
    QLabel *sourceFramerateLabel;
    QComboBox *sourceFramerateComboBox;
    QFormLayout *formLayout_3;
    QLabel *targetFramerateLabel;
    QComboBox *targetFramerateComboBox;
    QGroupBox *timesGroupBox;
    QVBoxLayout *verticalLayout_3;
    QFormLayout *formLayout_5;
    QLabel *delayLabel;
    QLineEdit *delayLineEdit;
    QCheckBox *fixTooShortFramesCheckBox;
    QFormLayout *formLayout_4;
    QLabel *minTimeLabel;
    QLineEdit *minTimeLineEdit;
    QGroupBox *forcedFlagsGroupBox;
    QLabel *forceAllLabel;
    QComboBox *forceFlagsComboBox;
    QGroupBox *defaultsGroupBox;
    QPushButton *storeButton;
    QPushButton *restoreButton;
    QPushButton *resetButton;
    QFrame *buttonsPanel;
    QHBoxLayout *horizontalLayout;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QGroupBox *scaleGroupBox;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QCheckBox *scaleCheckBox;
    QFormLayout *formLayout;
    QLabel *scaleXLabel;
    QLineEdit *scaleXLineEdit;
    QFormLayout *formLayout_8;
    QLineEdit *scaleYLineEdit;
    QLabel *scaleYLabel;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QDialog *ConversionDialog)
    {
        if (ConversionDialog->objectName().isEmpty())
            ConversionDialog->setObjectName(QString::fromUtf8("ConversionDialog"));
        ConversionDialog->resize(500, 350);
        ConversionDialog->setMinimumSize(QSize(500, 350));
        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        font.setPointSize(8);
        ConversionDialog->setFont(font);
        ConversionDialog->setModal(true);
        gridLayout = new QGridLayout(ConversionDialog);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setVerticalSpacing(0);
        resolutionGroupBox = new QGroupBox(ConversionDialog);
        resolutionGroupBox->setObjectName(QString::fromUtf8("resolutionGroupBox"));
        resolutionGroupBox->setMinimumSize(QSize(240, 80));
        verticalLayout = new QVBoxLayout(resolutionGroupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        convertResolutionCheckBox = new QCheckBox(resolutionGroupBox);
        convertResolutionCheckBox->setObjectName(QString::fromUtf8("convertResolutionCheckBox"));

        verticalLayout->addWidget(convertResolutionCheckBox);

        formLayout_6 = new QFormLayout();
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        resolutionLabel = new QLabel(resolutionGroupBox);
        resolutionLabel->setObjectName(QString::fromUtf8("resolutionLabel"));
        resolutionLabel->setMinimumSize(QSize(70, 20));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, resolutionLabel);

        resolutionComboBox = new QComboBox(resolutionGroupBox);
        resolutionComboBox->setObjectName(QString::fromUtf8("resolutionComboBox"));
        resolutionComboBox->setEnabled(false);
        resolutionComboBox->setMinimumSize(QSize(150, 20));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, resolutionComboBox);


        verticalLayout->addLayout(formLayout_6);


        gridLayout->addWidget(resolutionGroupBox, 0, 0, 1, 1);

        moveGroupBox = new QGroupBox(ConversionDialog);
        moveGroupBox->setObjectName(QString::fromUtf8("moveGroupBox"));
        moveGroupBox->setMinimumSize(QSize(240, 50));
        gridLayout_2 = new QGridLayout(moveGroupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        applyMoveAllSettingsCheckBox = new QCheckBox(moveGroupBox);
        applyMoveAllSettingsCheckBox->setObjectName(QString::fromUtf8("applyMoveAllSettingsCheckBox"));
        applyMoveAllSettingsCheckBox->setEnabled(false);

        gridLayout_2->addWidget(applyMoveAllSettingsCheckBox, 0, 0, 1, 1);


        gridLayout->addWidget(moveGroupBox, 1, 0, 1, 1);

        framerateGroupBox = new QGroupBox(ConversionDialog);
        framerateGroupBox->setObjectName(QString::fromUtf8("framerateGroupBox"));
        framerateGroupBox->setMinimumSize(QSize(240, 110));
        verticalLayout_4 = new QVBoxLayout(framerateGroupBox);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        changeFrameRateCheckBox = new QCheckBox(framerateGroupBox);
        changeFrameRateCheckBox->setObjectName(QString::fromUtf8("changeFrameRateCheckBox"));

        verticalLayout_4->addWidget(changeFrameRateCheckBox);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        sourceFramerateLabel = new QLabel(framerateGroupBox);
        sourceFramerateLabel->setObjectName(QString::fromUtf8("sourceFramerateLabel"));
        sourceFramerateLabel->setMinimumSize(QSize(70, 20));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, sourceFramerateLabel);

        sourceFramerateComboBox = new QComboBox(framerateGroupBox);
        sourceFramerateComboBox->setObjectName(QString::fromUtf8("sourceFramerateComboBox"));
        sourceFramerateComboBox->setEnabled(false);
        sourceFramerateComboBox->setMinimumSize(QSize(150, 20));
        sourceFramerateComboBox->setEditable(true);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, sourceFramerateComboBox);


        verticalLayout_4->addLayout(formLayout_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        targetFramerateLabel = new QLabel(framerateGroupBox);
        targetFramerateLabel->setObjectName(QString::fromUtf8("targetFramerateLabel"));
        targetFramerateLabel->setMinimumSize(QSize(70, 20));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, targetFramerateLabel);

        targetFramerateComboBox = new QComboBox(framerateGroupBox);
        targetFramerateComboBox->setObjectName(QString::fromUtf8("targetFramerateComboBox"));
        targetFramerateComboBox->setMinimumSize(QSize(150, 20));
        targetFramerateComboBox->setEditable(true);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, targetFramerateComboBox);


        verticalLayout_4->addLayout(formLayout_3);


        gridLayout->addWidget(framerateGroupBox, 2, 0, 1, 1);

        timesGroupBox = new QGroupBox(ConversionDialog);
        timesGroupBox->setObjectName(QString::fromUtf8("timesGroupBox"));
        timesGroupBox->setMinimumSize(QSize(240, 105));
        verticalLayout_3 = new QVBoxLayout(timesGroupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        formLayout_5 = new QFormLayout();
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        delayLabel = new QLabel(timesGroupBox);
        delayLabel->setObjectName(QString::fromUtf8("delayLabel"));
        delayLabel->setMinimumSize(QSize(70, 20));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, delayLabel);

        delayLineEdit = new QLineEdit(timesGroupBox);
        delayLineEdit->setObjectName(QString::fromUtf8("delayLineEdit"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, delayLineEdit);


        verticalLayout_3->addLayout(formLayout_5);

        fixTooShortFramesCheckBox = new QCheckBox(timesGroupBox);
        fixTooShortFramesCheckBox->setObjectName(QString::fromUtf8("fixTooShortFramesCheckBox"));

        verticalLayout_3->addWidget(fixTooShortFramesCheckBox);

        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        minTimeLabel = new QLabel(timesGroupBox);
        minTimeLabel->setObjectName(QString::fromUtf8("minTimeLabel"));
        minTimeLabel->setMinimumSize(QSize(70, 20));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, minTimeLabel);

        minTimeLineEdit = new QLineEdit(timesGroupBox);
        minTimeLineEdit->setObjectName(QString::fromUtf8("minTimeLineEdit"));
        minTimeLineEdit->setEnabled(false);

        formLayout_4->setWidget(0, QFormLayout::FieldRole, minTimeLineEdit);


        verticalLayout_3->addLayout(formLayout_4);


        gridLayout->addWidget(timesGroupBox, 2, 1, 1, 1);

        forcedFlagsGroupBox = new QGroupBox(ConversionDialog);
        forcedFlagsGroupBox->setObjectName(QString::fromUtf8("forcedFlagsGroupBox"));
        forcedFlagsGroupBox->setMinimumSize(QSize(240, 50));
        forceAllLabel = new QLabel(forcedFlagsGroupBox);
        forceAllLabel->setObjectName(QString::fromUtf8("forceAllLabel"));
        forceAllLabel->setGeometry(QRect(10, 23, 70, 20));
        forceAllLabel->setMinimumSize(QSize(70, 20));
        forceFlagsComboBox = new QComboBox(forcedFlagsGroupBox);
        forceFlagsComboBox->setObjectName(QString::fromUtf8("forceFlagsComboBox"));
        forceFlagsComboBox->setGeometry(QRect(86, 23, 150, 20));
        forceFlagsComboBox->setMinimumSize(QSize(150, 20));

        gridLayout->addWidget(forcedFlagsGroupBox, 3, 0, 1, 1);

        defaultsGroupBox = new QGroupBox(ConversionDialog);
        defaultsGroupBox->setObjectName(QString::fromUtf8("defaultsGroupBox"));
        defaultsGroupBox->setMinimumSize(QSize(240, 55));
        storeButton = new QPushButton(defaultsGroupBox);
        storeButton->setObjectName(QString::fromUtf8("storeButton"));
        storeButton->setGeometry(QRect(10, 23, 70, 22));
        storeButton->setAutoDefault(false);
        restoreButton = new QPushButton(defaultsGroupBox);
        restoreButton->setObjectName(QString::fromUtf8("restoreButton"));
        restoreButton->setGeometry(QRect(86, 23, 69, 22));
        restoreButton->setAutoDefault(false);
        resetButton = new QPushButton(defaultsGroupBox);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setGeometry(QRect(161, 23, 69, 22));
        resetButton->setAutoDefault(false);

        gridLayout->addWidget(defaultsGroupBox, 3, 1, 1, 1);

        buttonsPanel = new QFrame(ConversionDialog);
        buttonsPanel->setObjectName(QString::fromUtf8("buttonsPanel"));
        buttonsPanel->setFrameShape(QFrame::StyledPanel);
        buttonsPanel->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(buttonsPanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(9, 0, -1, 0);
        cancelButton = new QPushButton(buttonsPanel);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setAutoDefault(false);

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer = new QSpacerItem(327, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(buttonsPanel);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setAutoDefault(false);
        okButton->setDefault(true);

        horizontalLayout->addWidget(okButton);


        gridLayout->addWidget(buttonsPanel, 4, 0, 1, 2);

        scaleGroupBox = new QGroupBox(ConversionDialog);
        scaleGroupBox->setObjectName(QString::fromUtf8("scaleGroupBox"));
        scaleGroupBox->setMinimumSize(QSize(240, 105));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Tahoma"));
        scaleGroupBox->setFont(font1);
        verticalLayout_2 = new QVBoxLayout(scaleGroupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        scaleCheckBox = new QCheckBox(scaleGroupBox);
        scaleCheckBox->setObjectName(QString::fromUtf8("scaleCheckBox"));

        verticalLayout_2->addWidget(scaleCheckBox);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setVerticalSpacing(0);
        scaleXLabel = new QLabel(scaleGroupBox);
        scaleXLabel->setObjectName(QString::fromUtf8("scaleXLabel"));
        scaleXLabel->setMinimumSize(QSize(70, 20));

        formLayout->setWidget(0, QFormLayout::LabelRole, scaleXLabel);

        scaleXLineEdit = new QLineEdit(scaleGroupBox);
        scaleXLineEdit->setObjectName(QString::fromUtf8("scaleXLineEdit"));
        scaleXLineEdit->setEnabled(false);

        formLayout->setWidget(0, QFormLayout::FieldRole, scaleXLineEdit);


        verticalLayout_2->addLayout(formLayout);

        formLayout_8 = new QFormLayout();
        formLayout_8->setObjectName(QString::fromUtf8("formLayout_8"));
        formLayout_8->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout_8->setVerticalSpacing(0);
        scaleYLineEdit = new QLineEdit(scaleGroupBox);
        scaleYLineEdit->setObjectName(QString::fromUtf8("scaleYLineEdit"));
        scaleYLineEdit->setEnabled(false);

        formLayout_8->setWidget(0, QFormLayout::FieldRole, scaleYLineEdit);

        scaleYLabel = new QLabel(scaleGroupBox);
        scaleYLabel->setObjectName(QString::fromUtf8("scaleYLabel"));
        scaleYLabel->setMinimumSize(QSize(70, 20));

        formLayout_8->setWidget(0, QFormLayout::LabelRole, scaleYLabel);


        verticalLayout_2->addLayout(formLayout_8);

        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        gridLayout->addWidget(scaleGroupBox, 0, 1, 2, 1);

        QWidget::setTabOrder(okButton, convertResolutionCheckBox);
        QWidget::setTabOrder(convertResolutionCheckBox, resolutionComboBox);
        QWidget::setTabOrder(resolutionComboBox, applyMoveAllSettingsCheckBox);
        QWidget::setTabOrder(applyMoveAllSettingsCheckBox, changeFrameRateCheckBox);
        QWidget::setTabOrder(changeFrameRateCheckBox, sourceFramerateComboBox);
        QWidget::setTabOrder(sourceFramerateComboBox, targetFramerateComboBox);
        QWidget::setTabOrder(targetFramerateComboBox, forceFlagsComboBox);
        QWidget::setTabOrder(forceFlagsComboBox, scaleCheckBox);
        QWidget::setTabOrder(scaleCheckBox, scaleXLineEdit);
        QWidget::setTabOrder(scaleXLineEdit, scaleYLineEdit);
        QWidget::setTabOrder(scaleYLineEdit, delayLineEdit);
        QWidget::setTabOrder(delayLineEdit, fixTooShortFramesCheckBox);
        QWidget::setTabOrder(fixTooShortFramesCheckBox, minTimeLineEdit);
        QWidget::setTabOrder(minTimeLineEdit, storeButton);
        QWidget::setTabOrder(storeButton, restoreButton);
        QWidget::setTabOrder(restoreButton, resetButton);
        QWidget::setTabOrder(resetButton, cancelButton);

        retranslateUi(ConversionDialog);

        sourceFramerateComboBox->setCurrentIndex(1);
        targetFramerateComboBox->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(ConversionDialog);
    } // setupUi

    void retranslateUi(QDialog *ConversionDialog)
    {
        ConversionDialog->setWindowTitle(QApplication::translate("ConversionDialog", "Conversion Options", 0, QApplication::UnicodeUTF8));
        resolutionGroupBox->setTitle(QApplication::translate("ConversionDialog", "Resolution", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        convertResolutionCheckBox->setToolTip(QApplication::translate("ConversionDialog", "Convert resolution", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        convertResolutionCheckBox->setText(QApplication::translate("ConversionDialog", "Convert &resolution", 0, QApplication::UnicodeUTF8));
        resolutionLabel->setText(QApplication::translate("ConversionDialog", "Resolution", 0, QApplication::UnicodeUTF8));
        resolutionComboBox->clear();
        resolutionComboBox->insertItems(0, QStringList()
         << QApplication::translate("ConversionDialog", "NTSC (720x480)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "PAL (720x576)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "720p (1280x720)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "1080p- (1440x1080)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "1080p (1920x1080)", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        resolutionComboBox->setToolTip(QApplication::translate("ConversionDialog", "Select the target resolution", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveGroupBox->setTitle(QApplication::translate("ConversionDialog", "Move", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        applyMoveAllSettingsCheckBox->setToolTip(QApplication::translate("ConversionDialog", "Apply settings for moving captions", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        applyMoveAllSettingsCheckBox->setText(QApplication::translate("ConversionDialog", "Apply 'move all' settings", 0, QApplication::UnicodeUTF8));
        framerateGroupBox->setTitle(QApplication::translate("ConversionDialog", "Framerate", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        changeFrameRateCheckBox->setToolTip(QApplication::translate("ConversionDialog", "Convert frame rate from FPS Source to FPS target", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        changeFrameRateCheckBox->setText(QApplication::translate("ConversionDialog", "Change &frame rate", 0, QApplication::UnicodeUTF8));
        sourceFramerateLabel->setText(QApplication::translate("ConversionDialog", "FPS Source", 0, QApplication::UnicodeUTF8));
        sourceFramerateComboBox->clear();
        sourceFramerateComboBox->insertItems(0, QStringList()
         << QApplication::translate("ConversionDialog", "23.975", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "23.976", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "24", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "25", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "29.97", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "50", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "59.94", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        sourceFramerateComboBox->setToolTip(QApplication::translate("ConversionDialog", "Set the source frame rate (only needed for frame rate conversion)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        targetFramerateLabel->setText(QApplication::translate("ConversionDialog", "FPS Traget", 0, QApplication::UnicodeUTF8));
        targetFramerateComboBox->clear();
        targetFramerateComboBox->insertItems(0, QStringList()
         << QApplication::translate("ConversionDialog", "23.975", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "23.976", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "24", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "25", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "29.97", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "50", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "59.94", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        targetFramerateComboBox->setToolTip(QApplication::translate("ConversionDialog", "Set the target frame rate", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        timesGroupBox->setTitle(QApplication::translate("ConversionDialog", "Times", 0, QApplication::UnicodeUTF8));
        delayLabel->setText(QApplication::translate("ConversionDialog", "Delay (ms)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        delayLineEdit->setToolTip(QApplication::translate("ConversionDialog", "Set global delay (in milliseconds) added to all timestamps", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        fixTooShortFramesCheckBox->setToolTip(QApplication::translate("ConversionDialog", "Force a minimum display duration of 'Min Time'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        fixTooShortFramesCheckBox->setText(QApplication::translate("ConversionDialog", "Fix too &short frames", 0, QApplication::UnicodeUTF8));
        minTimeLabel->setText(QApplication::translate("ConversionDialog", "Min Time (ms)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        minTimeLineEdit->setToolTip(QApplication::translate("ConversionDialog", "Set minimum display time for a subtitle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        minTimeLineEdit->setText(QApplication::translate("ConversionDialog", "520", 0, QApplication::UnicodeUTF8));
        forcedFlagsGroupBox->setTitle(QApplication::translate("ConversionDialog", "Global forced flags", 0, QApplication::UnicodeUTF8));
        forceAllLabel->setText(QApplication::translate("ConversionDialog", "Force all", 0, QApplication::UnicodeUTF8));
        forceFlagsComboBox->clear();
        forceFlagsComboBox->insertItems(0, QStringList()
         << QApplication::translate("ConversionDialog", "keep", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "set all", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("ConversionDialog", "clear all", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        forceFlagsComboBox->setToolTip(QApplication::translate("ConversionDialog", "Select the target resolution", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        defaultsGroupBox->setTitle(QApplication::translate("ConversionDialog", "Defaults", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        storeButton->setToolTip(QApplication::translate("ConversionDialog", "Store current settings as default", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        storeButton->setText(QApplication::translate("ConversionDialog", "Store", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        restoreButton->setToolTip(QApplication::translate("ConversionDialog", "Restore last default settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        restoreButton->setText(QApplication::translate("ConversionDialog", "Restore", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resetButton->setToolTip(QApplication::translate("ConversionDialog", "Reset defaults", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetButton->setText(QApplication::translate("ConversionDialog", "Reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("ConversionDialog", "Lose all changes and use the default values", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("ConversionDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        okButton->setToolTip(QApplication::translate("ConversionDialog", "Use current values and continue", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        okButton->setText(QApplication::translate("ConversionDialog", "&Ok", 0, QApplication::UnicodeUTF8));
        scaleGroupBox->setTitle(QApplication::translate("ConversionDialog", "Scale", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        scaleCheckBox->setToolTip(QApplication::translate("ConversionDialog", "Allow free scaling of subtitles in X and Y direction", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        scaleCheckBox->setText(QApplication::translate("ConversionDialog", "&Apply free scaling", 0, QApplication::UnicodeUTF8));
        scaleXLabel->setText(QApplication::translate("ConversionDialog", "Scale X", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        scaleXLineEdit->setToolTip(QApplication::translate("ConversionDialog", "Set free scaling factor in X direction", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        scaleXLineEdit->setText(QApplication::translate("ConversionDialog", "1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        scaleYLineEdit->setToolTip(QApplication::translate("ConversionDialog", "Set free scaling factor in Y direction", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        scaleYLineEdit->setText(QApplication::translate("ConversionDialog", "1", 0, QApplication::UnicodeUTF8));
        scaleYLabel->setText(QApplication::translate("ConversionDialog", "Scale Y", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConversionDialog: public Ui_ConversionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONVERSIONDIALOG_H
