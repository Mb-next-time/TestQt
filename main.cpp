#include <QApplication>
#include "mywindow.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    MyWindow *window = new MyWindow();
    window->show();

    return a.exec();
}
