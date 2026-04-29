#include "turingmachine.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TuringMachine window;
    window.show();

    return QApplication::exec();
}