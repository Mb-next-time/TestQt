#include "mywindow.h"

// for test
qreal reductFactor = 2;

Image::Image() {

}

Image::Image(QString _name, QString _pathName, QPixmap _image) {
    name = _name;
    pathName = _pathName;
    image = _image;
    width = _image.width();
    height = _image.height();
    defineDiag();
    defineLayers();
}

void Image::defineDiag() {
    diag = qPow((qPow(width, 2) + qPow(height,2)), (qreal)1/2);
}

void Image::defineLayers() {

    if(width <= height) {
        while(width > 0) {
            width /= reductFactor;
            layers++;
        }
    }
    else {
        while(height > 0) {
            height /= reductFactor;
            layers++;
        }
    }
}

QString Image::getPathName() {
    return pathName;
}

QString Image::getName() {
    return name;
}

QPixmap Image::getImage() {
    return image;
}
int unsigned Image::getWidth() {
    return width;
}

int unsigned Image::getHeight() {
    return height;
}

int unsigned Image::getLayers() {
    return layers;
}

qreal Image::getDiag() const {
    return diag;
}

Image::~Image() {

}

void MyWindow::setImage() {

    display->setPixmap(*imageCurrent);
    sa->setMinimumHeight(502);
    sa->setMinimumWidth(502);
    sa->setAlignment(Qt::AlignCenter);
    sa->resize(imageSource->width(), imageSource->height());
    sa->setVisible(true);
    display->setAutoFillBackground(true);
    lblResol->setText(QString::number(imageCurrent->width()) + " x " + QString::number(imageCurrent->height()));
    display->setPixmap(*imageCurrent);
    display->adjustSize();
    sa->setWidget(display);
}

MyWindow::MyWindow(QWidget *parent) : QWidget(parent) {

    int initWidth = 500;
    int initHeight = 500;

    imageCurrent = new QPixmap(initWidth, initHeight);
    imageCurrent->fill(Qt::white);
    imageSource = new QPixmap(initWidth, initHeight);
    imageSource->fill(Qt::white);
    display = new QLabel;
    sa = new QScrollArea;
    lblLayers = new QLabel("Layers");
    listLayers = new QComboBox;
    //reductFact = new QLineEdit("Reduction factor");
    lblFile = new QLabel("File");
    listFiles = new QComboBox;
    lblSize = new QLabel("Size:");
    lblResol = new QLabel;
    openBut = new QPushButton("Open");
    QVBoxLayout *work = new QVBoxLayout;
    QHBoxLayout *interFace = new QHBoxLayout;

    setImage();

    interFace->addWidget(openBut);
    interFace->addWidget(lblFile);
    interFace->addWidget(listFiles);
    interFace->addWidget(lblLayers);
    interFace->addWidget(listLayers);
    interFace->addWidget(lblSize);
    interFace->addWidget(lblResol);

    interFace->setSpacing(10);
    interFace->setStretch(0, 20);
    interFace->setStretch(1, 10);
    interFace->setStretch(2, 25);
    interFace->setStretch(3, 10);

    work->addWidget(sa);
    work->addItem(interFace);
    //work->addWidget(reductFact);
    setWindowTitle("Pyramid");
    setLayout(work);

    connect(listLayers, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLayer(int)));
    connect(listFiles, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFile(int)));
    connect(openBut, SIGNAL(clicked(bool)), this, SLOT(open()));
}

void MyWindow::refreshListLayers(int index) {

    listLayers->clear();

    for(int unsigned i = 0; i < images[index].getLayers(); i++) {
        listLayers->addItem(QString::number(i));
    }
    listLayers->update();
}

void MyWindow::refreshListFiles() {

    listFiles->clear();
    for(int i = 0; i < images.size(); i++) {
        listFiles->addItem(images[i].getName());
    }
    listFiles->update();

}

void MyWindow::changeFile(int number) {

    if(number > -1) {
        *imageCurrent = images[number].getImage();
        *imageSource = images[number].getImage();
        display->setPixmap(*imageCurrent);
        display->adjustSize();

        refreshListLayers(number);
    }
}

void MyWindow::changeLayer(int layer) {

    if(layer > -1) {
        if(layer == 0) {
            lblResol->setText(QString::number(imageSource->width()) + " x " + QString::number(imageSource->height()));
            *imageCurrent = *imageSource;
            display->setPixmap(*imageCurrent);
            display->adjustSize();
        }
        else {
            qreal koef = qPow(reductFactor, layer - 1);
            QPixmap *imageBuf = new QPixmap;
            QPixmap *imageBlur = new QPixmap;
            *imageBuf = imageSource->scaled(imageSource->width()/koef, imageSource->height()/koef,  Qt::IgnoreAspectRatio);
            *imageBlur = blur(*imageBuf);
            koef = qPow(reductFactor, layer);
            *imageBuf = imageBlur->scaled(imageSource->width()/koef, imageSource->height()/koef,  Qt::IgnoreAspectRatio);
            lblResol->setText(QString::number((int)(imageSource->width()/koef)) + " x " + QString::number((int)(imageSource->height()/koef)));
            *imageCurrent = imageBuf->scaled(imageBuf->width()*koef, imageBuf->height()*koef,  Qt::IgnoreAspectRatio);
            display->setPixmap(*imageCurrent);
            display->adjustSize();
            delete imageBuf;
            delete imageBlur;
        }
    }
}

