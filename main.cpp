#include "welcome.h"
#include <QApplication>
#include <QMetaType>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<QVector<QPointF>> >("QVector<QVector<QPointF>>");//注册类型，使其能用信号-槽来传输
    welcome w;
    w.show();
    return a.exec();
}
