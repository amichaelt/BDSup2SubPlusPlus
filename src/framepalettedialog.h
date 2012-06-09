#ifndef FRAMEPALETTEDIALOG_H
#define FRAMEPALETTEDIALOG_H

#include <QDialog>

class SubtitleProcessor;

namespace Ui {
class FramePaletteDialog;
}

class FramePaletteDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FramePaletteDialog(QWidget *parent = 0, SubtitleProcessor* subtitleProcessor = 0);
    ~FramePaletteDialog();

    void setIndex(int idx);
    
private slots:

    void on_color1ComboBox_currentIndexChanged(int index);
    void on_color2ComboBox_currentIndexChanged(int index);
    void on_color3ComboBox_currentIndexChanged(int index);
    void on_color4ComboBox_currentIndexChanged(int index);
    void on_alpha1ComboBox_currentIndexChanged(int index);
    void on_alpha2ComboBox_currentIndexChanged(int index);
    void on_alpha3ComboBox_currentIndexChanged(int index);
    void on_alpha4ComboBox_currentIndexChanged(int index);

    void on_resetAllButton_clicked();
    void on_setAllButton_clicked();
    void on_cancelButton_clicked();
    void on_resetButton_clicked();
    void on_okButton_clicked();

private:
    Ui::FramePaletteDialog *ui;
    SubtitleProcessor* subtitleProcessor = 0;
    QStringList colorNames = {	"00", "01", "02", "03", "04", "05", "06" ,"07",
                                "08", "09", "10", "11", "12", "13", "14", "15"
                             };
    QVector<QIcon> colorIcons;
    int index;
    QVector<int> alpha;
    QVector<int> pal;
};

#endif // FRAMEPALETTEDIALOG_H
