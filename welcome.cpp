#include "welcome.h"
#include "ui_welcome.h"

welcome::welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::welcome)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setFixedSize(200, 150);

}

welcome::~welcome()
{
    delete ui;
}

void welcome::on_call_tracking_released()
{
    this->hide();
    track = new tracking;
    connect(track, SIGNAL(SendSignal()), this, SLOT(reshow()));
    track->show();
}

void welcome::reshow(){
    this->show();
}

void welcome::on_call_docking_released()
{
    this->hide();
    dock = new docking;
    connect(dock, SIGNAL(SendSignal()), this, SLOT(reshow()));
    dock->show();
}
