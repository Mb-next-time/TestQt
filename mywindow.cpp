#include "mywindow.h"

Image::Image() {

}

Image::Image(QString _name, QPixmap _image) {
    name = _name;
    image = _image;
    width = _image.width();
    heigth = _image.height();
}

void Image::defineDiag() {
    diag = qPow((qPow(width, 2) + qPow(heigth,2)), (qreal)1/2);
}

void Image::defineLayers() {

    if(width <= heigth) {
        while(width != 0) {
            width /=2;
            layers++;
        }
    }
    else {
        while(heigth != 0) {
            heigth /=2;
            layers++;
        }
    }
}

QPixmap Image::getImage() {
    return image;
}
int unsigned Image::getWidth() {
    return width;
}

int unsigned Image::getHeigth() {
    return heigth;
}

int unsigned Image::getLayers() {
    return layers;
}
qreal Image::getDiag() {
    return diag;
}

MyWindow::MyWindow(QWidget *parent) : QWidget(parent) {

    imageCurent = new QPixmap(500, 500);
    imageCurent->fill(Qt::white);
    imageSource = new QPixmap;

    display = new QLabel;
    display->setPixmap(*imageCurent);
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

    *imageCurent = images[number].getImage();
    *imageSource = images[number].getImage();
    display->setPixmap(*imageCurrent);
    display->adjustSize();

    //refresh listLayers
    listLayers->clear();

    for(int unsigned i = 0; i < images[number].getLayers(); i++) {
        listLayers->addItem(QString::number(i));
    }
}

void MyWindow::changeLayer(int layer) {

    if(layer > -1) {
        qreal koef = qPow(2, layer);
        QPixmap *imageBuf = new QPixmap;
        *imageBuf = imageSource->scaled(imageSource->width()/koef, imageSource->height()/koef,  Qt::IgnoreAspectRatio);
        infoSize->setText(QString::number((int)imageSource->width()/(int)koef) + " x " + QString::number((int)imageSource->height()/(int)koef));
        *imageCurent = imageBuf->scaled(imageBuf->width()*koef, imageBuf->height()*koef,  Qt::IgnoreAspectRatio);
        display->setPixmap(*imageCurrent);
        display->adjustSize();
        delete imageBuf;
    }
}

void MyWindow::setImage(const QString &fileName) {

    imageCurrent->load(fileName);
    imageSource->load(fileName);

    Image buf(fileName, *imageCurent);
    buf.defineDiag();
    buf.defineLayers();
    images.push_back(buf);
    /*
    QMessageBox mess;
    mess.setText("Layers: " + QString::number(buf.getLayers()) + "\n" + "Diag: " + QString::number(buf.getDiag()));
    mess.exec();
    */
    display->setPixmap(*imageCurent);

    sa->resize(imageSource->width(), imageSource->height());
    sa->setVisible(true);
    display->setAutoFillBackground(true);
    infoSize->setText(QString::number(imageCurrent->width()) + " x " + QString::number(imageCurrent->height()));
    display->setPixmap(*imageCurrent);
    display->adjustSize();
    sa->setWidget(display);
}

bool MyWindow::loadFile(const QString &fileName) {

    setImage(fileName);

    listLayers->clear();

    // refresh combobox
    for(int unsigned i = 0; i < images[currenNumberImages].getLayers(); i++) {
        listLayers->addItem(QString::number(i));
    }

    listFiles->addItem(fileName);
    currentNumberFiles++;

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
