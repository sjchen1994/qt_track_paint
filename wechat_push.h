#ifndef WECHAT_PUSH_H
#define WECHAT_PUSH_H
#include <QThread>
#include <QProcess>
class wechat_push : public QThread
{
public:
    wechat_push();
    virtual void run();
    void ProTeminal();
private:
    QProcess *pro;
};

#endif // WECHAT_PUSH_H
