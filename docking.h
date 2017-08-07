#ifndef DOCKING_H
#define DOCKING_H
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <dataprocess.h>
#include <rece_data.h>

namespace Ui {
class docking;
}

class docking : public QMainWindow
{
    Q_OBJECT

public:
    explicit docking(QWidget *parent = 0);
    ~docking();

    QPointF GetAvgPoint(QVector<QPointF>& points);
    void MainProcess();

    void GetPoint(QString flag);//获取当前小车坐标
signals:
    void SendSignal();
private slots:
    void AcceptConn();

    void ReadClient();

    void on_quit_released();

    void on_actionTCP_triggered();

    void on_analyze_released();

    void on_run_released();

    void on_continue_run_released();

    void on_stop_released();

private:
    QTcpServer* server;
    QTcpSocket* ClientConn;
    QVector<QPointF> In_points; //存储坐标
    QVector<QPointF> Out_points;
    QString rece_message;
    rece_data rd_thread;
    int tcp_process_enable;
    DataProcess dp;
    Ui::docking *ui;
};

#endif // DOCKING_H
