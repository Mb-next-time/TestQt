#include "mywindow.h"

MyWindow::MyWindow(QWidget *parent) : QWidget(parent) {

    imageCurent = new QPixmap;
    QPixmap buf(500, 500);
    buf.fill(Qt::white);
    *imageCurent = buf;

    /*
    if(imageCurent->load("C:/Users/Mb_next_time/Desktop/test/testClass/resource/4.jpg")) {
        QMessageBox check;
        check.setText("Успешно");
        check.exec();
    }
    else {
        QMessageBox check;
        check.setText("Файл не загружен");
        check.exec();
    }
    */

    imageSource = new QPixmap;
    //imageSource->load(":\resource\4.jpg");
    display = new QLabel;
    display->setPixmap(*imageCurent);
    display->setAutoFillBackground(true);
    display->adjustSize();

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

    openBut = new QPushButton("Open");

    interFace->addWidget(openBut);
    interFace->addWidget(layers);
    interFace->addWidget(infoSize);
    work->addWidget(sa);
    work->addItem(interFace);
    setWindowTitle("Pyramid");
    setLayout(work);

    connect(layers, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLayer(int)));
    connect(openBut, SIGNAL(clicked(bool)), this, SLOT(open()));
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

    // добавить хэш-таблицу
    if(layer > 0) {
        qreal koef = qPow(2, layer);
        QPixmap *imageBuf = new QPixmap;
        *imageBuf = imageSource->scaled(imageSource->width()/koef, imageSource->height()/koef,  Qt::IgnoreAspectRatio);
        infoSize->setText(QString::number((int)imageSource->width()/(int)koef) + " x " + QString::number((int)imageSource->height()/(int)koef));
        *imageCurent = imageBuf->scaled(imageBuf->width()*koef, imageBuf->height()*koef,  Qt::IgnoreAspectRatio);
        display->setPixmap(*imageCurent);
        delete imageBuf;
    }
}

void MyWindow::setImage(const QPixmap &image) {

    *imageSource = image;
    *imageCurent = image;

    display->setPixmap(*imageCurent);

    int maxLayers = defineLayers(imageSource->width(), imageSource->height());

    for(int i = 0; i < maxLayers; i++) {
         layers->addItem(QString::number(i));
    }
}

bool MyWindow::loadFile(const QString &fileName) {

    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    QImage newImage = reader.read();

    QPixmap buf;
    buf.load(fileName);
    *imageCurent = buf;
    *imageSource = buf;

    QMessageBox test;
    test.setText(QString::number(imageSource->height()));
    test.exec();

    sa->resize(imageSource->width(), imageSource->height());
    sa->setVisible(true);
    display->setAutoFillBackground(true);
    infoSize->setText(QString::number(imageCurent->width()) + " x " + QString::number(imageCurent->height()));
    display->setPixmap(*imageCurent);
    display->adjustSize();
    sa->setWidget(display);

    layers->clear();
    int maxLayers = defineLayers(imageSource->width(), imageSource->height());

    for(int i = 0; i < maxLayers; i++) {
         layers->addItem(QString::number(i));
    }
    layers->update();
    return true;
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode) {

    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void MyWindow::open() {

    QFileDialog dialog(this, tr("Open file"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}
