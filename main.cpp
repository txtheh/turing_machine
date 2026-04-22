#include "turingmachine.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TuringMachine w;
    w.show();
    return QCoreApplication::exec();
}
