#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QtWidgets>

class MyWindow : public QWidget {
    Q_OBJECT
public:
    MyWindow(QWidget *parent=0);
private:
    QLabel *display;
    QComboBox *layers;
    QLabel *infoSize;
    QScrollArea *sa;
    QPixmap *imageCurent;
    QPixmap *imageSource;
    int defineLayers(int width, int heigth);
private slots:
    void changeLayer(int layer);
    //void changeFile(QString str);
    //void load();
};

#endif // MYWINDOW_H
