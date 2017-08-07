#include "docking.h"
#include "ui_docking.h"

docking::docking(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::docking)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint);
    tcp_process_enable = 0;
    server = new QTcpServer;

    ui->state->clear();
    ui->run->setEnabled(false);
    ui->continue_run->setEnabled(false);
    ui->stop->setEnabled(false);
    ui->stop_point_view->setColumnCount(3);
    ui->stop_point_view->setHorizontalHeaderLabels(QStringList() << "In or Out" << "Std deviation" << "Num of sp");
    ui->stop_point_view->setColumnWidth(0, 65);
    ui->stop_point_view->setColumnWidth(1, 100);
    ui->stop_point_view->setColumnWidth(2, 120);
    ui->stop_point_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->stop_point_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->stop_point_view->verticalHeader()->setVisible(false);
    ui->stop_point_view->setShowGrid(false);
    ui->stop_point_view->setFocusPolicy(Qt::NoFocus);


    connect(server, SIGNAL(newConnection()), this, SLOT(AcceptConn()));
    connect(ui->tcp_conn, SIGNAL(released()), ui->actionTCP, SLOT(trigger()));

}

docking::~docking()
{
    delete ui;
}

//对获取的信息进行判断
void docking::MainProcess(){
    GetPoint(rece_message);
}

//读取坐标
void docking::GetPoint(QString flag){
    //变量定义
    QVector<QPointF> tmp_points;
    QPointF tmp_point;
    QProcess pro;
    QStringList list;
    double tmp_value;
    QString data;
    char* return_message = "roger that";
    QString readline_data;
    QString dir = "D:\\project\\detectFordock\\Debug\\test.exe";;
    //读取坐标

    pro.start(dir, list);
    QFile file("D:\\project\\detectFordock\\Debug\\dock1.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    while(true){
        QTextStream stream(&file);
        readline_data = stream.readLine(0);
        if(readline_data != ""){
            QString::iterator it = readline_data.begin();
            for(;it != readline_data.end(); it++){
                if(*it == ','){
                    tmp_value = data.toDouble();
                    tmp_value = dp.X_World2Axis(tmp_value);
                    tmp_point.setX(tmp_value);
                    data.clear();
                    continue;
                }
                else if(*it == " "){
                    tmp_value = data.toDouble();
                    tmp_value = dp.Y_World2Axis(tmp_value);
                    tmp_point.setY(tmp_value);
                    data.clear();
                }
                else{
                    data += *it;
                }
            }
            tmp_points.push_back(tmp_point);
            data.clear();
        }
        if(tmp_points.size() > 5){  //至少30个坐标
            break;
        }
    }
    file.close();
    pro.kill();
    //对坐标取平均值，插入对应容器
    if(flag == "isIn"){
        In_points.push_back(GetAvgPoint(tmp_points));
    }
    else if (flag == "isOut") {
        Out_points.push_back(GetAvgPoint(tmp_points));
    }
    else{
        return_message = "wrong message send!";
    }

    //返回确认收到信息
    ClientConn->write(return_message);

    //清空txt
    file.open(QIODevice::WriteOnly);
    file.close();
}

QPointF docking::GetAvgPoint(QVector<QPointF>& points){
    if(points.empty()){
        return QPointF(0, 0);
    }
    QPointF SumOfPoints = QPointF(0, 0);
    int size = points.size();
    for(QPointF point : points){
        SumOfPoints += point;
    }

    return SumOfPoints / size;

}

void docking::on_quit_released()
{
    this->close();
    emit SendSignal();
}

void docking::AcceptConn(){
    ClientConn = server->nextPendingConnection();
    connect(ClientConn, SIGNAL(readyRead()), this, SLOT(ReadClient()));
    ui->state->setText("TCP connected!");
    ui->run->setEnabled(true);
}

void docking::ReadClient(){
    QString str = ClientConn->readAll();
    rece_message = str;
    if(tcp_process_enable == 1){
        MainProcess();
    }
    else{
        ClientConn->write("server not ready!");
    }
}

void docking::on_actionTCP_triggered()
{
    server->listen(QHostAddress::Any, 6665);
}

void docking::on_analyze_released()
{
    if(In_points.empty() || Out_points.empty()){
        return;
    }
    tcp_process_enable = 0;
    ui->stop_point_view->setRowCount(2);
    ui->stop_point_view->setItem(0, 0, new QTableWidgetItem(QString("In")));
    ui->stop_point_view->setItem(0, 1, new QTableWidgetItem(QString::number( dp.AnalyzeStopPoints(In_points) )));
    ui->stop_point_view->setItem(0, 2, new QTableWidgetItem(QString::number(In_points.size())));

    ui->stop_point_view->setItem(1, 0, new QTableWidgetItem(QString("Out")));
    ui->stop_point_view->setItem(1, 1, new QTableWidgetItem(QString::number( dp.AnalyzeStopPoints(Out_points) )));
    ui->stop_point_view->setItem(1, 2, new QTableWidgetItem(QString::number(Out_points.size())));
}

void docking::on_run_released()
{
    tcp_process_enable = 1;
    ui->run->setEnabled(false);
    ui->stop->setEnabled(true);
}

void docking::on_continue_run_released()
{
    ui->continue_run->setEnabled(false);
    ui->stop->setEnabled(true);
    tcp_process_enable = 1;
}

void docking::on_stop_released()
{
    ui->continue_run->setEnabled(true);
    ui->stop->setEnabled(false);
    tcp_process_enable = 0;
}
