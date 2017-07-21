#ifndef DATAPROCESS_H
#define DATAPROCESS_H
#include <QTextStream>
#include <QtMath>
#include <QThread>
#include <QVector>
#include <QPointF>
#include <QLine>
#include <QColor>
#include <QTime>
#include <QMap>
#include <QDebug>
#include <QFile>
class DataProcess : public QThread
{
    Q_OBJECT
public:
    DataProcess();
    virtual void run();

    void GetPoint();
    void MainLoop();
    void AnalyzePoints();
    void Sleep(unsigned int msec);
    void QPointfSort(QVector<QPointF> &vpf);

    double x_world2axis(const double &tmp);
    double x_axis2world(const double &tmp);
    double y_world2axis(const double &tmp);
    double y_axis2world(const double &tmp);
    double AnalyzeStopPoints(QVector<QPointF> ana_points);
    double AnalyzeTrackPoints(QVector<QPointF> ana_points);
    double GetDistanceOfPoints(const QPointF* first, const QPointF* second);

    bool NearEdgeJudge(const QPointF* judge_point);
    bool IncludeJudge(int i, QMap<QString, QVector<QPointF> > &new_map, QMap<int,QVector<QPointF>> &update_stop_pointf, QMap<int, QVector<QPointF> > &update_all_stop_pointf, QPointF compare_point, int &change_id);


    QMap<QChar,QPointF> edge_pointf;  //边缘点
    QVector<QMap<int,QVector<QPointF>>> all_stop_pointf;
    QVector<QMap<int,QVector<QPointF>>> stop_pointf;
    QVector<QMap<QString,QVector<QPointF>>> track_pointf;//存储中间轨迹点
    QVector<QMap<int,QVector<QPointF>>> last_stop_pointf;
    QVector<QVector<QPointF>> g_pointf;            //存储所有点坐标
    QVector<QVector<QPointF>> show_g_pointf;  //用于绘制的点集
    int clear_pushed = 0;                     //标记是否点击过清空按钮（清空坐标容器）（会重置）
    int data_clear = 0;                       //标记是否点击过清空按钮（用在点击后有新坐标加入时）（会重置）
    int ever_clear = 0;                       //标记是否曾经点击过清空按钮（不会重置）
signals:
    void send_data(const QVector<QVector<QPointF>> g_pointf);
};

#endif // DATAPROCESS_H
