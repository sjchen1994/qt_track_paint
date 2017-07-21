#ifndef TRACKING_H
#define TRACKING_H

#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QtMath>

#include <QMouseEvent>
#include <QMetaType>
#include <dataprocess.h>
#include <rece_data.h>
#include <wechat_push.h>
namespace Ui {
class tracking;
}

class tracking : public QMainWindow
{
    Q_OBJECT

public:
    explicit tracking(QWidget *parent = 0);
    ~tracking();

    void Init();
    void paintEvent(QPaintEvent *);
    void PaintPointf(const QVector<QVector<QPointF> > &tmp_points);
    void MyDrawArrow(const QPointF& pt1, const QPointF& pt2, QPainter &p);

private slots:
    void ReceiveData(const QVector<QVector<QPointF>> g_pointf);

    void on_ScreenShot_triggered();

    void on_quit_triggered();

    void on_WechatPush_triggered();

    void on_TrackAnalyze_triggered();

    void on_TrackConnect_triggered();

    void on_InterfaceClear_triggered();

    void on_txt_clear_triggered();

    void on_rece_data_triggered();

private:
    QPoint last;
    rece_data rd;
    wechat_push wp;
    DataProcess dp;
    QImage illu_image;
    QImage axis_image;
    Ui::tracking *ui;
protected:
    void mousePressEvent(QMouseEvent *e);

    void mouseMoveEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // TRACKING_H
