#include "wechat_push.h"

//--------------构造函数--------------//
wechat_push::wechat_push()
{
    pro = new QProcess;
}

//--------------线程主程序--------------//
void wechat_push::run(){
    QStringList args;
    args.append("C:\\Python27\\project\\itchat1.py");
    pro->start("python.exe", args);
}

//--------------类析构函数--------------//
void wechat_push::ProTeminal(){
    if(pro){
        pro->kill();
        this->quit();
    }
}
