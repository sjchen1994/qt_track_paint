#include "rece_data.h"
//QProcess *pro = new QProcess;
//--------------构造函数--------------//
rece_data::rece_data()
{
    pro = new QProcess;
}

//--------------线程主程序--------------//
void rece_data::run(){
    //QProcess *p = new QProcess;
   // connect(pro,SIGNAL(readyRead()),this,SLOT(on_read()));
    QStringList list;
    pro->start("D:\\project\\test\\Debug\\test.exe",list);

    /*while(true){
        QByteArray ba = p->readLine();
        if(ba.isEmpty()){
            qDebug()<<"1";
        }
    }
*/
}

//--------------类析构函数--------------//
void rece_data::ProTeminal(){
    if(pro){
       pro->kill();
        this->quit();
   }
}

/*void rece_data::on_read(){
    QProcess *p = (QProcess*)sender();
    QString result = p->readAll();
    QMessageBox::warning(NULL,"",result);
}*/
