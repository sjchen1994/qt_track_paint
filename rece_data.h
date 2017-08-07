#ifndef RECE_DATA_H
#define RECE_DATA_H
#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
class rece_data : public QThread
{
public:
    rece_data();
    void ProTeminal();
    virtual void run();
    QProcess *pro;

    QString dir;
};

#endif // RECE_DATA_H
