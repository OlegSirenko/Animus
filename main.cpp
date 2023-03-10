#include "mainwindow.h"
#include <gst/gst.h>
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Animus_v_1.0-Linux");
    w.show();

    return a.exec();
}


