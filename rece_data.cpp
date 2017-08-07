#include "rece_data.h"

//--------------构造函数--------------//
rece_data::rece_data()
{
    pro = new QProcess;
}

//--------------线程主程序--------------//
void rece_data::run(){

    QStringList list;
    pro->start(dir, list);
}

//--------------类析构函数--------------//
void rece_data::ProTeminal(){
    if(pro){
       pro->kill();

       /*qDebug()<<"before teminate";
       this->terminate();
       this->wait();
       qDebug() <<" after teminate";*/
   }
}


