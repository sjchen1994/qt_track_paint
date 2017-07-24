#include "tracking.h"
#include "ui_tracking.h"

QColor v_color[7] = {Qt::black, Qt::cyan, Qt::magenta, Qt::green, Qt::yellow, Qt::blue, Qt::gray};
QColor s_color[7] = {Qt::red, Qt::darkCyan, Qt::darkMagenta, Qt::darkGreen, Qt::darkYellow, Qt::darkBlue, Qt::darkGray};

tracking::tracking(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tracking)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint);

    //界面控件初始化

    ui->stop_stat_view1->setColumnCount(4);
    ui->stop_stat_view1->setHorizontalHeaderLabels(QStringList() << "Num" << "Std deviation" << "Avg point" << "Num of sp");
    ui->stop_stat_view1->setColumnWidth(0, 35);
    ui->stop_stat_view1->setColumnWidth(1, 100);
    ui->stop_stat_view1->setColumnWidth(2, 120);
    ui->stop_stat_view1->setColumnWidth(3, 100);
    ui->stop_stat_view1->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->stop_stat_view1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->stop_stat_view1->verticalHeader()->setVisible(false);
    ui->stop_stat_view1->setShowGrid(false);
    ui->stop_stat_view1->setFocusPolicy(Qt::NoFocus);

    ui->track_stat_view1->setColumnCount(2);
    ui->track_stat_view1->setHorizontalHeaderLabels(QStringList()<< "Num" << "Std deviation");
    ui->track_stat_view1->setColumnWidth(0, 35);
    ui->track_stat_view1->setColumnWidth(1, 100);
    ui->track_stat_view1->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->track_stat_view1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->track_stat_view1->verticalHeader()->setVisible(false);
    ui->track_stat_view1->setShowGrid(false);
    ui->track_stat_view1->setFocusPolicy(Qt::NoFocus);

    //画布填充
    illu_image = QImage(187.5, 75, QImage::Format_RGB32);
    axis_image = QImage(1875, 750, QImage::Format_RGB32);
    QColor backgColor = qRgb(255, 255, 255);
    illu_image.fill(backgColor);
    axis_image.fill(backgColor);

    //界面初始化
    Init();

    //数据处理线程开始
    dp.start();

    //信号连接
    connect(ui->screenshot_button,    SIGNAL(released()),   ui->ScreenShot,    SLOT(trigger()));
    connect(ui->connect_line_button,  SIGNAL(released()),   ui->TrackConnect, SLOT(trigger()));
    connect(ui->analyze_button,         SIGNAL(released()),  ui->TrackAnalyze,   SLOT(trigger()));
    connect(ui->clear_interface,          SIGNAL(released()),  ui->InterfaceClear, SLOT(trigger()));
    connect(ui->wechat_push_button,  SIGNAL(released()),  ui->WechatPush,   SLOT(trigger()));
    connect(ui->window_quit,             SIGNAL(released()),  ui->quit,              SLOT(trigger()));
    connect(ui->txt_clear_button,       SIGNAL(released()),  ui->txt_clear,       SLOT(trigger()));

    connect(&dp, SIGNAL(Senddata(QVector<QVector<QPointF>>)), this,SLOT(ReceiveData(QVector<QVector<QPointF>>)));
}

//--------------构造函数--------------//
tracking::~tracking()
{
    delete ui;
}

//--------------初始化函数--------------//
void tracking::Init(){

    //画布，坐标轴部分
    QPainter mypainter(&axis_image);
    int point0x = 61, point0y = 727.5;//坐标轴原点
    int pointx = 25.5,pointy = 79.77;
    int width = 1811.25,height = 712.5;//坐标轴宽度，高度
    mypainter.drawLine(0,749,1875,749);//外围矩形
    QPainter mypainter1(&illu_image);
    mypainter1.drawRect(0, 0, 186.5, 74.5);
    //y
    mypainter.drawLine(point0x, point0y-height, point0x, point0y);
    //x
    mypainter.drawLine(pointx, pointy, pointx+width, pointy);

    QPen pendegree;
    pendegree.setColor(Qt::black);
    pendegree.setWidth(2);
    mypainter.setPen(pendegree);
    //x轴刻度
    for(int i = 0; i < 30; i++){
        mypainter.drawLine( pointx + (i + 1) * width / 30, pointy, pointx + (i + 1) * width / 30, pointy + 4);
        mypainter.drawText(pointx + (i + 0.65) * width / 30, pointy + 13, QString::number(-100 + (i + 1) * 170));
    }
    //y轴刻度
    for(int i = 0; i < 20; i++){
        mypainter.drawLine(point0x, point0y - i * height / 20, point0x + 4, point0y - i * height / 20);
        mypainter.drawText(point0x - 22, point0y - (i - 0.15) * height / 20, QString::number(2000 - i * 110));
    }
}

