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
#include <QMutex>
#include <mytools.h>
class DataProcess : public QThread
{
    Q_OBJECT
public:
    DataProcess();
    virtual void run();

    void GetPoint();
    void MainLoop();
    void AnalyzePoints();
    void QPointfSort(QVector<QPointF> &vpf);
    QVector<QPointF> QPointfOrderSort(const QVector<QPointF> &vpf, const QString &key, const QMap<int, QVector<QPointF> > &stop_pointf);
    QVector<QPointF> QPointfOrderSortForJumpInsert(const QVector<QPointF> &vpf, int nowid, const QMap<int, QVector<QPointF> > &stop_pointf);
    double X_World2Axis(const double &tmp);
    double X_Axis2World(const double &tmp);
    double Y_World2Axis(const double &tmp);
    double Y_Axis2World(const double &tmp);
    void JumpPointInsert(QVector<QPointF> inserting_points, int nowid, const QMap<int, QVector<QPointF> > &stop_pointf, QMap<QString,QVector<QPointF>>& push_track_pointf);
    double AnalyzeStopPoints(QVector<QPointF> ana_points);
    double AnalyzeTrackPoints(QVector<QPointF> ana_points);
    void MapPointSort(QMap<int, QVector<QPointF> > &sorting_points, int find_id);
    void MapTrackSort(QMap<QString, QVector<QPointF>>& sorting_track, QVector<QPointF> insert_points1, QVector<QPointF> insert_points2, int find_id);
    double GetDistanceOfPoints(const QPointF* first, const QPointF* second);

    bool NearEdgeJudge(const QPointF* judge_point);
    bool NewPointSort(int i, QMap<QString, QVector<QPointF> > &new_map, QMap<int,QVector<QPointF>> &update_stop_pointf, QMap<int, QVector<QPointF> > &update_all_stop_pointf, QMap<int,QVector<QPointF>>& update_last_stop_pointf, QPointF compare_point, int &change_id);



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
    int quit_flag = 0;
    QString txt_path = "D:\\project\\test\\debug";
signals:
    void Senddata(const QVector<QVector<QPointF>> g_pointf);
private:

    MyTools tools;
};

#endif // DATAPROCESS_H
