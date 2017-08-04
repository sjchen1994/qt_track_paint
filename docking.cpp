#include "docking.h"
#include "ui_docking.h"

docking::docking(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::docking)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint);
}

docking::~docking()
{
    delete ui;
}



void docking::on_quit_released()
{
    this->close();
    emit SendSignal();
}
