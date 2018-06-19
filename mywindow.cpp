#include "mywindow.h"

MyWindow::MyWindow(QWidget *parent) : QWidget(parent) {

    imageCurent = new QPixmap;
    imageCurent->load(":/3.jpg");
    imageSource = new QPixmap;
    imageSource->load(":/3.jpg");
    display = new QLabel;
    display->setPixmap(*imageCurent);
    display->setAutoFillBackground(true);

    sa = new QScrollArea;
    sa->setMinimumHeight(500);
    sa->setMinimumWidth(500);
    sa->setAlignment(Qt::AlignCenter);
    sa->setWidget(display);

    layers = new QComboBox;

    int maxLayers = defineLayers(imageSource->width(), imageSource->height());

    for(int i = 0; i < maxLayers; i++) {
         layers->addItem(QString::number(i));
    }

    infoSize = new QLabel;
    infoSize->setText(QString::number(imageCurent->width()) + " x " + QString::number(imageCurent->height()));

    QVBoxLayout *work = new QVBoxLayout;
    QHBoxLayout *interFace = new QHBoxLayout;
    interFace->addWidget(layers);
    interFace->addWidget(infoSize);
    work->addWidget(sa);
    work->addItem(interFace);
    setWindowTitle("Pyramid");

    connect(layers, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLayer(int)));
    connect(layers, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSize()));

    setLayout(work);
}

int MyWindow::defineLayers(int width, int height) {

    int countLayers = 0;

    if(width <= height) {
        while(width != 0) {
            width /=2;
            countLayers++;
        }
    }
    else {
        while(height != 0) {
            height /=2;
            countLayers++;
        }
    }

    return countLayers;
}

void MyWindow::changeLayer(int layer) {

    qreal koef = qPow(2, layer);
    QPixmap *imageBuf = new QPixmap;
    *imageBuf = imageSource->scaled(imageSource->width()/koef, imageSource->height()/koef,  Qt::IgnoreAspectRatio);
    infoSize->setText(QString::number((int)imageSource->width()/(int)koef) + " x " + QString::number((int)imageSource->height()/(int)koef));
    *imageCurent = imageBuf->scaled(imageBuf->width()*koef, imageBuf->height()*koef,  Qt::IgnoreAspectRatio);
    display->setPixmap(*imageCurent);
    delete imageBuf;
}
