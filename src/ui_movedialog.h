/********************************************************************************
** Form generated from reading UI file 'movedialog.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOVEDIALOG_H
#define UI_MOVEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
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
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <editpane.h>

QT_BEGIN_NAMESPACE

class Ui_MoveDialog
{
public:
    QVBoxLayout *verticalLayout_4;
    QFrame *frame;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_6;
    QLabel *infoLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QFrame *frame_2;
    QGridLayout *gridLayout;
    EditPane *subtitleImage;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QRadioButton *keepXPositionRadioButton;
    QRadioButton *moveFromXPositionRadioButton;
    QRadioButton *moveLeftRadioButton;
    QRadioButton *moveRightRadioButton;
    QRadioButton *moveToCenterRadioButton;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *keepYPositionRadioButton;
    QRadioButton *moveFromYPositionRadioButton;
    QRadioButton *moveInsideBoundsRadioButton;
    QRadioButton *moveOutsideBoundsRadioButton;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QFormLayout *formLayout;
    QLabel *xOffsetLabel;
    QLineEdit *xOffsetLineEdit;
    QSpacerItem *horizontalSpacer_3;
    QFormLayout *formLayout_2;
    QLabel *yOffsetLabel;
    QLineEdit *yOffsetLineEdit;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QFormLayout *formLayout_3;
    QLineEdit *aspectRatioLineEdit;
    QLabel *aspectRatioLabel;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *aspectRatio1Button;
    QPushButton *aspectRatio2Button;
    QPushButton *aspectRatio3Button;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QFormLayout *formLayout_4;
    QLabel *cropOffsetYLabel;
    QLineEdit *cropOffsetYLineEdit;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *cropBarsButton;
    QFrame *frame_3;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *moveAllButton;

    void setupUi(QDialog *MoveDialog)
    {
        if (MoveDialog->objectName().isEmpty())
            MoveDialog->setObjectName(QString::fromUtf8("MoveDialog"));
        MoveDialog->resize(392, 520);
        MoveDialog->setMinimumSize(QSize(392, 520));
        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        MoveDialog->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/resources/bdsup2sub.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MoveDialog->setWindowIcon(icon);
        MoveDialog->setModal(true);
        verticalLayout_4 = new QVBoxLayout(MoveDialog);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setSizeConstraint(QLayout::SetFixedSize);
        frame = new QFrame(MoveDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(200, 30));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_4 = new QGridLayout(frame);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(9, -1, 9, -1);
        infoLabel = new QLabel(frame);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
        infoLabel->setMinimumSize(QSize(100, 0));

        horizontalLayout_6->addWidget(infoLabel);

        horizontalSpacer_2 = new QSpacerItem(150, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        previousButton = new QPushButton(frame);
        previousButton->setObjectName(QString::fromUtf8("previousButton"));
        previousButton->setMaximumSize(QSize(55, 18));
        previousButton->setAutoDefault(false);

        horizontalLayout_6->addWidget(previousButton);

        nextButton = new QPushButton(frame);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));
        nextButton->setMaximumSize(QSize(55, 18));
        nextButton->setAutoDefault(false);

        horizontalLayout_6->addWidget(nextButton);


        gridLayout_4->addLayout(horizontalLayout_6, 0, 0, 1, 1);


        verticalLayout_4->addWidget(frame);

        frame_2 = new QFrame(MoveDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMinimumSize(QSize(392, 0));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_2);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        subtitleImage = new EditPane(frame_2);
        subtitleImage->setObjectName(QString::fromUtf8("subtitleImage"));
        subtitleImage->setMinimumSize(QSize(392, 216));

        gridLayout->addWidget(subtitleImage, 0, 0, 1, 1);


        verticalLayout_4->addWidget(frame_2);

        groupBox = new QGroupBox(MoveDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(392, 0));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_3->setContentsMargins(9, 0, 9, 9);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        keepXPositionRadioButton = new QRadioButton(groupBox);
        keepXPositionRadioButton->setObjectName(QString::fromUtf8("keepXPositionRadioButton"));
        keepXPositionRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout->addWidget(keepXPositionRadioButton);

        moveFromXPositionRadioButton = new QRadioButton(groupBox);
        moveFromXPositionRadioButton->setObjectName(QString::fromUtf8("moveFromXPositionRadioButton"));

        verticalLayout->addWidget(moveFromXPositionRadioButton);

        moveLeftRadioButton = new QRadioButton(groupBox);
        moveLeftRadioButton->setObjectName(QString::fromUtf8("moveLeftRadioButton"));
        moveLeftRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout->addWidget(moveLeftRadioButton);

        moveRightRadioButton = new QRadioButton(groupBox);
        moveRightRadioButton->setObjectName(QString::fromUtf8("moveRightRadioButton"));
        moveRightRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout->addWidget(moveRightRadioButton);

        moveToCenterRadioButton = new QRadioButton(groupBox);
        moveToCenterRadioButton->setObjectName(QString::fromUtf8("moveToCenterRadioButton"));
        moveToCenterRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout->addWidget(moveToCenterRadioButton);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        keepYPositionRadioButton = new QRadioButton(groupBox);
        keepYPositionRadioButton->setObjectName(QString::fromUtf8("keepYPositionRadioButton"));
        keepYPositionRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout_2->addWidget(keepYPositionRadioButton);

        moveFromYPositionRadioButton = new QRadioButton(groupBox);
        moveFromYPositionRadioButton->setObjectName(QString::fromUtf8("moveFromYPositionRadioButton"));

        verticalLayout_2->addWidget(moveFromYPositionRadioButton);

        moveInsideBoundsRadioButton = new QRadioButton(groupBox);
        moveInsideBoundsRadioButton->setObjectName(QString::fromUtf8("moveInsideBoundsRadioButton"));
        moveInsideBoundsRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout_2->addWidget(moveInsideBoundsRadioButton);

        moveOutsideBoundsRadioButton = new QRadioButton(groupBox);
        moveOutsideBoundsRadioButton->setObjectName(QString::fromUtf8("moveOutsideBoundsRadioButton"));
        moveOutsideBoundsRadioButton->setMinimumSize(QSize(180, 0));

        verticalLayout_2->addWidget(moveOutsideBoundsRadioButton);

        verticalSpacer = new QSpacerItem(20, 13, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout_3->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setVerticalSpacing(6);
        formLayout->setContentsMargins(-1, 0, -1, 0);
        xOffsetLabel = new QLabel(groupBox);
        xOffsetLabel->setObjectName(QString::fromUtf8("xOffsetLabel"));
        xOffsetLabel->setMinimumSize(QSize(70, 0));

        formLayout->setWidget(0, QFormLayout::LabelRole, xOffsetLabel);

        xOffsetLineEdit = new QLineEdit(groupBox);
        xOffsetLineEdit->setObjectName(QString::fromUtf8("xOffsetLineEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(xOffsetLineEdit->sizePolicy().hasHeightForWidth());
        xOffsetLineEdit->setSizePolicy(sizePolicy);
        xOffsetLineEdit->setMinimumSize(QSize(50, 20));
        xOffsetLineEdit->setMaximumSize(QSize(50, 16777215));

        formLayout->setWidget(0, QFormLayout::FieldRole, xOffsetLineEdit);


        horizontalLayout_2->addLayout(formLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        yOffsetLabel = new QLabel(groupBox);
        yOffsetLabel->setObjectName(QString::fromUtf8("yOffsetLabel"));
        yOffsetLabel->setMinimumSize(QSize(70, 0));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, yOffsetLabel);

        yOffsetLineEdit = new QLineEdit(groupBox);
        yOffsetLineEdit->setObjectName(QString::fromUtf8("yOffsetLineEdit"));
        sizePolicy.setHeightForWidth(yOffsetLineEdit->sizePolicy().hasHeightForWidth());
        yOffsetLineEdit->setSizePolicy(sizePolicy);
        yOffsetLineEdit->setMinimumSize(QSize(50, 20));
        yOffsetLineEdit->setMaximumSize(QSize(50, 16777215));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, yOffsetLineEdit);


        horizontalLayout_2->addLayout(formLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_4->addWidget(groupBox);

        groupBox_2 = new QGroupBox(MoveDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(392, 0));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(9, 0, 9, 9);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(5);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        aspectRatioLineEdit = new QLineEdit(groupBox_2);
        aspectRatioLineEdit->setObjectName(QString::fromUtf8("aspectRatioLineEdit"));
        sizePolicy.setHeightForWidth(aspectRatioLineEdit->sizePolicy().hasHeightForWidth());
        aspectRatioLineEdit->setSizePolicy(sizePolicy);
        aspectRatioLineEdit->setMinimumSize(QSize(50, 0));
        aspectRatioLineEdit->setMaximumSize(QSize(50, 16777215));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, aspectRatioLineEdit);

        aspectRatioLabel = new QLabel(groupBox_2);
        aspectRatioLabel->setObjectName(QString::fromUtf8("aspectRatioLabel"));
        aspectRatioLabel->setMinimumSize(QSize(70, 0));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, aspectRatioLabel);


        horizontalLayout_4->addLayout(formLayout_3);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_4->addWidget(label_6);

        horizontalSpacer_5 = new QSpacerItem(36, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        aspectRatio1Button = new QPushButton(groupBox_2);
        aspectRatio1Button->setObjectName(QString::fromUtf8("aspectRatio1Button"));
        aspectRatio1Button->setMinimumSize(QSize(55, 0));
        aspectRatio1Button->setAutoDefault(false);

        horizontalLayout_4->addWidget(aspectRatio1Button);

        aspectRatio2Button = new QPushButton(groupBox_2);
        aspectRatio2Button->setObjectName(QString::fromUtf8("aspectRatio2Button"));
        aspectRatio2Button->setMinimumSize(QSize(55, 0));
        aspectRatio2Button->setAutoDefault(false);

        horizontalLayout_4->addWidget(aspectRatio2Button);

        aspectRatio3Button = new QPushButton(groupBox_2);
        aspectRatio3Button->setObjectName(QString::fromUtf8("aspectRatio3Button"));
        aspectRatio3Button->setMinimumSize(QSize(55, 0));
        aspectRatio3Button->setAutoDefault(false);

        horizontalLayout_4->addWidget(aspectRatio3Button);


        gridLayout_2->addLayout(horizontalLayout_4, 0, 0, 1, 1);


        verticalLayout_4->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(MoveDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(392, 0));
        gridLayout_3 = new QGridLayout(groupBox_3);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(9, 0, 9, 9);
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        cropOffsetYLabel = new QLabel(groupBox_3);
        cropOffsetYLabel->setObjectName(QString::fromUtf8("cropOffsetYLabel"));
        cropOffsetYLabel->setMinimumSize(QSize(70, 0));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, cropOffsetYLabel);

        cropOffsetYLineEdit = new QLineEdit(groupBox_3);
        cropOffsetYLineEdit->setObjectName(QString::fromUtf8("cropOffsetYLineEdit"));
        sizePolicy.setHeightForWidth(cropOffsetYLineEdit->sizePolicy().hasHeightForWidth());
        cropOffsetYLineEdit->setSizePolicy(sizePolicy);
        cropOffsetYLineEdit->setMinimumSize(QSize(50, 0));
        cropOffsetYLineEdit->setMaximumSize(QSize(50, 16777215));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, cropOffsetYLineEdit);


        horizontalLayout_5->addLayout(formLayout_4);

        horizontalSpacer_6 = new QSpacerItem(185, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        cropBarsButton = new QPushButton(groupBox_3);
        cropBarsButton->setObjectName(QString::fromUtf8("cropBarsButton"));
        cropBarsButton->setAutoDefault(false);

        horizontalLayout_5->addWidget(cropBarsButton);


        gridLayout_3->addLayout(horizontalLayout_5, 0, 0, 1, 1);


        verticalLayout_4->addWidget(groupBox_3);

        frame_3 = new QFrame(MoveDialog);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMinimumSize(QSize(392, 0));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayout_5 = new QGridLayout(frame_3);
        gridLayout_5->setSpacing(0);
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(9, -1, 9, 0);
        cancelButton = new QPushButton(frame_3);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setAutoDefault(false);

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer = new QSpacerItem(212, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        moveAllButton = new QPushButton(frame_3);
        moveAllButton->setObjectName(QString::fromUtf8("moveAllButton"));
        moveAllButton->setAutoDefault(false);
        moveAllButton->setDefault(true);

        horizontalLayout->addWidget(moveAllButton);


        gridLayout_5->addLayout(horizontalLayout, 0, 0, 1, 1);


        verticalLayout_4->addWidget(frame_3);

        QWidget::setTabOrder(moveAllButton, previousButton);
        QWidget::setTabOrder(previousButton, nextButton);
        QWidget::setTabOrder(nextButton, keepXPositionRadioButton);
        QWidget::setTabOrder(keepXPositionRadioButton, moveFromXPositionRadioButton);
        QWidget::setTabOrder(moveFromXPositionRadioButton, moveLeftRadioButton);
        QWidget::setTabOrder(moveLeftRadioButton, moveRightRadioButton);
        QWidget::setTabOrder(moveRightRadioButton, moveToCenterRadioButton);
        QWidget::setTabOrder(moveToCenterRadioButton, keepYPositionRadioButton);
        QWidget::setTabOrder(keepYPositionRadioButton, moveFromYPositionRadioButton);
        QWidget::setTabOrder(moveFromYPositionRadioButton, moveInsideBoundsRadioButton);
        QWidget::setTabOrder(moveInsideBoundsRadioButton, moveOutsideBoundsRadioButton);
        QWidget::setTabOrder(moveOutsideBoundsRadioButton, xOffsetLineEdit);
        QWidget::setTabOrder(xOffsetLineEdit, yOffsetLineEdit);
        QWidget::setTabOrder(yOffsetLineEdit, aspectRatioLineEdit);
        QWidget::setTabOrder(aspectRatioLineEdit, aspectRatio1Button);
        QWidget::setTabOrder(aspectRatio1Button, aspectRatio2Button);
        QWidget::setTabOrder(aspectRatio2Button, aspectRatio3Button);
        QWidget::setTabOrder(aspectRatio3Button, cropOffsetYLineEdit);
        QWidget::setTabOrder(cropOffsetYLineEdit, cropBarsButton);
        QWidget::setTabOrder(cropBarsButton, cancelButton);

        retranslateUi(MoveDialog);

        QMetaObject::connectSlotsByName(MoveDialog);
    } // setupUi

    void retranslateUi(QDialog *MoveDialog)
    {
        MoveDialog->setWindowTitle(QApplication::translate("MoveDialog", "Move all captions", 0, QApplication::UnicodeUTF8));
        infoLabel->setText(QApplication::translate("MoveDialog", "Info", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        previousButton->setToolTip(QApplication::translate("MoveDialog", "Lose changes and skip to previous frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        previousButton->setText(QApplication::translate("MoveDialog", "<", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        nextButton->setToolTip(QApplication::translate("MoveDialog", "Lose changes and skip to next frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        nextButton->setText(QApplication::translate("MoveDialog", ">", 0, QApplication::UnicodeUTF8));
        subtitleImage->setText(QString());
        groupBox->setTitle(QApplication::translate("MoveDialog", "Move", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        keepXPositionRadioButton->setToolTip(QApplication::translate("MoveDialog", "Don't alter current X position", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        keepXPositionRadioButton->setText(QApplication::translate("MoveDialog", "keep &X position", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveFromXPositionRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move from the original X position", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveFromXPositionRadioButton->setText(QApplication::translate("MoveDialog", "move from original X position", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveLeftRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move to the left", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveLeftRadioButton->setText(QApplication::translate("MoveDialog", "move from &left", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveRightRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move to the right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveRightRadioButton->setText(QApplication::translate("MoveDialog", "move from &right", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveToCenterRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move to center", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveToCenterRadioButton->setText(QApplication::translate("MoveDialog", "move from &center", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        keepYPositionRadioButton->setToolTip(QApplication::translate("MoveDialog", "Don't alter current Y position", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        keepYPositionRadioButton->setText(QApplication::translate("MoveDialog", "keep &Y position", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveFromYPositionRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move from the original Y position", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveFromYPositionRadioButton->setText(QApplication::translate("MoveDialog", "move from original Y position", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveInsideBoundsRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move the subtitles inside the inner frame", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveInsideBoundsRadioButton->setText(QApplication::translate("MoveDialog", "move from &inside bounds", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveOutsideBoundsRadioButton->setToolTip(QApplication::translate("MoveDialog", "Move the subtitles outside the inner frame as much as possible", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveOutsideBoundsRadioButton->setText(QApplication::translate("MoveDialog", "move from &outside bounds", 0, QApplication::UnicodeUTF8));
        xOffsetLabel->setText(QApplication::translate("MoveDialog", "Offset X", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        xOffsetLineEdit->setToolTip(QApplication::translate("MoveDialog", "Set offset from left/right border in pixels", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        xOffsetLineEdit->setText(QApplication::translate("MoveDialog", "10", 0, QApplication::UnicodeUTF8));
        yOffsetLabel->setText(QApplication::translate("MoveDialog", "Offset Y", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        yOffsetLineEdit->setToolTip(QApplication::translate("MoveDialog", "Set offset from lower/upper border in pixels", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        yOffsetLineEdit->setText(QApplication::translate("MoveDialog", "10", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MoveDialog", "Screen Ratio", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        aspectRatioLineEdit->setToolTip(QApplication::translate("MoveDialog", "Set inner frame ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        aspectRatioLineEdit->setText(QApplication::translate("MoveDialog", "2.333", 0, QApplication::UnicodeUTF8));
        aspectRatioLabel->setText(QApplication::translate("MoveDialog", "Aspect ratio", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MoveDialog", " : 1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        aspectRatio1Button->setToolTip(QApplication::translate("MoveDialog", "Set inner frame ratio to 21:9", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        aspectRatio1Button->setText(QApplication::translate("MoveDialog", "21:9", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        aspectRatio2Button->setToolTip(QApplication::translate("MoveDialog", "Set inner frame ratio to 2.35:1", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        aspectRatio2Button->setText(QApplication::translate("MoveDialog", "2.35:1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        aspectRatio3Button->setToolTip(QApplication::translate("MoveDialog", "Set inner frame ratio to 2.40:1", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        aspectRatio3Button->setText(QApplication::translate("MoveDialog", "2.40:1", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MoveDialog", "Crop", 0, QApplication::UnicodeUTF8));
        cropOffsetYLabel->setText(QApplication::translate("MoveDialog", "Crop Offset Y", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cropOffsetYLineEdit->setToolTip(QApplication::translate("MoveDialog", "Set number of lines to be cropped from upper and lower border", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        cropBarsButton->setToolTip(QApplication::translate("MoveDialog", "Set crop offsets to cinemascope bars", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cropBarsButton->setText(QApplication::translate("MoveDialog", "&Crop Bars", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cancelButton->setToolTip(QApplication::translate("MoveDialog", "Lose changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cancelButton->setText(QApplication::translate("MoveDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveAllButton->setToolTip(QApplication::translate("MoveDialog", "Save changes and return", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveAllButton->setText(QApplication::translate("MoveDialog", "&Move all", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MoveDialog: public Ui_MoveDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOVEDIALOG_H
