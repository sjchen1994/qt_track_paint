#ifndef RECE_DATA_H
#define RECE_DATA_H
#include <QThread>
#include <QProcess>

class rece_data : public QThread
{
public:
    rece_data();
    void ProTeminal();
    virtual void run();
private:
    QProcess *pro;
};

#endif // RECE_DATA_H
