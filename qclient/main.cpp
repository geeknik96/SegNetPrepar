//
// Created by Nikita Somenkov on 14/06/16.
//

#include <QApplication>

#include "MainWindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

