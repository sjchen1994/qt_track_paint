#include "rece_data.h"

//--------------构造函数--------------//
rece_data::rece_data()
{
    pro = new QProcess;
}

//--------------线程主程序--------------//
void rece_data::run(){

    QStringList args;
    pro->startDetached("D:\\project\\test\\Debug\\test.exe");
    //QProcess::execute("D:\\project\\test\\Debug\\test.exe",args);
}

//--------------类析构函数--------------//
void rece_data::ProTeminal(){
    if(pro){
        pro->close();
        pro->write("quit");
        pro->kill();
        delete pro;
    }
}