void MyWindow::open() {

    QString fileName = QFileDialog::getOpenFileName(this, "Open file", qApp->applicationDirPath(), "Images (*.jpg *.png)");

    if(!QFile(fileName).exists())
        return;

    QPixmap bufQ;

    if(!bufQ.load(fileName))
        return;

    QString name = parser(fileName);

    int index = findIndex(name, fileName, images);

    if(index == images.size()) {
        // Add image
        Image buf(name, fileName, bufQ);
        images.push_back(buf);

        std::sort(images.begin(), images.end(), comp);

        refreshListFiles();

        index = findIndex(name, fileName, images);

        *imageCurrent = images[index].getImage();
        *imageSource = images[index].getImage();
        listFiles->setCurrentIndex(index);
        refreshListLayers(index);

        setImage();
    }
    else {
        // Image is exist
        *imageCurrent = images[index].getImage();
        *imageSource = images[index].getImage();
        listFiles->setCurrentIndex(index);
        refreshListLayers(index);

        setImage();
    }
}

bool comp(const Image &value1,  const Image &value2) {

    return value1.getDiag() < value2.getDiag();
}

QPixmap blur(QPixmap image) {

    QImage blur = image.toImage();
    QImage source = image.toImage();

    // blur.setP
    QMatrix3x3 kernel;
        kernel(0, 0) = 1; kernel(0, 1) = 2; kernel(0, 2) = 1;
        kernel(1, 0) = 2; kernel(1, 1) = 4; kernel(1, 2) = 2;
        kernel(2, 0) = 1; kernel(2, 1) = 2; kernel(2, 2) = 1;
        float kernel_sum = 16.0;


        for(int i=1; i<source.width()-1; i++) {
            for(int j=1; j<source.height()-1; j++) {
                float red = 0, green = 0, blue = 0;

                // *****************************************************
                red =
                        kernel(0, 0) * qRed(source.pixel(i+1, j+1)) +
                        kernel(0, 1) * qRed(source.pixel(i, j+1)) +
                        kernel(0, 2) * qRed(source.pixel(i-1, j+1)) +

                        kernel(1, 0) * qRed(source.pixel(i+1, j)) +
                        kernel(1, 1) * qRed(source.pixel(i, j)) +
                        kernel(1, 2) * qRed(source.pixel(i-1, j)) +

                        kernel(2, 0) * qRed(source.pixel(i+1, j-1)) +
                        kernel(2, 1) * qRed(source.pixel(i, j-1)) +
                        kernel(2, 2) * qRed(source.pixel(i-1, j-1));

                // *****************************************************
                green =
                        kernel(0, 0) * qGreen(source.pixel(i+1, j+1)) +
                        kernel(0, 1) * qGreen(source.pixel(i, j+1)) +
                        kernel(0, 2) * qGreen(source.pixel(i-1, j+1)) +

                        kernel(1, 0) * qGreen(source.pixel(i+1, j)) +
                        kernel(1, 1) * qGreen(source.pixel(i, j)) +
                        kernel(1, 2) * qGreen(source.pixel(i-1, j)) +

                        kernel(2, 0) * qGreen(source.pixel(i+1, j-1)) +
                        kernel(2, 1) * qGreen(source.pixel(i, j-1)) +
                        kernel(2, 2) * qGreen(source.pixel(i-1, j-1));

                // *****************************************************
                blue =
                        kernel(0, 0) * qBlue(source.pixel(i+1, j+1)) +
                        kernel(0, 1) * qBlue(source.pixel(i, j+1)) +
                        kernel(0, 2) * qBlue(source.pixel(i-1, j+1)) +

                        kernel(1, 0) * qBlue(source.pixel(i+1, j)) +
                        kernel(1, 1) * qBlue(source.pixel(i, j)) +
                        kernel(1, 2) * qBlue(source.pixel(i-1, j)) +

                        kernel(2, 0) * qBlue(source.pixel(i+1, j-1)) +
                        kernel(2, 1) * qBlue(source.pixel(i, j-1)) +
                        kernel(2, 2) * qBlue(source.pixel(i-1, j-1));

                blur.setPixel(i,j, qRgb(red/kernel_sum, green/kernel_sum, blue/kernel_sum));

            }
        }

    image = QPixmap::fromImage(blur);

    return image;
}

QString parser(QString str) {

    QStringList list = str.split('/');
    QString name = list.back();

    return name;
}

int findIndex(const QString name, const QString pathName, QVector<Image> &data) {

    int index;
    int i;

    for(i = 0; i < data.size(); i++) {
        if((data[i].getName() == name) && (data[i].getPathName() == pathName)) {
            index = i;
            break;
        }
    }

    if(i == data.size()) {
        index = data.size();
    }

    return index;
}
