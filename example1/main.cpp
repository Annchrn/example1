#include "mainwindow.h"
#include "windowcontroller.h"

#include <QApplication>

#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);
    MainWindow w;
    WindowController controller(&w); // создаём контроллер
    w.show();
    w.setWindowTitle("Визуализация лог-файла");
    return a.exec();
}
