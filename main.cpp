#include <QCoreApplication>
#include "backend.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Backend back;

    return a.exec();
}
