/********************************************************************************
** Form generated from reading UI file 'bdsup2sub.ui'
**
** Created: Sat Jan 26 17:56:00 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BDSUP2SUB_H
#define UI_BDSUP2SUB_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <editpane.h>
#include "zoomableimagearea.h"

QT_BEGIN_NAMESPACE

class Ui_BDSup2Sub
{
public:
    QAction *action_Load;
    QAction *action_Save_Export;
    QAction *action_Close;
    QAction *action_Exit;
    QAction *action_Edit_Frame;
    QAction *actionEdit_default_DVD_Palette;
    QAction *actionEdit_imported_DVD_Palette;
    QAction *actionEdit_DVD_Frame_Palette;
    QAction *action_Move_all_captions;
    QAction *action_Reset_crop_offset;
    QAction *action_Conversion_Settings;
    QAction *action_Swap_Cr_Cb;
    QAction *action_Fix_invisible_frames;
    QAction *action_Verbose_Output;
    QAction *action_Help;
    QAction *actionHello;
    QAction *actionAbout_Qt;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QFrame *topFrame;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *subtitleLanguageLayout;
    QLabel *subtitleLanguageLabel;
    QComboBox *subtitleLanguageComboBox;
    QVBoxLayout *subtitleNumberLayout;
    QLabel *subtitleNumberLabel;
    QComboBox *subtitleNumberComboBox;
    QVBoxLayout *alphaThresholdLayout;
    QLabel *alphaThresholdLabel;
    QComboBox *alphaThresholdComboBox;
    QVBoxLayout *medLowThresholdLayout;
    QLabel *medLowThresholdLabel;
    QComboBox *medLowThresholdComboBox;
    QVBoxLayout *hiMedThresholdLayout;
    QLabel *hiMedThresholdLabel;
    QComboBox *hiMedThresholdComboBox;
    QVBoxLayout *outputFormatLayout;
    QLabel *outputFormatLabel;
    QComboBox *outputFormatComboBox;
    QVBoxLayout *paletteLayout;
    QLabel *paletteLabel;
    QComboBox *paletteComboBox;
    QVBoxLayout *filterLayout;
    QLabel *filterLabel;
    QComboBox *filterComboBox;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_8;
    QLabel *sourceInfoLabel;
    QScrollArea *sourceScrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    ZoomableImageArea *sourceImage;
    QFrame *frame_5;
    QVBoxLayout *verticalLayout_9;
    QLabel *targetInfoLabel;
    QScrollArea *targetScrollArea;
    QWidget *scrollAreaWidgetContents_2;
    QGridLayout *gridLayout_2;
    ZoomableImageArea *targetImage;
    QFrame *bottomFrame;
    QHBoxLayout *horizontalLayout_2;
    QPlainTextEdit *consoleOutput;
    EditPane *subtitleImage;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Recent_Files;
    QMenu *menu_Edit;
    QMenu *menu_Settings;
    QMenu *menu_About;

    void setupUi(QMainWindow *BDSup2Sub)
    {
        if (BDSup2Sub->objectName().isEmpty())
            BDSup2Sub->setObjectName(QString::fromUtf8("BDSup2Sub"));
        BDSup2Sub->resize(738, 562);
        BDSup2Sub->setAcceptDrops(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/resources/bdsup2sub.ico"), QSize(), QIcon::Normal, QIcon::Off);
        BDSup2Sub->setWindowIcon(icon);
        action_Load = new QAction(BDSup2Sub);
        action_Load->setObjectName(QString::fromUtf8("action_Load"));
        action_Save_Export = new QAction(BDSup2Sub);
        action_Save_Export->setObjectName(QString::fromUtf8("action_Save_Export"));
        action_Save_Export->setEnabled(false);
        action_Close = new QAction(BDSup2Sub);
        action_Close->setObjectName(QString::fromUtf8("action_Close"));
        action_Close->setEnabled(false);
        action_Exit = new QAction(BDSup2Sub);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        action_Edit_Frame = new QAction(BDSup2Sub);
        action_Edit_Frame->setObjectName(QString::fromUtf8("action_Edit_Frame"));
        action_Edit_Frame->setEnabled(false);
        actionEdit_default_DVD_Palette = new QAction(BDSup2Sub);
        actionEdit_default_DVD_Palette->setObjectName(QString::fromUtf8("actionEdit_default_DVD_Palette"));
        actionEdit_imported_DVD_Palette = new QAction(BDSup2Sub);
        actionEdit_imported_DVD_Palette->setObjectName(QString::fromUtf8("actionEdit_imported_DVD_Palette"));
        actionEdit_imported_DVD_Palette->setEnabled(false);
        actionEdit_DVD_Frame_Palette = new QAction(BDSup2Sub);
        actionEdit_DVD_Frame_Palette->setObjectName(QString::fromUtf8("actionEdit_DVD_Frame_Palette"));
        actionEdit_DVD_Frame_Palette->setEnabled(false);
        action_Move_all_captions = new QAction(BDSup2Sub);
        action_Move_all_captions->setObjectName(QString::fromUtf8("action_Move_all_captions"));
        action_Move_all_captions->setEnabled(false);
        action_Reset_crop_offset = new QAction(BDSup2Sub);
        action_Reset_crop_offset->setObjectName(QString::fromUtf8("action_Reset_crop_offset"));
        action_Conversion_Settings = new QAction(BDSup2Sub);
        action_Conversion_Settings->setObjectName(QString::fromUtf8("action_Conversion_Settings"));
        action_Swap_Cr_Cb = new QAction(BDSup2Sub);
        action_Swap_Cr_Cb->setObjectName(QString::fromUtf8("action_Swap_Cr_Cb"));
        action_Swap_Cr_Cb->setCheckable(true);
        action_Fix_invisible_frames = new QAction(BDSup2Sub);
        action_Fix_invisible_frames->setObjectName(QString::fromUtf8("action_Fix_invisible_frames"));
        action_Fix_invisible_frames->setCheckable(true);
        action_Verbose_Output = new QAction(BDSup2Sub);
        action_Verbose_Output->setObjectName(QString::fromUtf8("action_Verbose_Output"));
        action_Verbose_Output->setCheckable(true);
        action_Help = new QAction(BDSup2Sub);
        action_Help->setObjectName(QString::fromUtf8("action_Help"));
        actionHello = new QAction(BDSup2Sub);
        actionHello->setObjectName(QString::fromUtf8("actionHello"));
        actionAbout_Qt = new QAction(BDSup2Sub);
        actionAbout_Qt->setObjectName(QString::fromUtf8("actionAbout_Qt"));
        centralWidget = new QWidget(BDSup2Sub);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setAcceptDrops(false);
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        topFrame = new QFrame(centralWidget);
        topFrame->setObjectName(QString::fromUtf8("topFrame"));
        topFrame->setAutoFillBackground(true);
        topFrame->setFrameShape(QFrame::Panel);
        topFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(topFrame);
        horizontalLayout->setSpacing(7);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(7, 1, 6, 7);
        subtitleLanguageLayout = new QVBoxLayout();
        subtitleLanguageLayout->setSpacing(0);
        subtitleLanguageLayout->setObjectName(QString::fromUtf8("subtitleLanguageLayout"));
        subtitleLanguageLabel = new QLabel(topFrame);
        subtitleLanguageLabel->setObjectName(QString::fromUtf8("subtitleLanguageLabel"));

        subtitleLanguageLayout->addWidget(subtitleLanguageLabel);

        subtitleLanguageComboBox = new QComboBox(topFrame);
        subtitleLanguageComboBox->setObjectName(QString::fromUtf8("subtitleLanguageComboBox"));
        subtitleLanguageComboBox->setEnabled(false);
        subtitleLanguageComboBox->setEditable(false);

        subtitleLanguageLayout->addWidget(subtitleLanguageComboBox);


        horizontalLayout->addLayout(subtitleLanguageLayout);

        subtitleNumberLayout = new QVBoxLayout();
        subtitleNumberLayout->setSpacing(0);
        subtitleNumberLayout->setObjectName(QString::fromUtf8("subtitleNumberLayout"));
        subtitleNumberLabel = new QLabel(topFrame);
        subtitleNumberLabel->setObjectName(QString::fromUtf8("subtitleNumberLabel"));
        subtitleNumberLabel->setIndent(4);

        subtitleNumberLayout->addWidget(subtitleNumberLabel);

        subtitleNumberComboBox = new QComboBox(topFrame);
        subtitleNumberComboBox->setObjectName(QString::fromUtf8("subtitleNumberComboBox"));
        subtitleNumberComboBox->setEnabled(false);
        subtitleNumberComboBox->setEditable(true);
        subtitleNumberComboBox->setMaxVisibleItems(8);

        subtitleNumberLayout->addWidget(subtitleNumberComboBox);


        horizontalLayout->addLayout(subtitleNumberLayout);

        alphaThresholdLayout = new QVBoxLayout();
        alphaThresholdLayout->setSpacing(0);
        alphaThresholdLayout->setObjectName(QString::fromUtf8("alphaThresholdLayout"));
        alphaThresholdLabel = new QLabel(topFrame);
        alphaThresholdLabel->setObjectName(QString::fromUtf8("alphaThresholdLabel"));

        alphaThresholdLayout->addWidget(alphaThresholdLabel);

        alphaThresholdComboBox = new QComboBox(topFrame);
        alphaThresholdComboBox->setObjectName(QString::fromUtf8("alphaThresholdComboBox"));
        alphaThresholdComboBox->setEnabled(false);
        alphaThresholdComboBox->setEditable(true);
        alphaThresholdComboBox->setMaxVisibleItems(8);

        alphaThresholdLayout->addWidget(alphaThresholdComboBox);


        horizontalLayout->addLayout(alphaThresholdLayout);

        medLowThresholdLayout = new QVBoxLayout();
        medLowThresholdLayout->setSpacing(0);
        medLowThresholdLayout->setObjectName(QString::fromUtf8("medLowThresholdLayout"));
        medLowThresholdLabel = new QLabel(topFrame);
        medLowThresholdLabel->setObjectName(QString::fromUtf8("medLowThresholdLabel"));

        medLowThresholdLayout->addWidget(medLowThresholdLabel);

        medLowThresholdComboBox = new QComboBox(topFrame);
        medLowThresholdComboBox->setObjectName(QString::fromUtf8("medLowThresholdComboBox"));
        medLowThresholdComboBox->setEnabled(false);
        medLowThresholdComboBox->setEditable(true);
        medLowThresholdComboBox->setMaxVisibleItems(8);

        medLowThresholdLayout->addWidget(medLowThresholdComboBox);


        horizontalLayout->addLayout(medLowThresholdLayout);

        hiMedThresholdLayout = new QVBoxLayout();
        hiMedThresholdLayout->setSpacing(0);
        hiMedThresholdLayout->setObjectName(QString::fromUtf8("hiMedThresholdLayout"));
        hiMedThresholdLabel = new QLabel(topFrame);
        hiMedThresholdLabel->setObjectName(QString::fromUtf8("hiMedThresholdLabel"));

        hiMedThresholdLayout->addWidget(hiMedThresholdLabel);

        hiMedThresholdComboBox = new QComboBox(topFrame);
        hiMedThresholdComboBox->setObjectName(QString::fromUtf8("hiMedThresholdComboBox"));
        hiMedThresholdComboBox->setEnabled(false);
        hiMedThresholdComboBox->setEditable(true);
        hiMedThresholdComboBox->setMaxVisibleItems(8);

        hiMedThresholdLayout->addWidget(hiMedThresholdComboBox);


        horizontalLayout->addLayout(hiMedThresholdLayout);

        outputFormatLayout = new QVBoxLayout();
        outputFormatLayout->setSpacing(0);
        outputFormatLayout->setObjectName(QString::fromUtf8("outputFormatLayout"));
        outputFormatLabel = new QLabel(topFrame);
        outputFormatLabel->setObjectName(QString::fromUtf8("outputFormatLabel"));

        outputFormatLayout->addWidget(outputFormatLabel);

        outputFormatComboBox = new QComboBox(topFrame);
        outputFormatComboBox->setObjectName(QString::fromUtf8("outputFormatComboBox"));
        outputFormatComboBox->setEnabled(false);

        outputFormatLayout->addWidget(outputFormatComboBox);


        horizontalLayout->addLayout(outputFormatLayout);

        paletteLayout = new QVBoxLayout();
        paletteLayout->setSpacing(0);
        paletteLayout->setObjectName(QString::fromUtf8("paletteLayout"));
        paletteLabel = new QLabel(topFrame);
        paletteLabel->setObjectName(QString::fromUtf8("paletteLabel"));

        paletteLayout->addWidget(paletteLabel);

        paletteComboBox = new QComboBox(topFrame);
        paletteComboBox->setObjectName(QString::fromUtf8("paletteComboBox"));
        paletteComboBox->setEnabled(false);

        paletteLayout->addWidget(paletteComboBox);


        horizontalLayout->addLayout(paletteLayout);

        filterLayout = new QVBoxLayout();
        filterLayout->setSpacing(0);
        filterLayout->setObjectName(QString::fromUtf8("filterLayout"));
        filterLabel = new QLabel(topFrame);
        filterLabel->setObjectName(QString::fromUtf8("filterLabel"));

        filterLayout->addWidget(filterLabel);

        filterComboBox = new QComboBox(topFrame);
        filterComboBox->setObjectName(QString::fromUtf8("filterComboBox"));
        filterComboBox->setEnabled(false);
        filterComboBox->setMaxVisibleItems(8);

        filterLayout->addWidget(filterComboBox);


        horizontalLayout->addLayout(filterLayout);


        verticalLayout->addWidget(topFrame);

        frame_3 = new QFrame(centralWidget);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMinimumSize(QSize(0, 20));
        frame_3->setFrameShape(QFrame::Panel);
        frame_3->setFrameShadow(QFrame::Raised);
        verticalLayout_8 = new QVBoxLayout(frame_3);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(7, 1, 6, 0);
        sourceInfoLabel = new QLabel(frame_3);
        sourceInfoLabel->setObjectName(QString::fromUtf8("sourceInfoLabel"));

        verticalLayout_8->addWidget(sourceInfoLabel);


        verticalLayout->addWidget(frame_3);

        sourceScrollArea = new QScrollArea(centralWidget);
        sourceScrollArea->setObjectName(QString::fromUtf8("sourceScrollArea"));
        sourceScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 736, 150));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        sourceImage = new ZoomableImageArea(scrollAreaWidgetContents);
        sourceImage->setObjectName(QString::fromUtf8("sourceImage"));
        sourceImage->setScaledContents(false);

        gridLayout->addWidget(sourceImage, 0, 0, 1, 1);

        sourceScrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(sourceScrollArea);

        frame_5 = new QFrame(centralWidget);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setMinimumSize(QSize(0, 20));
        frame_5->setFrameShape(QFrame::Panel);
        frame_5->setFrameShadow(QFrame::Raised);
        verticalLayout_9 = new QVBoxLayout(frame_5);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(7, 1, 6, 0);
        targetInfoLabel = new QLabel(frame_5);
        targetInfoLabel->setObjectName(QString::fromUtf8("targetInfoLabel"));

        verticalLayout_9->addWidget(targetInfoLabel);


        verticalLayout->addWidget(frame_5);

        targetScrollArea = new QScrollArea(centralWidget);
        targetScrollArea->setObjectName(QString::fromUtf8("targetScrollArea"));
        targetScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 736, 150));
        gridLayout_2 = new QGridLayout(scrollAreaWidgetContents_2);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        targetImage = new ZoomableImageArea(scrollAreaWidgetContents_2);
        targetImage->setObjectName(QString::fromUtf8("targetImage"));
        targetImage->setScaledContents(false);

        gridLayout_2->addWidget(targetImage, 0, 0, 1, 1);

        targetScrollArea->setWidget(scrollAreaWidgetContents_2);

        verticalLayout->addWidget(targetScrollArea);

        bottomFrame = new QFrame(centralWidget);
        bottomFrame->setObjectName(QString::fromUtf8("bottomFrame"));
        bottomFrame->setFrameShape(QFrame::Panel);
        bottomFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(bottomFrame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        consoleOutput = new QPlainTextEdit(bottomFrame);
        consoleOutput->setObjectName(QString::fromUtf8("consoleOutput"));
        consoleOutput->setFocusPolicy(Qt::NoFocus);
        consoleOutput->setAcceptDrops(false);
        consoleOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        consoleOutput->setUndoRedoEnabled(false);
        consoleOutput->setReadOnly(true);

        horizontalLayout_2->addWidget(consoleOutput);

        subtitleImage = new EditPane(bottomFrame);
        subtitleImage->setObjectName(QString::fromUtf8("subtitleImage"));

        horizontalLayout_2->addWidget(subtitleImage);

        horizontalLayout_2->setStretch(0, 10);
        horizontalLayout_2->setStretch(1, 4);

        verticalLayout->addWidget(bottomFrame);

        verticalLayout->setStretch(2, 6);
        verticalLayout->setStretch(4, 6);
        verticalLayout->setStretch(5, 6);
        BDSup2Sub->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BDSup2Sub);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 738, 21));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Recent_Files = new QMenu(menu_File);
        menu_Recent_Files->setObjectName(QString::fromUtf8("menu_Recent_Files"));
        menu_Recent_Files->setEnabled(false);
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        menu_Settings = new QMenu(menuBar);
        menu_Settings->setObjectName(QString::fromUtf8("menu_Settings"));
        menu_About = new QMenu(menuBar);
        menu_About->setObjectName(QString::fromUtf8("menu_About"));
        BDSup2Sub->setMenuBar(menuBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menu_Settings->menuAction());
        menuBar->addAction(menu_About->menuAction());
        menu_File->addAction(action_Load);
        menu_File->addAction(menu_Recent_Files->menuAction());
        menu_File->addAction(action_Save_Export);
        menu_File->addAction(action_Close);
        menu_File->addAction(action_Exit);
        menu_Edit->addAction(action_Edit_Frame);
        menu_Edit->addAction(actionEdit_default_DVD_Palette);
        menu_Edit->addAction(actionEdit_imported_DVD_Palette);
        menu_Edit->addAction(actionEdit_DVD_Frame_Palette);
        menu_Edit->addAction(action_Move_all_captions);
        menu_Edit->addAction(action_Reset_crop_offset);
        menu_Settings->addAction(action_Conversion_Settings);
        menu_Settings->addAction(action_Swap_Cr_Cb);
        menu_Settings->addAction(action_Fix_invisible_frames);
        menu_Settings->addAction(action_Verbose_Output);
        menu_About->addAction(actionAbout_Qt);

        retranslateUi(BDSup2Sub);

        alphaThresholdComboBox->setCurrentIndex(-1);
        paletteComboBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(BDSup2Sub);
    } // setupUi

    void retranslateUi(QMainWindow *BDSup2Sub)
    {
        BDSup2Sub->setWindowTitle(QApplication::translate("BDSup2Sub", "BDSup2Sub++", 0, QApplication::UnicodeUTF8));
        action_Load->setText(QApplication::translate("BDSup2Sub", "&Load", 0, QApplication::UnicodeUTF8));
        action_Save_Export->setText(QApplication::translate("BDSup2Sub", "&Save/Export", 0, QApplication::UnicodeUTF8));
        action_Close->setText(QApplication::translate("BDSup2Sub", "&Close", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("BDSup2Sub", "&Exit", 0, QApplication::UnicodeUTF8));
        action_Edit_Frame->setText(QApplication::translate("BDSup2Sub", "&Edit Frame", 0, QApplication::UnicodeUTF8));
        actionEdit_default_DVD_Palette->setText(QApplication::translate("BDSup2Sub", "Edit &default DVD Palette", 0, QApplication::UnicodeUTF8));
        actionEdit_imported_DVD_Palette->setText(QApplication::translate("BDSup2Sub", "Edit &imported DVD Palette", 0, QApplication::UnicodeUTF8));
        actionEdit_DVD_Frame_Palette->setText(QApplication::translate("BDSup2Sub", "Edit DVD &Frame Palette", 0, QApplication::UnicodeUTF8));
        action_Move_all_captions->setText(QApplication::translate("BDSup2Sub", "&Move all captions", 0, QApplication::UnicodeUTF8));
        action_Reset_crop_offset->setText(QApplication::translate("BDSup2Sub", "&Reset crop offset", 0, QApplication::UnicodeUTF8));
        action_Conversion_Settings->setText(QApplication::translate("BDSup2Sub", "&Conversion Settings", 0, QApplication::UnicodeUTF8));
        action_Swap_Cr_Cb->setText(QApplication::translate("BDSup2Sub", "&Swap Cr/Cb", 0, QApplication::UnicodeUTF8));
        action_Fix_invisible_frames->setText(QApplication::translate("BDSup2Sub", "&Fix invisible frames", 0, QApplication::UnicodeUTF8));
        action_Verbose_Output->setText(QApplication::translate("BDSup2Sub", "&Verbose Output", 0, QApplication::UnicodeUTF8));
        action_Help->setText(QApplication::translate("BDSup2Sub", "&Help", 0, QApplication::UnicodeUTF8));
        actionHello->setText(QApplication::translate("BDSup2Sub", "hello", 0, QApplication::UnicodeUTF8));
        actionAbout_Qt->setText(QApplication::translate("BDSup2Sub", "About &Qt", 0, QApplication::UnicodeUTF8));
        subtitleLanguageLabel->setText(QApplication::translate("BDSup2Sub", "Subtitle Language", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        subtitleLanguageComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Select language in IDX file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        subtitleNumberLabel->setText(QApplication::translate("BDSup2Sub", "Subtitle", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        subtitleNumberComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Set subtitle number", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        alphaThresholdLabel->setText(QApplication::translate("BDSup2Sub", "Alpha Threshold", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        alphaThresholdComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Set alpha threshold", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        medLowThresholdLabel->setText(QApplication::translate("BDSup2Sub", "Med/Low Threshold", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        medLowThresholdComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Set low/medium luminance threshold", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        hiMedThresholdLabel->setText(QApplication::translate("BDSup2Sub", "Hi/Med Threshold", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        hiMedThresholdComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Set medium/high luminance threshold", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        outputFormatLabel->setText(QApplication::translate("BDSup2Sub", "Output Format", 0, QApplication::UnicodeUTF8));
        outputFormatComboBox->clear();
        outputFormatComboBox->insertItems(0, QStringList()
         << QApplication::translate("BDSup2Sub", "SUB/IDX", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "SUP/IFO", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "SUP(BD)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "XML/PNG", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        outputFormatComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Select export format", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        paletteLabel->setText(QApplication::translate("BDSup2Sub", "Palette", 0, QApplication::UnicodeUTF8));
        paletteComboBox->clear();
        paletteComboBox->insertItems(0, QStringList()
         << QApplication::translate("BDSup2Sub", "keep existing", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "create new", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "dithered", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        paletteComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Select palette mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        filterLabel->setText(QApplication::translate("BDSup2Sub", "Filter", 0, QApplication::UnicodeUTF8));
        filterComboBox->clear();
        filterComboBox->insertItems(0, QStringList()
         << QApplication::translate("BDSup2Sub", "Bilinear", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Triangle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Bicubic", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Bell", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Bicubic-Spline", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Hermite", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Lanczos3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("BDSup2Sub", "Mitchell", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        filterComboBox->setToolTip(QApplication::translate("BDSup2Sub", "Select filter for scaling", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        sourceInfoLabel->setText(QString());
        sourceImage->setText(QString());
        targetInfoLabel->setText(QString());
        targetImage->setText(QString());
        subtitleImage->setText(QString());
        menu_File->setTitle(QApplication::translate("BDSup2Sub", "&File", 0, QApplication::UnicodeUTF8));
        menu_Recent_Files->setTitle(QApplication::translate("BDSup2Sub", "&Recent Files", 0, QApplication::UnicodeUTF8));
        menu_Edit->setTitle(QApplication::translate("BDSup2Sub", "&Edit", 0, QApplication::UnicodeUTF8));
        menu_Settings->setTitle(QApplication::translate("BDSup2Sub", "&Settings", 0, QApplication::UnicodeUTF8));
        menu_About->setTitle(QApplication::translate("BDSup2Sub", "&About", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BDSup2Sub: public Ui_BDSup2Sub {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BDSUP2SUB_H
