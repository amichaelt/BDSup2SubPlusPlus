/********************************************************************************
** Form generated from reading UI file 'editdialog.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITDIALOG_H
#define UI_EDITDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
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
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <editpane.h>

QT_BEGIN_NAMESPACE

class Ui_EditDialog
{
public:
    QGridLayout *gridLayout_6;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QLabel *infoLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *storePreviousButton;
    QPushButton *storeNextButton;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QFrame *frame_2;
    QGridLayout *gridLayout;
    EditPane *subtitleImage;
    QSlider *verticalSlider;
    QSlider *horizontalSlider;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QFormLayout *formLayout;
    QLabel *startTimeLabel;
    QLineEdit *startTimeLineEdit;
    QSpacerItem *verticalSpacer;
    QFormLayout *formLayout_2;
    QLabel *endTimeLabel;
    QLineEdit *endTimeLineEdit;
    QFormLayout *formLayout_3;
    QLabel *durationLabel;
    QLineEdit *durationLineEdit;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *minButton;
    QPushButton *maxButton;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout_6;
    QFormLayout *formLayout_4;
    QLabel *xOffsetLabel;
    QLineEdit *xOffsetLineEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *centerButton;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_7;
    QFormLayout *formLayout_5;
    QLabel *yOffsetLabel;
    QLineEdit *yOffsetLineEdit;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *topButton;
    QPushButton *bottomButton;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QCheckBox *excludeCheckBox;
    QCheckBox *forcedCaptionCheckBox;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_4;
    QPushButton *addErasePatchButton;
    QPushButton *undoErasePatchButton;
    QPushButton *undoAllErasePatchesButton;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *saveButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *okButton;

    void setupUi(QDialog *EditDialog)
    {
        if (EditDialog->objectName().isEmpty())
            EditDialog->setObjectName(QString::fromUtf8("EditDialog"));
        EditDialog->resize(702, 570);
        EditDialog->setMinimumSize(QSize(702, 570));
        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        EditDialog->setFont(font);
        EditDialog->setSizeGripEnabled(false);
        EditDialog->setModal(true);
        gridLayout_6 = new QGridLayout(EditDialog);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout_6->setHorizontalSpacing(6);
        gridLayout_6->setVerticalSpacing(3);
        gridLayout_6->setContentsMargins(2, 0, 2, 10);
        frame = new QFrame(EditDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(0, 32));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 0, 15, 0);
        infoLabel = new QLabel(frame);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
        infoLabel->setMinimumSize(QSize(110, 16));
        infoLabel->setFont(font);

        horizontalLayout->addWidget(infoLabel);

        horizontalSpacer = new QSpacerItem(385, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        storePreviousButton = new QPushButton(frame);
        storePreviousButton->setObjectName(QString::fromUtf8("storePreviousButton"));
        storePreviousButton->setMinimumSize(QSize(47, 21));
        storePreviousButton->setMaximumSize(QSize(0, 21));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Tahoma"));
        font1.setBold(true);
        font1.setWeight(75);
        storePreviousButton->setFont(font1);
        storePreviousButton->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout->addWidget(storePreviousButton);

        storeNextButton = new QPushButton(frame);
        storeNextButton->setObjectName(QString::fromUtf8("storeNextButton"));
        storeNextButton->setMinimumSize(QSize(47, 21));
        storeNextButton->setMaximumSize(QSize(47, 21));
        storeNextButton->setFont(font1);
        storeNextButton->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout->addWidget(storeNextButton);

        previousButton = new QPushButton(frame);
        previousButton->setObjectName(QString::fromUtf8("previousButton"));
        previousButton->setMinimumSize(QSize(47, 21));
        previousButton->setMaximumSize(QSize(47, 21));
        previousButton->setFont(font1);

        horizontalLayout->addWidget(previousButton);

        nextButton = new QPushButton(frame);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));
        nextButton->setMinimumSize(QSize(47, 21));
        nextButton->setMaximumSize(QSize(47, 21));
        nextButton->setFont(font1);

        horizontalLayout->addWidget(nextButton);


        gridLayout_6->addWidget(frame, 0, 0, 1, 2);

        frame_2 = new QFrame(EditDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMinimumSize(QSize(0, 0));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_2);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(2, 0, 2, 0);
        subtitleImage = new EditPane(frame_2);
        subtitleImage->setObjectName(QString::fromUtf8("subtitleImage"));
        subtitleImage->setMinimumSize(QSize(0, 310));

        gridLayout->addWidget(subtitleImage, 0, 0, 1, 1);

        verticalSlider = new QSlider(frame_2);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        verticalSlider->setMaximum(100);
        verticalSlider->setValue(50);

        gridLayout->addWidget(verticalSlider, 0, 1, 1, 1);

        horizontalSlider = new QSlider(frame_2);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setMaximum(100);
        horizontalSlider->setValue(50);
        horizontalSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider, 1, 0, 1, 1);


        gridLayout_6->addWidget(frame_2, 1, 0, 1, 2);

        groupBox = new QGroupBox(EditDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 0));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(6);
        gridLayout_3->setVerticalSpacing(0);
        gridLayout_3->setContentsMargins(-1, 6, -1, 6);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setHorizontalSpacing(0);
        formLayout->setVerticalSpacing(0);
        formLayout->setContentsMargins(0, -1, 0, -1);
        startTimeLabel = new QLabel(groupBox);
        startTimeLabel->setObjectName(QString::fromUtf8("startTimeLabel"));
        startTimeLabel->setMinimumSize(QSize(80, 0));

        formLayout->setWidget(0, QFormLayout::LabelRole, startTimeLabel);

        startTimeLineEdit = new QLineEdit(groupBox);
        startTimeLineEdit->setObjectName(QString::fromUtf8("startTimeLineEdit"));
        startTimeLineEdit->setMinimumSize(QSize(80, 0));
        startTimeLineEdit->setMaximumSize(QSize(80, 16777215));

        formLayout->setWidget(0, QFormLayout::FieldRole, startTimeLineEdit);


        gridLayout_3->addLayout(formLayout, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(72, 53, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 0, 1, 2, 1);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setHorizontalSpacing(0);
        formLayout_2->setVerticalSpacing(0);
        endTimeLabel = new QLabel(groupBox);
        endTimeLabel->setObjectName(QString::fromUtf8("endTimeLabel"));
        endTimeLabel->setMinimumSize(QSize(80, 0));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, endTimeLabel);

        endTimeLineEdit = new QLineEdit(groupBox);
        endTimeLineEdit->setObjectName(QString::fromUtf8("endTimeLineEdit"));
        endTimeLineEdit->setMinimumSize(QSize(80, 0));
        endTimeLineEdit->setMaximumSize(QSize(80, 16777215));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, endTimeLineEdit);


        gridLayout_3->addLayout(formLayout_2, 1, 0, 1, 1);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setHorizontalSpacing(0);
        formLayout_3->setVerticalSpacing(0);
        durationLabel = new QLabel(groupBox);
        durationLabel->setObjectName(QString::fromUtf8("durationLabel"));
        durationLabel->setMinimumSize(QSize(80, 0));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, durationLabel);

        durationLineEdit = new QLineEdit(groupBox);
        durationLineEdit->setObjectName(QString::fromUtf8("durationLineEdit"));
        durationLineEdit->setMinimumSize(QSize(80, 0));
        durationLineEdit->setMaximumSize(QSize(80, 16777215));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, durationLineEdit);


        gridLayout_3->addLayout(formLayout_3, 2, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        minButton = new QPushButton(groupBox);
        minButton->setObjectName(QString::fromUtf8("minButton"));
        minButton->setMinimumSize(QSize(65, 0));
        minButton->setAutoDefault(false);

        horizontalLayout_5->addWidget(minButton);

        maxButton = new QPushButton(groupBox);
        maxButton->setObjectName(QString::fromUtf8("maxButton"));
        maxButton->setMinimumSize(QSize(65, 0));
        maxButton->setAutoDefault(false);

        horizontalLayout_5->addWidget(maxButton);

        horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 1);
        horizontalLayout_5->setStretch(2, 4);

        gridLayout_3->addLayout(horizontalLayout_5, 2, 1, 1, 1);


        gridLayout_6->addWidget(groupBox, 2, 0, 1, 1);

        groupBox_3 = new QGroupBox(EditDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(0, 0));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        xOffsetLabel = new QLabel(groupBox_3);
        xOffsetLabel->setObjectName(QString::fromUtf8("xOffsetLabel"));
        xOffsetLabel->setMinimumSize(QSize(80, 0));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, xOffsetLabel);

        xOffsetLineEdit = new QLineEdit(groupBox_3);
        xOffsetLineEdit->setObjectName(QString::fromUtf8("xOffsetLineEdit"));
        xOffsetLineEdit->setMinimumSize(QSize(80, 0));
        xOffsetLineEdit->setMaximumSize(QSize(80, 16777215));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, xOffsetLineEdit);


        horizontalLayout_6->addLayout(formLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        centerButton = new QPushButton(groupBox_3);
        centerButton->setObjectName(QString::fromUtf8("centerButton"));
        centerButton->setMinimumSize(QSize(65, 0));
        centerButton->setAutoDefault(false);

        horizontalLayout_2->addWidget(centerButton);

        horizontalSpacer_6 = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 2);

        horizontalLayout_6->addLayout(horizontalLayout_2);

        horizontalLayout_6->setStretch(0, 2);
        horizontalLayout_6->setStretch(1, 2);

        gridLayout_5->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        formLayout_5 = new QFormLayout();
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        yOffsetLabel = new QLabel(groupBox_3);
        yOffsetLabel->setObjectName(QString::fromUtf8("yOffsetLabel"));
        yOffsetLabel->setMinimumSize(QSize(80, 0));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, yOffsetLabel);

        yOffsetLineEdit = new QLineEdit(groupBox_3);
        yOffsetLineEdit->setObjectName(QString::fromUtf8("yOffsetLineEdit"));
        yOffsetLineEdit->setMinimumSize(QSize(80, 0));
        yOffsetLineEdit->setMaximumSize(QSize(80, 16777215));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, yOffsetLineEdit);


        horizontalLayout_7->addLayout(formLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        topButton = new QPushButton(groupBox_3);
        topButton->setObjectName(QString::fromUtf8("topButton"));
        topButton->setMinimumSize(QSize(65, 0));
        topButton->setMaximumSize(QSize(66, 16777215));
        topButton->setAutoDefault(false);

        horizontalLayout_3->addWidget(topButton);

        bottomButton = new QPushButton(groupBox_3);
        bottomButton->setObjectName(QString::fromUtf8("bottomButton"));
        bottomButton->setMinimumSize(QSize(65, 0));
        bottomButton->setAutoDefault(false);

        horizontalLayout_3->addWidget(bottomButton);

        horizontalSpacer_7 = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_7);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 1);
        horizontalLayout_3->setStretch(2, 1);

        horizontalLayout_7->addLayout(horizontalLayout_3);

        horizontalLayout_7->setStretch(0, 2);
        horizontalLayout_7->setStretch(1, 2);

        gridLayout_5->addLayout(horizontalLayout_7, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 2, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_3, 2, 1, 1, 1);

        groupBox_2 = new QGroupBox(EditDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(0, 55));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(-1, 0, -1, 0);
        excludeCheckBox = new QCheckBox(groupBox_2);
        excludeCheckBox->setObjectName(QString::fromUtf8("excludeCheckBox"));

        gridLayout_2->addWidget(excludeCheckBox, 1, 0, 1, 1);

        forcedCaptionCheckBox = new QCheckBox(groupBox_2);
        forcedCaptionCheckBox->setObjectName(QString::fromUtf8("forcedCaptionCheckBox"));

        gridLayout_2->addWidget(forcedCaptionCheckBox, 0, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_2, 3, 0, 1, 1);

        groupBox_4 = new QGroupBox(EditDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setMinimumSize(QSize(0, 55));
        gridLayout_4 = new QGridLayout(groupBox_4);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        addErasePatchButton = new QPushButton(groupBox_4);
        addErasePatchButton->setObjectName(QString::fromUtf8("addErasePatchButton"));
        addErasePatchButton->setEnabled(false);

        gridLayout_4->addWidget(addErasePatchButton, 0, 0, 1, 1);

        undoErasePatchButton = new QPushButton(groupBox_4);
        undoErasePatchButton->setObjectName(QString::fromUtf8("undoErasePatchButton"));
        undoErasePatchButton->setEnabled(false);

        gridLayout_4->addWidget(undoErasePatchButton, 0, 1, 1, 1);

        undoAllErasePatchesButton = new QPushButton(groupBox_4);
        undoAllErasePatchesButton->setObjectName(QString::fromUtf8("undoAllErasePatchesButton"));
        undoAllErasePatchesButton->setEnabled(false);

        gridLayout_4->addWidget(undoAllErasePatchesButton, 0, 2, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(100, 19, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_4, 0, 3, 1, 1);


        gridLayout_6->addWidget(groupBox_4, 3, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(5, -1, 5, -1);
        cancelButton = new QPushButton(EditDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMinimumSize(QSize(65, 0));
        cancelButton->setAutoDefault(false);

        horizontalLayout_4->addWidget(cancelButton);

        horizontalSpacer_2 = new QSpacerItem(220, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        saveButton = new QPushButton(EditDialog);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));
        saveButton->setMinimumSize(QSize(100, 0));
        saveButton->setAutoDefault(false);

        horizontalLayout_4->addWidget(saveButton);

        horizontalSpacer_3 = new QSpacerItem(220, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        okButton = new QPushButton(EditDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setMinimumSize(QSize(65, 0));
        okButton->setAutoDefault(false);
        okButton->setDefault(true);

        horizontalLayout_4->addWidget(okButton);


        gridLayout_6->addLayout(horizontalLayout_4, 4, 0, 1, 2);

        gridLayout_6->setRowStretch(1, 2);
        QWidget::setTabOrder(okButton, storePreviousButton);
        QWidget::setTabOrder(storePreviousButton, storeNextButton);
        QWidget::setTabOrder(storeNextButton, previousButton);
        QWidget::setTabOrder(previousButton, nextButton);
        QWidget::setTabOrder(nextButton, verticalSlider);
        QWidget::setTabOrder(verticalSlider, horizontalSlider);
        QWidget::setTabOrder(horizontalSlider, startTimeLineEdit);
        QWidget::setTabOrder(startTimeLineEdit, endTimeLineEdit);
        QWidget::setTabOrder(endTimeLineEdit, durationLineEdit);
        QWidget::setTabOrder(durationLineEdit, minButton);
        QWidget::setTabOrder(minButton, maxButton);
        QWidget::setTabOrder(maxButton, xOffsetLineEdit);
        QWidget::setTabOrder(xOffsetLineEdit, centerButton);
        QWidget::setTabOrder(centerButton, yOffsetLineEdit);
        QWidget::setTabOrder(yOffsetLineEdit, topButton);
        QWidget::setTabOrder(topButton, bottomButton);
        QWidget::setTabOrder(bottomButton, forcedCaptionCheckBox);
        QWidget::setTabOrder(forcedCaptionCheckBox, excludeCheckBox);
        QWidget::setTabOrder(excludeCheckBox, addErasePatchButton);
        QWidget::setTabOrder(addErasePatchButton, undoErasePatchButton);
        QWidget::setTabOrder(undoErasePatchButton, undoAllErasePatchesButton);
        QWidget::setTabOrder(undoAllErasePatchesButton, cancelButton);
        QWidget::setTabOrder(cancelButton, saveButton);

        retranslateUi(EditDialog);

        QMetaObject::connectSlotsByName(EditDialog);
    } // setupUi

    void retranslateUi(QDialog *EditDialog)
    {
        EditDialog->setWindowTitle(QString());
        infoLabel->setText(QApplication::translate("EditDialog", "9999 of 9999", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        storePreviousButton->setToolTip(QApplication::translate("EditDialog", "Store changes and skip to previous frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        storePreviousButton->setText(QApplication::translate("EditDialog", "<", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        storeNextButton->setToolTip(QApplication::translate("EditDialog", "Store changes and skip to next frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        storeNextButton->setText(QApplication::translate("EditDialog", ">", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        previousButton->setToolTip(QApplication::translate("EditDialog", "Lose changes and skip to previous frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        previousButton->setText(QApplication::translate("EditDialog", "<", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        nextButton->setToolTip(QApplication::translate("EditDialog", "Lose changes and skip to next frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        nextButton->setText(QApplication::translate("EditDialog", ">", 0, QApplication::UnicodeUTF8));
        subtitleImage->setText(QString());
#ifndef QT_NO_TOOLTIP
        verticalSlider->setToolTip(QApplication::translate("EditDialog", "Move subtitle vertically", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        horizontalSlider->setToolTip(QApplication::translate("EditDialog", "Move subtitle horizontally", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox->setTitle(QApplication::translate("EditDialog", "Times", 0, QApplication::UnicodeUTF8));
        startTimeLabel->setText(QApplication::translate("EditDialog", "Start Time", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        startTimeLineEdit->setToolTip(QApplication::translate("EditDialog", "Set start time of subtitle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        endTimeLabel->setText(QApplication::translate("EditDialog", "End Time", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        endTimeLineEdit->setToolTip(QApplication::translate("EditDialog", "Set end time of subtitle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        durationLabel->setText(QApplication::translate("EditDialog", "Duration (ms)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        durationLineEdit->setToolTip(QApplication::translate("EditDialog", "Set display duration of subtitle in milliseconds", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        minButton->setToolTip(QApplication::translate("EditDialog", "Set minimum duration", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        minButton->setText(QApplication::translate("EditDialog", "Mi&n", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        maxButton->setToolTip(QApplication::translate("EditDialog", "Set maximum duration", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        maxButton->setText(QApplication::translate("EditDialog", "&Max", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("EditDialog", "Position", 0, QApplication::UnicodeUTF8));
        xOffsetLabel->setText(QApplication::translate("EditDialog", "X Offset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        xOffsetLineEdit->setToolTip(QApplication::translate("EditDialog", "Set X coordinate of upper left corner of subtitle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        centerButton->setToolTip(QApplication::translate("EditDialog", "Center subpicture horizontally", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        centerButton->setText(QApplication::translate("EditDialog", "&Center", 0, QApplication::UnicodeUTF8));
        yOffsetLabel->setText(QApplication::translate("EditDialog", "Y Offset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        yOffsetLineEdit->setToolTip(QApplication::translate("EditDialog", "Set Y coordinate of upper left corner of subtitle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        topButton->setToolTip(QApplication::translate("EditDialog", "Move to upper cinemascope bar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        topButton->setText(QApplication::translate("EditDialog", "&Top", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        bottomButton->setToolTip(QApplication::translate("EditDialog", "Move to lower cinemascope bar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        bottomButton->setText(QApplication::translate("EditDialog", "&Bottom", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("EditDialog", "Flags", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        excludeCheckBox->setToolTip(QApplication::translate("EditDialog", "Exclude this subtitle from export", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        excludeCheckBox->setText(QApplication::translate("EditDialog", "E&xclude from export", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        forcedCaptionCheckBox->setToolTip(QApplication::translate("EditDialog", "Force display of this subtitle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        forcedCaptionCheckBox->setText(QApplication::translate("EditDialog", "&Forced Caption", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("EditDialog", "Erase Patches", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addErasePatchButton->setToolTip(QApplication::translate("EditDialog", "Add erase patch to make the selected area transparent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addErasePatchButton->setText(QApplication::translate("EditDialog", "&Erase", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        undoErasePatchButton->setToolTip(QApplication::translate("EditDialog", "Remove one erase patch from the stack (undo one delete step)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        undoErasePatchButton->setText(QApplication::translate("EditDialog", "&Undo Erase", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        undoAllErasePatchesButton->setToolTip(QApplication::translate("EditDialog", "Remove all erase patches from the stack (undo all delete steps)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        undoAllErasePatchesButton->setText(QApplication::translate("EditDialog", "Undo &All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("EditDialog", "Lose changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("EditDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        saveButton->setToolTip(QApplication::translate("EditDialog", "Save changes and continue editing", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        saveButton->setText(QApplication::translate("EditDialog", "&Save Changes", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        okButton->setToolTip(QApplication::translate("EditDialog", "Save changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        okButton->setText(QApplication::translate("EditDialog", "&Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EditDialog: public Ui_EditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITDIALOG_H
