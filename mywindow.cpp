#include "mywindow.h"

Image::Image() {

}

Image::Image(QString _name, QString _pathName, QPixmap _image) {
    name = _name;
    pathName = _pathName;
    image = _image;
    width = _image.width();
    height = _image.height();
    defineDiag();
}

void Image::defineDiag() {
    diag = qPow((qPow(width, 2) + qPow(height,2)), (qreal)1/2);
}

void Image::defineLayers(qreal reductFactor, int _width, int _height) {

    layers = 0;

    while((_width > 0) && (_height > 0)) {
        _width /= reductFactor;
        _height /= reductFactor;
        layers++;
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

    int Width = 502;
    int Height = 502;

    display->setPixmap(*imageCurrent);
    sa->setMinimumHeight(Height);
    sa->setMinimumWidth(Width);
    sa->setAlignment(Qt::AlignCenter);
    sa->resize(imageSource->width(), imageSource->height());
    sa->setVisible(true);
    display->setAutoFillBackground(true);
    lblResol->setText(QString::number(imageCurrent->width()) + " x " + QString::number(imageCurrent->height()));
    display->setPixmap(*imageCurrent);
    display->adjustSize();
    sa->setWidget(display);
}

MyWindow::MyWindow(double value, QString fileName) {

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
    lblFile = new QLabel("File");
    listFiles = new QComboBox;
    lblSize = new QLabel("Size:");
    lblResol = new QLabel;
    lblResol->setMaximumSize(QSize(70, 20));
    lblResol->setMinimumSize(QSize(60, 20));
    openBut = new QPushButton("Open");
    lblReductFact = new QLabel("Reduction factor");
    spinBoxReductFact = new QDoubleSpinBox;

    qreal minSP = 1.1;
    qreal maxSP = 2.0;
    qreal stepSP = 0.1;
    qreal decimalsSP = 1.0;
    qreal initValSP;

    if((value < minSP) || (value > maxSP)) {
        initValSP = 2.0;
    }
    else {
        initValSP = value;
    }

    spinBoxReductFact->setMinimum(minSP);
    spinBoxReductFact->setMaximum(maxSP);
    spinBoxReductFact->setSingleStep(stepSP);
    spinBoxReductFact->setDecimals(decimalsSP);
    spinBoxReductFact->setValue(initValSP);
    spinBoxReductFact->setMaximumSize(QSize(100, 20));

    QVBoxLayout *work = new QVBoxLayout;
    QHBoxLayout *interFace = new QHBoxLayout;

    interFace->addWidget(openBut);
    interFace->addWidget(lblFile);
    interFace->addWidget(listFiles);
    interFace->addWidget(lblLayers);
    interFace->addWidget(listLayers);
    interFace->addWidget(lblSize);
    interFace->addWidget(lblResol);
    interFace->setSpacing(5);
    interFace->setStretch(0, 5);
    interFace->setStretch(2, 10);

    work->addWidget(sa);
    work->addItem(interFace);
    work->addWidget(lblReductFact);
    work->addWidget(spinBoxReductFact);

    setWindowTitle("Pyramid");
    setLayout(work);
    QSize sizeWind(510, 510);
    setMaximumSize(sizeWind);

    if(!fileName.isEmpty()) {
        openConsole(fileName);
    }

    setImage();

    connect(listLayers, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLayer(int)));
    connect(listFiles, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFile(int)));
    connect(openBut, SIGNAL(clicked(bool)), this, SLOT(open()));
    connect(spinBoxReductFact, SIGNAL(valueChanged(double)), this, SLOT(changeReductFactor()));
}

void MyWindow::refreshListLayers(int index) {

    listLayers->clear();

    images[index].defineLayers(getReductFactor(),images[index].getWidth(), images[index].getHeight() );

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
        QPixmap *imageBuf = new QPixmap;
        qreal koef = qPow(getReductFactor(), layer);
        int countLayers = 0;
        *imageCurrent = *imageSource;
        while(countLayers != layer) {
            *imageBuf = blur(*imageCurrent);
            *imageCurrent = imageCurrent->scaled(imageBuf->width()/getReductFactor(), imageBuf->height()/getReductFactor(),  Qt::IgnoreAspectRatio);
            countLayers++;
        }
        *imageCurrent = imageCurrent->scaled(imageCurrent->width()*koef, imageCurrent->height()*koef,  Qt::IgnoreAspectRatio);
        lblResol->setText(QString::number((int)(imageSource->width()/koef)) + " x " + QString::number((int)(imageSource->height()/koef)));
        display->setPixmap(*imageCurrent);
        display->adjustSize();
        delete imageBuf;
    }
}

void MyWindow::openConsole(QString fileName) {

    if(!QFile(fileName).exists())
        return;

    QPixmap bufPixmap;

    if(!bufPixmap.load(fileName))
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();

    if((ext == "jpg") || (ext == "png")) {

        QString name = parser(fileName);

        // Add image
        Image bufImage(name, fileName, bufPixmap);
        images.push_back(bufImage);

        refreshListFiles();

        int index = 0;

        *imageCurrent = images[index].getImage();
        *imageSource = images[index].getImage();
        listFiles->setCurrentIndex(index);
        refreshListLayers(index);
        setImage();
    }
    else {
        QMessageBox mess;
        mess.setText("Выберите поддерживаемый формат приложения jpg или png");
        mess.exec();
    }
}

void MyWindow::open() {

    QString fileName = QFileDialog::getOpenFileName(this, "Open file", qApp->applicationDirPath(), "Images (*.jpg *.png)");

    if(!QFile(fileName).exists())
        return;

    QPixmap bufPixmap;

    if(!bufPixmap.load(fileName))
        return;


    QString name = parser(fileName);

    int index = findIndex(name, fileName, images);

    if(index == images.size()) {
        // Add image
        Image bufImage(name, fileName, bufPixmap);
        images.push_back(bufImage);

        std::sort(images.begin(), images.end(), comp);

        refreshListFiles();

        index = findIndex(name, fileName, images);
        *imageCurrent = images[index].getImage();
        *imageSource = images[index].getImage();
        listFiles->setCurrentIndex(index);

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

void MyWindow::changeReductFactor() {

    if(listFiles->currentIndex() > -1){
        refreshListLayers(listFiles->currentIndex());
    }
}

bool comp(const Image &value1,  const Image &value2) {

    return value1.getDiag() < value2.getDiag();
}

QPixmap blur(QPixmap image) {

    QImage blur = image.toImage();
    QImage source = image.toImage();

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
