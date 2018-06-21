#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QtWidgets>
#include <QtAlgorithms>

class Image {
public:
    Image();
    Image(QString , QPixmap);
    QString getName();
    QPixmap getImage();
    int unsigned getWidth();
    int unsigned getHeight();
    int unsigned getLayers();
    qreal getDiag() const;
    ~Image();
private:
    QString name;
    QPixmap image;
    int unsigned width = 0;
    int unsigned height = 0;
    int unsigned layers = 0;
    qreal diag = 0;
    void defineLayers();
    void defineDiag();
};

class MyWindow : public QWidget {
    Q_OBJECT
public:
    MyWindow(QWidget *parent=0);
private:
    QLabel *display;
    QComboBox *listLayers;
    QComboBox *listFiles;
    QLabel *infoSize;
    QPixmap *imageCurrent;
    QPixmap *imageSource;
    QScrollArea *sa;
    QPushButton *openBut;
    QVector<Image> images;
    int unsigned currentNumberFiles = 0;
    void setImage(const QString &);
    void refreshList();
    bool loadFile(const QString &);
private slots:
    void changeLayer(int layer);
    void changeFile(int number);
    void open();
};

#endif // MYWINDOW_H