//--------------画布事件函数--------------//
void tracking::paintEvent(QPaintEvent*){
    QPainter painter(this);
    painter.drawImage(0, 0, axis_image);
    painter.drawImage(60, 875, illu_image);

}

//--------------描点函数--------------//
void tracking::PaintPointf(const QVector<QVector<QPointF> > &tmp_points){
    QPainter paintpoint(&axis_image);
    QPen pen;
    pen.setWidth(2);
    int t = 0;
    for(QVector<QPointF> spe_id_pointf : tmp_points){
        pen.setColor(v_color[t]);
        paintpoint.setPen(pen);
        paintpoint.drawPoints(spe_id_pointf);
        t++;
        this->repaint();
    }
}

//--------------画箭头函数--------------//
void tracking::MyDrawArrow(const QPointF& pt1, const QPointF& pt2, QPainter &p){

    p.drawLine(pt1,pt2);
    double x1 = pt2.x() - 5 * cos(atan2(pt2.y() - pt1.y(), pt2.x() - pt1.x()) - 0.4);
    double y1 = pt2.y() - 5 * sin(atan2(pt2.y() - pt1.y(), pt2.x() - pt1.x()) - 0.4);
    double x2 = pt2.x() - 5 * sin(atan2(pt2.x() - pt1.x(), pt2.y() - pt1.y()) - 0.4);
    double y2 = pt2.y() - 5 * cos(atan2(pt2.x() - pt1.x(), pt2.y() - pt1.y()) - 0.4);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);
    p.setPen(pen);
    p.drawLine(pt2.x(),pt2.y(),x1,y1);
    p.drawLine(pt2.x(),pt2.y(),x2,y2);
    pen.setWidth(1);
    p.setPen(pen);
}

//--------------主线程SLOT函数--------------//
void tracking::ReceiveData(const QVector<QVector<QPointF> > g_pointf){//接受数据处理进程传送的数据
    PaintPointf(g_pointf);
}

//--------------截图按钮事件--------------//
void tracking::on_ScreenShot_triggered()
{
    QPixmap save_image = QPixmap::grabWidget(this);
    save_image.save("11.png","png");
}

//--------------退出按钮事件--------------//
void tracking::on_quit_triggered()
{
    rd.ProTeminal();
    rd.quit();
    //rd.terminate();
    dp.quit();
    dp.terminate();//停止数据处理线程
    this->close();
}

//--------------微信按钮事件--------------//
void tracking::on_WechatPush_triggered()
{
    wp.start();
}

