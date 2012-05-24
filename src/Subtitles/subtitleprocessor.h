/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SUBTITLEPROCESSOR_H
#define SUBTITLEPROCESSOR_H

#include <array>
#include <QObject>
#include <QByteArray>
#include <QString>
#include <vector>
#include <QStringList>

#include "palette.h"
#include "types.h"

static QVector<QVector<int>> resolutions = {
    {720, 480},
    {720, 576},
    {1280, 720},
    {1440, 1080},
    {1920, 1080}
};

static QVector<QVector<QString>> languages = {{
    {"German",       "de", "deu"},
    {"English",      "en", "eng"},
    {"French",       "fr", "fra"},
    {"Italian",      "it", "ita"},
    {"Spanish",      "es", "spa"},
    {"Abkhazian",    "ab", "abk"},
    {"Afar",         "aa", "aar"},
    {"Afrikaans",    "af", "afr"},
    {"Albanian",     "sq", "sqi"},
    {"Amharic",      "am", "amh"},
    {"Arabic",       "ar", "ara"},
    {"Aragonese",    "an", "arg"},
    {"Armenian",     "hy", "hye"},
    {"Assamese",     "as", "asm"},
    {"Avaric",       "av", "ava"},
    {"Avestan",      "ae", "ave"},
    {"Aymara",       "ay", "aym"},
    {"Azerbaijani",  "az", "aze"},
    {"Bambara",      "bm", "bam"},
    {"Bashkir",      "ba", "bak"},
    {"Basque",       "eu", "eus"},
    {"Belarusian",   "be", "bel"},
    {"Bengali",      "bn", "ben"},
    {"Bihari",       "bh", "bih"},
    {"Bislama",      "bi", "bis"},
    {"Bosnian",      "bs", "bos"},
    {"Breton",       "br", "bre"},
    {"Bulgarian",    "bg", "bul"},
    {"Burmese",      "my", "mya"},
    {"Cambodian",    "km", "khm"},
    {"Catalan",      "ca", "cat"},
    {"Chamorro",     "ch", "cha"},
    {"Chechen",      "ce", "che"},
    {"Chichewa",     "ny", "nya"},
    {"Chinese",      "zh", "zho"},
    {"Chuvash",      "cv", "chv"},
    {"Cornish",      "kw", "cor"},
    {"Corsican",     "co", "cos"},
    {"Cree",         "cr", "cre"},
    {"Croatian",     "hr", "hrv"},
    {"Czech",        "cs", "ces"},
    {"Danish",       "da", "dan"},
    {"Divehi",       "dv", "div"},
    {"Dzongkha",     "dz", "dzo"},
    {"Dutch",        "nl", "nld"},
    {"Esperanto",    "eo", "epo"},
    {"Estonian",     "et" ,"est"},
    {"Ewe",          "ee", "ewe"},
    {"Faroese",      "fo", "fao"},
    {"Fiji",         "fj", "fij"},
    {"Finnish",      "fi", "fin"},
    {"Frisian",      "fy", "fry"},
    {"Fulah",        "ff", "ful"},
    {"Gaelic",       "gd", "gla"},
    {"Galician",     "gl", "glg"},
    {"Ganda",        "lg", "lug"},
    {"Georgian",     "ka", "kat"},
    {"Greek",        "el", "ell"},
    {"Greenlandic",  "kl", "kal"},
    {"Guarani",      "gn", "grn"},
    {"Gujarati",     "gu", "guj"},
    {"Haitian",      "ht", "hat"},
    {"Hausa",        "ha", "hau"},
    {"Hebrew",       "he", "heb"},
    {"Herero",       "hz", "her"},
    {"Hindi",        "hi", "hin"},
    {"Hiri Motu",    "ho", "hmo"},
    {"Hungarian",    "hu", "hun"},
    {"Icelandic",    "is", "isl"},
    {"Ido",          "io", "ido"},
    {"Igbo",         "ig", "ibo"},
    {"Indonesian",   "id", "ind"},
    {"Interlingua",  "ia", "ina"},
    {"Interlingue",  "ie", "ile"},
    {"Inupiaq",      "ik", "ipk"},
    {"Inuktitut",    "iu", "iku"},
    {"Irish",        "ga", "gle"},
    {"Japanese",     "ja", "jpn"},
    {"Javanese",     "jv", "jav"},
    {"Kannada",      "kn", "kan"},
    {"Kanuri",       "kr", "kau"},
    {"Kashmiri",     "ks", "kas"},
    {"Kazakh",       "kk", "kaz"},
    {"Kikuyu",       "ki", "kik"},
    {"Kinyarwanda",  "rw", "kin"},
    {"Kirghiz",      "ky", "kir"},
    {"Komi",         "kv", "kom"},
    {"Kongo",        "kg", "kon"},
    {"Korean",       "ko", "kor"},
    {"Kuanyama",     "kj", "kua"},
    {"Kurdish",      "ku", "kur"},
    {"Lao",          "lo", "lao"},
    {"Latin",        "la", "lat"},
    {"Latvian",      "lv", "lav"},
    {"Limburgan",    "li", "lim"},
    {"Lingala",      "ln", "lin"},
    {"Lithuanian",   "lt", "lit"},
    {"Luba",         "lu", "lub"},
    {"Luxembourgish","lb", "ltz"},
    {"Macedonian",   "mk", "mkd"},
    {"Malagasy",     "mg", "mlg"},
    {"Malay",        "ms", "msa"},
    {"Malayalam",    "ml", "mal"},
    {"Maltese",      "mt", "mlt"},
    {"Marshallese",  "mh", "mah"},
    {"Manx",         "gv", "glv"},
    {"Maori",        "mi", "mri"},
    {"Marathi",      "mr", "mar"},
    {"Mongolian",    "mn", "mon"},
    {"Nauru",        "na", "nau"},
    {"Navajo",       "nv", "nav"},
    {"Ndebele",      "nd", "nde"},
    {"Ndonga",       "ng", "ndo"},
    {"Nepali",       "ne", "nep"},
    {"Norwegian",    "no", "nor"},
    {"Occitan",      "oc", "oci"},
    {"Ojibwa",       "oj", "oji"},
    {"Oriya",        "or", "ori"},
    {"Oromo",        "om", "orm"},
    {"Ossetian",     "os", "oss"},
    {"Pali",         "pi", "pli"},
    {"Panjabi",      "pa", "pan"},
    {"Pashto",       "ps", "pus"},
    {"Persian",      "fa", "fas"},
    {"Polish",       "pl", "pol"},
    {"Portuguese",   "pt", "por"},
    {"Quechua",      "qu", "que"},
    {"Romansh",      "rm", "roh"},
    {"Romanian",     "ro", "ron"},
    {"Rundi",        "rn", "run"},
    {"Russian",      "ru", "rus"},
    {"Sami",         "se", "sme"},
    {"Samoan",       "sm", "smo"},
    {"Sango",        "sg", "sag"},
    {"Sanskrit",     "sa", "san"},
    {"Sardinian",    "sc", "srd"},
    {"Serbian",      "sr", "srp"},
    {"Shona",        "sn", "sna"},
    {"Sichuan Yi",   "ii", "iii"},
    {"Sindhi",       "sd", "snd"},
    {"Sinhalese",    "si", "sin"},
    {"Slavonic",     "cu", "chu"},
    {"Slovak",       "sk", "slk"},
    {"Slovenian",    "sl", "slv"},
    {"Somali",       "so", "som"},
    {"Sotho",        "st", "sot"},
    {"Sundanese",    "su", "sun"},
    {"Swahili",      "sw", "swa"},
    {"Swati",        "ss", "ssw"},
    {"Swedish",      "sv", "swe"},
    {"Tagalog",      "tl", "tgl"},
    {"Tahitian",     "ty", "tah"},
    {"Tajik",        "tg", "tgk"},
    {"Tamil",        "ta", "tam"},
    {"Tatar",        "tt", "tar"},
    {"Telugu",       "te", "tel"},
    {"Thai",         "th", "tha"},
    {"Tibetan",      "bo", "bod"},
    {"Tigrinya",     "ti", "tir"},
    {"Tonga",        "to", "ton"},
    {"Tsonga",       "ts", "tso"},
    {"Tswana",       "tn", "tsn"},
    {"Turkish",      "tr", "tur"},
    {"Turkmen",      "tk", "tuk"},
    {"Twi",          "tw", "twi"},
    {"Uighur",       "ug", "uig"},
    {"Ukrainian",    "uk", "ukr"},
    {"Urdu",         "ur", "urd"},
    {"Uzbek",        "uz", "uzb"},
    {"Venda",        "ve", "ven"},
    {"Vietnamese",   "vi", "vie"},
    {"Volapük",     "vo", "vol"},
    {"Welsh",        "cy", "cym"},
    {"Walloon",      "wa", "wln"},
    {"Wolof",        "wo", "wol"},
    {"Xhosa",        "xh", "xho"},
    {"Yiddish",      "yi", "yid"},
    {"Yoruba",       "yo", "yor"},
    {"Zhuang",       "za", "zha"},
    {"Zulu",         "zu", "zul"}
}};

