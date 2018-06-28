#include <QApplication>
#include "mywindow.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    double value;
    QString path;

    if(argc == 3) {
        QString valueStr(argv[1]);
        value = valueStr.toDouble();
        if(value == 0) {
            QMessageBox mess;
            mess.setText("Соблюдайте порядок ввода входных параметров, сначала\nкоэффициент уменьшения, затем путь к изображению");
            mess.exec();
        }
        path.push_back(argv[2]);
    }
    else {
        if (argc == 1) {
            value = 0;
            path.push_back("\0");
        }
        else {
            QMessageBox mess;
            mess.setText("Неверный ввод входных параметров");
            mess.exec();
            value = 0;
            path.push_back("\0");
        }
    }

    MyWindow *window = new MyWindow(value, path);
    window->show();

    return a.exec();
}
