#include "welcome.h"
#include "ui_welcome.h"

welcome::welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::welcome)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint); //设置窗口总是最前
    this->setFixedSize(200, 150);                                //设置窗口固定大小

}

welcome::~welcome()
{
    delete ui;
}

void welcome::on_call_tracking_released()                  //打开tracking窗口
{
    this->hide(); //隐藏主窗口
    track = new tracking;
    connect(track, SIGNAL(SendSignal()), this, SLOT(reshow())); //连接子窗口和主窗口信号，reshow()用于返回主窗口
    track->show();                                                    //显示子窗口
}

void welcome::reshow(){
    this->show();                                                     //显示主窗口
}

void welcome::on_call_docking_released()                 //打开docking窗口
{
    this->hide();                                                     //隐藏主窗口
    dock = new docking;
    connect(dock, SIGNAL(SendSignal()), this, SLOT(reshow()));//连接子窗口和主窗口信号，reshow()用于返回主窗口
    dock->show();                                                   //显示子窗口
}
