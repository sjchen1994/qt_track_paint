#include "tracking.h"
#include <QApplication>
#include <QMetaType>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<QVector<QPointF>> >("QVector<QVector<QPointF>>");//注册类型，使其能用信号-槽来传输
    tracking w;
    w.show();
    return a.exec();
}