/** Frames per seconds for 24p (23.976) */
constexpr double FPS_24P  = 24000.0/1001;
/** Frames per seconds for wrong 24P (23.975) */
constexpr double FPS_23_975 = 23.975;
/** Frames per seconds for 24Hz (24.0) */
constexpr double FPS_24HZ = 24.0;
/** Frames per seconds for PAL progressive (25.0) */
constexpr double FPS_PAL  = 25.0;
/** Frames per seconds for NTSC progressive (29.97) */
constexpr double FPS_NTSC = 30000.0/1001;
/** Frames per seconds for PAL interlaced (50.0) */
constexpr double FPS_PAL_I  = 50.0;
/** Frames per seconds for NTSC interlaced (59.94) */
constexpr double FPS_NTSC_I = 60000.0/1001;

class Substream;
class SubDVD;
class SupDVD;
class SupXML;
class SupHD;
class SupBD;
class SubPictureDVD;
class SubPicture;
class QImage;
class Bitmap;

class SubtitleProcessor : public QObject
{
    Q_OBJECT

public:
    SubtitleProcessor();

    QByteArray getFileID(QString fileName, int numberOfBytes);
    StreamID getStreamID(QByteArray id);
    QVector<QVector<QString>>& getLanguages() { return languages; }
    Palette* getDefaultDVDPalette() { return defaultDVDPalette; }
    int getAlphaCrop() { return alphaCrop; }
    bool getFixZeroAlpha() { return fixZeroAlpha; }
    int getAlphaThreshold() { return alphaThreshold; }
    int getNumberOfFrames();
    QVector<int> getLuminanceThreshold() { return luminanceThreshold; }
    QImage* getSrcImage();
    QImage* getSrcImage(int index);
    void setLoadPath(QString loadPath) { fileName = loadPath; }
    int getCropOfsY() { return cropOfsY; }
    int setCropOfsY(int ofs) { return cropOfsY = ofs; }
    OutputMode getOutputMode() { return outMode; }
    InputMode getInputMode() { return inMode; }
    bool getMoveCaptions() { return moveCaptions; }
    bool getConvertResolution() { return convertResolution; }
    Resolution getResolution(int width, int height);
    Resolution getOutputResolution() { return resolutionTrg; }
    SubPicture* getSubPictureSrc(int index);
    int getDelayPTS() { return delayPTS; }
    long syncTimePTS(long timeStamp, double fps);
    int getMinTimePTS() { return minTimePTS; }
    double getFPSTrg() { return fpsTrg; }
    bool getConvertFPS() { return convertFPS; }
    bool getApplyFreeScale() { return applyFreeScale; }
    bool getFixShortFrames() { return fixShortFrames; }
    double getFPSSrc() { return fpsSrc; }
    double getFreeScaleX() { return freeScaleX; }
    double getFreeScaleY() { return freeScaleY; }
    bool getFpsSrcCertain() { return fpsSrcCertain; }
    void close();
    void scanSubtitles();
    QVector<int> getResolution(Resolution resolution);
    void validateTimes(int index, SubPicture* subPicture, SubPicture* subPictureNext, SubPicture* subPicturePrevious);
    void moveAllToBounds();
    void convertSup(int index, int displayNumber, int displayMax);
    void convertSup(int index, int displayNumber, int displayMax, bool skipScaling);
    void determineFramePalette(int index);
    bool updateTrgPic(int index);
    void loadedHook() { fpsSrcSet = false; }
    void addRecent(QString fileName);
    void moveToBounds(SubPicture* picture, int index, double barFactor, int offsetX, int offsetY, MoveModeX moveModeX, MoveModeY moveModeY, int cropOffsetY);
    QString getSrcInfoStr(int index);
    QString getTrgInfoStr(int index);
    QStringList getRecentFiles() { return recentFiles; }
    PaletteMode getPaletteMode() { return paletteMode; }
    int getTrgWidth(int index);
    int getTrgHeight(int index);
    int getTrgOfsX(int index);
    int getTrgOfsY(int index);
    QImage* getTrgImage();
    int getTrgImgWidth(int index);
    int getTrgImgHeight(int index);
    bool getTrgExcluded(int index);

signals:
    void windowTitleChanged(const QString &newTitle);
    void progressDialogTitleChanged(QString newTime);
    void progressDialogTextChanged(QString newText);
    void progressDialogValueChanged(int value);
    void progressDialogVisibilityChanged(bool visible);
    void loadingSubtitleFinished();
    void moveAllFinished();

public slots:
    void setMaxProgress(int maxProgress);
    void setCurrentProgress(int currentProgress);
    void readSubtitleStream();
    void moveAll();

private:
    Substream* substream = 0;
    SubDVD* subDVD;
    SupDVD* supDVD;
    SupXML* supXML;
    SupHD* supHD;
    SupBD* supBD;
    SubPictureDVD* subVobTrg;
    Bitmap* trgBitmap;
    Bitmap* trgBitmapUnpatched;
    QVector<SubPicture*> subPictures;
    Palette* defaultSourceDVDPalette;
    Palette* currentSourceDVDPalette;
    Palette* trgPal;
    Palette *defaultDVDPalette;
    Palette *currentDVDPalette;
    QStringList recentFiles;
    int maxProgress = 0, lastProgress = 0;
    int numberOfErrors, numberOfWarnings;
    int languageIdx = 0;
    int alphaCrop = 14;
    int alphaThreshold = 80;
    int cropOfsY = 0;
    static constexpr int delayPTSdefault = 0;
    int delayPTS = delayPTSdefault;
    static constexpr int minTimePTSdefault = 90 * 500;
    int minTimePTS = minTimePTSdefault;
    int minDim = 8;
    int moveOffsetY = 10;
    int moveOffsetX = 10;
    int numRecent = 5;
    double minScale = 0.5;
    double maxScale = 2.0;
    static constexpr double fpsSrcDefault = FPS_24P;
    double fpsSrc = fpsSrcDefault;
    static constexpr double fpsTrgDefault = FPS_PAL;
    double fpsTrg = fpsTrgDefault;
    static constexpr double freeScaleXdefault = 1.0;
    double freeScaleX = freeScaleXdefault;
    static constexpr double freeScaleYdefault = 1.0;
    double freeScaleY = freeScaleYdefault;
    double cineBarFactor = 5.0/42;
    bool fpsSrcCertain = false;
    bool fpsSrcSet = false;
    bool useBT601 = false;
    bool fixZeroAlpha = false;
    bool moveCaptions = false;
    bool cliMode = false;
    static constexpr bool applyFreeScaleDefault = false;
    bool applyFreeScale = applyFreeScaleDefault;
    static constexpr bool convertFPSdefault = false;
    bool convertFPS = convertFPSdefault;
    static constexpr bool convertResolutionDefault = false;
    bool convertResolution = convertResolutionDefault;
    static constexpr bool fixShortFramesDefault = false;
    bool fixShortFrames = fixShortFramesDefault;
    bool scalingFilterSet = false;
    bool paletteModeSet = false;
    static constexpr PaletteMode paletteModeDefault = PaletteMode::CREATE_NEW;
    PaletteMode paletteMode = paletteModeDefault;
    RunType runType;
    static constexpr OutputMode outModeDefault = OutputMode::VOBSUB;
    OutputMode outMode = outModeDefault;
    bool outModeSet = false;
    InputMode inMode = InputMode::VOBSUB;
    static constexpr Resolution resolutionTrgDefault = Resolution::PAL;
    Resolution resolutionTrg = resolutionTrgDefault;
    SetState forceAll = SetState::KEEP;
    MoveModeY moveModeY = MoveModeY::KEEP;
    MoveModeX moveModeX = MoveModeX::KEEP;
    static constexpr ScalingFilters scalingFilterDefault = ScalingFilters::BILINEAR;
    ScalingFilters scalingFilter = scalingFilterDefault;

    QString fileName;
    QVector<int> luminanceThreshold = { 210, 160 };

    QVector<int> alphaDefault = { 0, 0xf, 0xf, 0xf};

    QVector<QVector<int>> resolutions = {
        {720, 480},
        {720, 576},
        {1280, 720},
        {1440, 1080},
        {1920, 1080}
    };

    void readXml();
    void readDVDSubStream(StreamID streamID, bool isVobSub);
    void readSup();
};

#endif // SUBTITLEPROCESSOR_H
