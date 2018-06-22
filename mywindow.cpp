#include "mywindow.h"

// for test
qreal koefDecreas = 2;

Image::Image() {

}

Image::Image(QString _name, QPixmap _image) {
    name = _name;
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
            width /=koefDecreas;
            layers++;
        }
    }
    else {
        while(height > 0) {
            height /=koefDecreas;
            layers++;
        }
    }
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

MyWindow::MyWindow(QWidget *parent) : QWidget(parent) {

    imageCurrent = new QPixmap(500, 500);
    imageCurrent->fill(Qt::white);
    imageSource = new QPixmap;

    display = new QLabel;
    display->setPixmap(*imageCurrent);
    display->setAutoFillBackground(true);
    display->adjustSize();

    sa = new QScrollArea;
    sa->setMinimumHeight(502);
    sa->setMinimumWidth(502);
    sa->setAlignment(Qt::AlignCenter);
    sa->setWidget(display);

    listLayers = new QComboBox;
    listFiles = new QComboBox;

    infoSize = new QLabel;
    infoSize->setText(QString::number(imageCurrent->width()) + " x " + QString::number(imageCurrent->height()));

    QVBoxLayout *work = new QVBoxLayout;
    QHBoxLayout *interFace = new QHBoxLayout;

    openBut = new QPushButton("Open");

    interFace->addWidget(openBut);
    interFace->addWidget(listFiles);
    interFace->addWidget(listLayers);
    interFace->addWidget(infoSize);
    work->addWidget(sa);
    work->addItem(interFace);
    setWindowTitle("Pyramid");
    setLayout(work);

    connect(listLayers, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLayer(int)));
    connect(listFiles, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFile(int)));
    connect(openBut, SIGNAL(clicked(bool)), this, SLOT(open()));
}

void MyWindow::changeFile(int number) {

    if(number > -1) {
        *imageCurrent = images[number].getImage();
        *imageSource = images[number].getImage();
        display->setPixmap(*imageCurrent);
        display->adjustSize();

        //refresh listLayers
        listLayers->clear();

        for(int unsigned i = 0; i < images[number].getLayers(); i++) {
            listLayers->addItem(QString::number(i));
        }
    }
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


        for(int i=1; i<source.width()-1; i++)
        {
            for(int j=1; j<source.height()-1; j++)
            {
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

void MyWindow::changeLayer(int layer) {

    if(layer > -1) {
        if(layer == 0) {
            infoSize->setText(QString::number(imageSource->width()) + " x " + QString::number(imageSource->height()));
            *imageCurrent = *imageSource;
            display->setPixmap(*imageCurrent);
        }
        else {
            QPixmap blurSource;
            blurSource = blur(*imageSource);
            qreal koef = qPow(koefDecreas, layer);
            QPixmap *imageBuf = new QPixmap;
            *imageBuf = blurSource.scaled(blurSource.width()/koef, blurSource.height()/koef,  Qt::IgnoreAspectRatio);
            infoSize->setText(QString::number((int)blurSource.width()/(int)koef) + " x " + QString::number((int)blurSource.height()/(int)koef));
            *imageCurrent = imageBuf->scaled(imageBuf->width()*koef, imageBuf->height()*koef,  Qt::IgnoreAspectRatio);
            display->setPixmap(*imageCurrent);
            display->adjustSize();
            delete imageBuf;
        }
    }
}

void MyWindow::setImage(const QString &fileName) {

    imageCurrent->load(fileName);
    imageSource->load(fileName);

    Image buf(fileName, *imageCurrent);
    images.push_back(buf);

    display->setPixmap(*imageCurrent);
    sa->resize(imageSource->width(), imageSource->height());
    sa->setVisible(true);
    display->setAutoFillBackground(true);
    infoSize->setText(QString::number(imageCurrent->width()) + " x " + QString::number(imageCurrent->height()));
    display->setPixmap(*imageCurrent);
    display->adjustSize();
    sa->setWidget(display);
}

bool comp(const Image &value1,  const Image &value2) {

    return value1.getDiag() < value2.getDiag();
}

bool MyWindow::loadFile(const QString &fileName) {

    setImage(fileName);

    listLayers->clear();

    // refresh combobox
    for(int unsigned i = 0; i < images[currentNumberFiles].getLayers(); i++) {
        listLayers->addItem(QString::number(i));
    }

    std::sort(images.begin(), images.end(), comp);

    listFiles->clear();
    for(int i = 0; i < images.size(); i++) {
        listFiles->addItem(images[i].getName());
    }

    currentNumberFiles++;

    return true;
}

void MyWindow::open() {

    QString fileName = QFileDialog::getOpenFileName(this, "Open file", qApp->applicationDirPath(), "Images (*.jpg *.png)");
        if(!QFile(fileName).exists())
            return;

    setImage(fileName);

    listLayers->clear();

    // refresh combobox
    for(int unsigned i = 0; i < images[currentNumberFiles].getLayers(); i++) {
        listLayers->addItem(QString::number(i));
    }

    std::sort(images.begin(), images.end(), comp);

    listFiles->clear();
    for(int i = 0; i < images.size(); i++) {
        listFiles->addItem(images[i].getName());
    }

    currentNumberFiles++;
}