//--------------数据分析按钮事件--------------//
void tracking::on_TrackAnalyze_triggered()
{
    //分析计算函数
    dp.AnalyzePoints();
    //-----------------------分析点坐标集合  输出分析结果-------------------//
    /*  分析对象：
     * 1:QVector<QMap<int,QVector<QPointF>>> stop_pointf      : 所有到点坐标（形式：id-value）
     * 2:QVector<QMap<QString,QVector<QPointF>>> track_pointf : 所有轨迹坐标    示例: "12",(558.221,241.704)...  指  id为 1 2 之间的轨迹坐标集
     * 3:画出示意图
     * 4:得到停止点的平均点
    */
    //1
    auto ana_it = dp.last_stop_pointf.begin();
    int current_rowcount;
    for(; ana_it != dp.last_stop_pointf.end(); ana_it++){
        for(int ana_id = 1; ana_it->find(ana_id) != ana_it->end(); ana_id++){
            current_rowcount = ui->stop_stat_view1->rowCount();
            ui->stop_stat_view1->setRowCount(current_rowcount+1);
            ui->stop_stat_view1->setItem(current_rowcount,0,new QTableWidgetItem( QString::number(ana_id)));
            ui->stop_stat_view1->setItem(current_rowcount,1,new QTableWidgetItem( QString::number( qSqrt( dp.AnalyzeStopPoints(ana_it->find(ana_id).value())) ) ));
            //qDebug()<< analyze_stop_point(ana_it->find(ana_id).value());
        }
    }

    //2
    for(auto ana_tra_it = dp.track_pointf.begin(); ana_tra_it != dp.track_pointf.end(); ana_tra_it++,ana_it++){
        for(auto it_tra = ana_tra_it->begin(); it_tra != ana_tra_it->end(); it_tra++){
            current_rowcount = ui->track_stat_view1->rowCount();
            ui->track_stat_view1->setRowCount(current_rowcount+1);
            ui->track_stat_view1->setItem(current_rowcount,0,new QTableWidgetItem(it_tra.key()));
            ui->track_stat_view1->setItem(current_rowcount,1,new QTableWidgetItem(QString::number(qSqrt(dp.AnalyzeTrackPoints(it_tra.value())))));
        }
    }

    //3
    //int IsInTrack;
    QMap<QString,QPointF> indi_pointf;
    for(auto ana_tra_it = dp.track_pointf.begin(); ana_tra_it != dp.track_pointf.end(); ana_tra_it++,ana_it++){
        for(auto it_tra = ana_tra_it->begin(); it_tra != ana_tra_it->end(); it_tra++){
            indi_pointf.insert(QString::number(it_tra.key().toInt()/10),it_tra.value().first());
            for(auto edge_it = dp.edge_pointf.begin(); edge_it != dp.edge_pointf.end(); edge_it++){//abc
                for(auto it_tra_pt = it_tra.value().begin(); it_tra_pt != it_tra.value().end(); it_tra_pt++ ){//12
                    if(dp.GetDistanceOfPoints(&edge_it.value(),it_tra_pt) < 100){
                        indi_pointf.insert(edge_it.key(),*edge_it);
                        break;
                    }
                }
            }
        }
    }

    QPainter illu_paint(&illu_image);
    QPen illu_pen;
    illu_pen.setWidth(1);
    illu_pen.setColor(Qt::black);
    illu_paint.setPen(illu_pen);

    for(auto it1 = indi_pointf.begin(), it2 = indi_pointf.begin(); it1 != indi_pointf.end(); it1++){
        QPointF pt = it1.value()/10;
        illu_paint.drawPoint(pt);
        illu_paint.drawText(pt - QPointF(0,5),it1.key());
        if(it2+1 != indi_pointf.end()){
            it2++;
        }
        else{
            it2 = indi_pointf.begin();
        }
        MyDrawArrow(pt,(it2).value()/10,illu_paint);
    }

    //4
    QPointF show_pointf;
    QVector<QMap<int,QVector<QPointF>>> pp = dp.all_stop_pointf;
    int all_size = 0;
    int insert_rowcount = 0;
    for(auto ana_all_it = dp.all_stop_pointf[0].begin(); ana_all_it != dp.all_stop_pointf[0].end(); ana_all_it++){
        for(auto sub_all_it = ana_all_it.value().begin(); sub_all_it != ana_all_it.value().end(); sub_all_it++){
            show_pointf += (*sub_all_it);
            all_size = ana_all_it.value().size();
        }
        show_pointf = show_pointf / all_size;
        ui->stop_stat_view1->setItem(insert_rowcount,2,new QTableWidgetItem(QString::number(dp.X_Axis2World((show_pointf.x()))) + " , " + QString::number(dp.Y_Axis2World(show_pointf.y()))));
        ui->stop_stat_view1->setItem(insert_rowcount,3,new QTableWidgetItem(QString::number(ana_all_it.value().size())));
        insert_rowcount++;
        show_pointf = QPointF(0,0);
    }
    this->repaint();
    dp.stop_pointf.clear();
    dp.track_pointf.clear();
    dp.last_stop_pointf.clear();
    dp.all_stop_pointf.clear();
    ui->analyze_button->setEnabled(false);
}

