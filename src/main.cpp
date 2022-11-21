#include <QMainWindow>
#include <QApplication>
#include <ScintillaEdit.h>
//#include "LexEditor.h"
#include "mainwindow.h"

int main(int argc, char*argv[]) {
    QApplication app(argc, argv);
    MainWindow win;

    win.show();

    return app.exec();
}
