#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QtWidgets>
#include <QtAlgorithms>

class Image {
public:
    Image();
    Image(QString , QString, QPixmap);
    QString getPathName();
    QString getName();
    QPixmap getImage();
    int unsigned getWidth();
    int unsigned getHeight();
    int unsigned getLayers();
    qreal getDiag() const;
    void defineLayers(qreal reductFactor, int _width, int _height);
    ~Image();
private:
    QString pathName;
    QString name;
    QPixmap image;
    int unsigned width = 0;
    int unsigned height = 0;
    int unsigned layers = 0;
    qreal diag = 0;
    void defineDiag();
};

class MyWindow : public QWidget {
    Q_OBJECT
public:
    MyWindow(QWidget *parent=0);
    qreal getReductFactor() {
        return spinBoxReductFact->value();
    }
private:
    QLabel *display;
    QLabel *lblResol;
    QLabel *lblSize;
    QLabel *lblLayers;
    QLabel *lblFile;
    QLabel *lblReductFact;
    QComboBox *listLayers;
    QComboBox *listFiles;
    QPixmap *imageCurrent;
    QPixmap *imageSource;
    QScrollArea *sa;
    QPushButton *openBut;
    QDoubleSpinBox  *spinBoxReductFact;
    QVector<Image> images;
    void setImage();
    void refreshListLayers(int);
    void refreshListFiles();
private slots:
    void changeLayer(int);
    void changeFile(int);
    void open();
    void changeReductFactor();
};

// helper functions
QPixmap blur(QPixmap image);
bool comp(const Image &,  const Image &);
QString parser(QString );
int findIndex(const QString name, const QString pathName, QVector<Image> &data);

#endif // MYWINDOW_H