//--------------连线按钮事件--------------//
void tracking::on_TrackConnect_triggered()
{
    QPainter paintpoint(&axis_image);
    QPen pen;
    int t = 0;
    pen.setWidth(0.2);
    QVector<QPointF>::iterator it1;
    QVector<QPointF>::iterator it2;

    //画的是show_g_pointf里的点，即假设点击过清空按钮以后，之后显示的轨迹不会包括之前清空过的
    for(auto it = dp.show_g_pointf.begin();it != dp.show_g_pointf.end(); it++){

        pen.setColor(v_color[t]);
        t++;
        paintpoint.setPen(pen);
        it1 = it->begin();
        if(it->size() > 1){
            it2 = it1 + 1;
        }
        else{
            continue;
        }
        //距离小于300的坐标才会被连接（如果车从一边离开视野，又从另一边进入视野，这临界的两个点是不应该相连的）（300数值可调，根据读取坐标速度）
        for(;it2 != it->end(); it1++, it2++){
            if( qAbs( dp.X_Axis2World(it1->x()) - dp.X_Axis2World(it2->x()) ) < 300 && qAbs( dp.Y_Axis2World(it1->y()) - dp.Y_Axis2World(it2->y()) ) < 300){
                paintpoint.drawLine(*it1,*it2);
            }
        }
    }
    this->repaint();
}

//--------------清除界面按钮事件--------------//
void tracking::on_InterfaceClear_triggered()
{
    dp.clear_pushed = 1;
    dp.ever_clear = 1;
    dp.data_clear = 1;
    axis_image = QImage(1250 * 1.5, 500 * 1.5, QImage::Format_RGB32);
    illu_image = QImage(187.5, 75, QImage::Format_RGB32);
    QColor backColor = qRgb(255, 255, 255);
    axis_image.fill(backColor);
    illu_image.fill(backColor);
    QPainter mypainter(&axis_image);
    int point0x = 61, point0y = 727.5;//坐标轴原点
    int pointx = 25.5, pointy = 79.77;
    int width = 1811.25,height = 712.5;//坐标轴宽度，高度
    mypainter.drawRect(5,5,1250 * 1.5-10, 500 * 1.5-10);//外围矩形
    //y
    mypainter.drawLine(point0x,point0y-height,point0x,point0y);
    //x
    mypainter.drawLine(pointx,pointy,pointx+width,pointy);

    QPen pendegree;
    pendegree.setColor(Qt::black);
    pendegree.setWidth(2);
    mypainter.setPen(pendegree);
    //x轴刻度
    for(int i = 0; i < 30; i++){
        mypainter.drawLine(pointx + (i + 1) * width / 30, pointy, pointx + (i + 1) * width / 30, pointy + 4);
        mypainter.drawText(pointx+(i + 0.65) * width / 30, pointy + 13, QString::number(-100 + (i + 1) * 170));
    }
    //y轴刻度
    for(int i = 0; i < 20; i++){
        mypainter.drawLine(point0x, point0y - i * height / 20, point0x + 4, point0y - i * height / 20);
        mypainter.drawText(point0x - 22, point0y - (i - 0.15) * height / 20, QString::number(2000 - i * 110));
    }
    ui->stop_stat_view1->setRowCount(0);
    ui->track_stat_view1->setRowCount(0);
    ui->stop_stat_view1->clear();
    ui->track_stat_view1->clear();
    this->repaint();
    ui->analyze_button->setEnabled(true);

}

//--------------数据存储清除事件--------------//
void tracking::on_txt_clear_triggered()
{
    this->on_InterfaceClear_triggered();
    dp.g_pointf.clear();
    QFile file("D:\\project\\test\\debug\\track1.txt");
    file.open(QIODevice::WriteOnly);
    file.close();
    ui->txtclr_label->setText("清除成功");
    this->repaint();
    //dp.sleep(1000);
    ui->txtclr_label->setText("清空坐标");
}

//--------------从摄像头程序获取坐标线程--------------//
void tracking::on_rece_data_triggered()
{
    rd.start();
}

//--------------移动窗口事件--------------//
void tracking::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}
void tracking::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    last = e->globalPos();
    move(x()+dx, y()+dy);
}
void tracking::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    move(x()+dx, y()+dy);
}

