#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QtWidgets>

class MyWindow : public QWidget {
    Q_OBJECT
public:
    MyWindow(QWidget *parent=0);
    bool loadFile(const QString &);
private:
    QLabel *display;
    QComboBox *layers;
    QLabel *infoSize;
    QScrollArea *sa;
    QPixmap *imageCurent;
    QPixmap *imageSource;
    QPushButton *openBut;
    int defineLayers(int width, int heigth);
     void setImage(const QPixmap &image);
private slots:
    void changeLayer(int layer);
    //void changeFile(QString str);
    void open();
};

#endif // MYWINDOW_H
