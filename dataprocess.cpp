#include "dataprocess.h"
//----------------宏定义--------------//
#define kx 0.355147      //x坐标转换系数
#define ky 0.323863     //y坐标转换系数
#define pi 3.14159265  //pi-value

//--------------全局变量定义--------------//

//QVector<QVector<QPointF>> s_pointf;            //存储所有停止点坐标
//QVector<QVector<QPointF>> new_s_pointf;     //用于绘制的停止点集
QVector<QPointF> empty_show_g_sub_pointf; //用于push_back到show_g_pointf的
QVector<QVector<double>> g_angle;                //存储所有点的角度
QVector<int> g_size;                                      //存储对应model_id点个数
QMutex mutex;
int new_pt_join = 0;                                       //标记是否有新的model_id点加入点集
int wechat_push = 0;                                     //标记微信通知功能是否打开
int stop_times = 0;                                        //记录停止次数
int single_stop_label = 0;                                //标记一次获取的点坐标中有无停止点


//--------------构造函数--------------//
DataProcess::DataProcess()
{

}

//--------------线程主程序--------------//
void DataProcess::run(){
    this->GetPoint();
    emit Senddata(g_pointf);
    while(true){
        if(this->quit_flag == 1){
            break;
        }
        this->Sleep(50);
        this->MainLoop();
    }
}


//--------------工具函数--------------//
//坐标转换函数
double DataProcess::X_Axis2World(const double &tmp){
    return (tmp - 25.5) / kx - 100;
}

double DataProcess::Y_Axis2World(const double &tmp){
    return (tmp - 15) / ky - 200;
}

double DataProcess::X_World2Axis(const double &tmp){
    return (tmp + 100) * kx + 25.5;
}

double DataProcess::Y_World2Axis(const double &tmp){
    return ky * (tmp + 200) + 15;
}

//获得两点之间的距离
double DataProcess::GetDistanceOfPoints(const QPointF *first, const QPointF *second){
    return qSqrt(qPow(X_Axis2World(first->x()) - X_Axis2World(second->x()), 2) + qPow(Y_Axis2World(first->y()) - Y_Axis2World(second->y()), 2));
}

//容器拷贝
void DataProcess::MyQVectorCopy(QVector<QPointF> &waiting_value, QVector<QPointF>::iterator first, QVector<QPointF>::iterator last){//按照迭代器范围拷贝容器    可考虑写成模版
    waiting_value.clear();
    for(auto it = first; it != last; it++){
        waiting_value.push_back(*it);
    }
}

//时延函数
void DataProcess::Sleep(unsigned int msec){
    QTime reachtime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < reachtime){
    }
}

//点集排序函数(从小到大)
void DataProcess::QPointfSort(QVector<QPointF> &vpf){
    QPointF tmp_pt;
    if(vpf.empty()){
        return;
    }
    for(auto last = vpf.end(); last - 1 != vpf.begin(); last--){
        for(auto first = vpf.begin(); first + 1 != last; first++){
            if(first->x()>(first + 1)->x()){
                tmp_pt = (*first);
                *first = *(first + 1);
                *(first + 1) = tmp_pt;
            }
        }
    }
}

//点集排序函数(按照点顺序)
QVector<QPointF> DataProcess::QPointfOrderSort(const QVector<QPointF> &vpf, const QString& key, const QMap<int,QVector<QPointF>> &stop_pointf){
    int t = 1;
    QPointF head_point = stop_pointf.find(key.toInt() / 10).value()[0];//作为比较的头结点
    QPointF nearest_point;  //最近的点
    QPointF sec_head_point;
    QMap<int, QPointF> VPF;//可能有问题
    for(QPointF pointf: vpf){//将点信息输入
        VPF.insert(t, pointf);
        t++;
    }
    double distance_to_head = 10000;
    double double_distance_to_heads = 10000;
    QVector<QPointF> result;//存储排序结果
    t = 1;
    //找到第一个点
    for(auto it = VPF.begin(); it != VPF.end(); it++){
        if(qAbs(it.value().x() - head_point.x()) + qAbs(it.value().y() - head_point.y()) < distance_to_head){//
            nearest_point = it.value();
            distance_to_head = qAbs(it.value().x() - head_point.x()) + qAbs(it.value().y() - head_point.y());
            t = it.key();
        }
    }
    VPF.erase(VPF.find(t));
    result.push_back(nearest_point);
    sec_head_point = nearest_point;


    //找到后面的点
    while(!VPF.empty()){
        for(auto it = VPF.begin(); it != VPF.end(); it++){
            if(qAbs(it.value().x() - head_point.x()) + qAbs(it.value().y() - head_point.y()) + qAbs(it.value().x() - sec_head_point.x()) + qAbs(it.value().y() - sec_head_point.y()) < double_distance_to_heads){
                nearest_point = it.value();
                double_distance_to_heads = qAbs(it.value().x() - head_point.x()) + qAbs(it.value().y() - head_point.y()) + qAbs(it.value().x() - sec_head_point.x()) + qAbs(it.value().y() - sec_head_point.y());
                t = it.key();
            }
        }
        VPF.erase(VPF.find(t));
        //if(double_distance_to_heads < 100){
            result.push_back(nearest_point);
            head_point = sec_head_point;
            sec_head_point = nearest_point;
        //}
        double_distance_to_heads = 11000;
    }
    return result;
}

//点集排序函数(按照点顺序)
QVector<QPointF> DataProcess::QPointfOrderSortForJumpInsert(const QVector<QPointF> &vpf, int nowid, const QMap<int,QVector<QPointF>> &stop_pointf){
    int t = 1;

    QPointF head_point = stop_pointf.find(nowid).value()[0];//作为比较的头结点
    QPointF nearest_point;  //最近的点
    QMap<int, QPointF> VPF;//可能有问题
    for(QPointF pointf: vpf){//将点信息输入
        VPF.insert(t, pointf);
        t++;
    }
    double distance_to_head = 10000;
    QVector<QPointF> result;//存储排序结果
    t = 1;
    while(!VPF.empty()){
        for(auto it = VPF.begin(); it != VPF.end(); it++){
            if(qAbs(it.value().x() - head_point.x()) + qAbs(it.value().y() - head_point.y()) < distance_to_head){//
                nearest_point = it.value();
                distance_to_head = qAbs(it.value().x() - head_point.x()) + qAbs(it.value().y() - head_point.y());
                t = it.key();
            }
        }
        VPF.erase(VPF.find(t)); //从容器中删除
        if(distance_to_head < 100){
            result.push_back(nearest_point);
            head_point = nearest_point;
        }
        distance_to_head = 10000;
    }
    return result;
}

//停止点map排序工具
void DataProcess::MapPointSort(QMap<int, QVector<QPointF>>& sorting_points, int find_id){//将点集按照key正确排序
    while(find_id != sorting_points.lastKey()){
        sorting_points.find(find_id).value().swap(sorting_points.last());
        find_id++;
    }
}

//轨迹map排序工具
void DataProcess::MapTrackSort(QMap<QString, QVector<QPointF> > &sorting_track, QVector<QPointF> insert_points1, QVector<QPointF> insert_points2, int find_id){//将轨迹坐标按key正确添加并排序
    sorting_track.erase(sorting_track.find(QString::number(find_id) + QString::number(find_id + 1))); //先把找到新的点的一段轨迹删除
    QMap<QString, QVector<QPointF>> sorted_track;
    //int sort_id = find_id + 1;
    for(auto it = sorting_track.begin(); it != sorting_track.end(); it++){
        if(it.key().toInt() / 10 > 1 && it.key().toInt() % 10 == 1){
            sorted_track.insert(QString::number(it.key().toInt() / 10 + 1) + "1", it.value());
        }
        else if (it.key().toInt() % 10 <= find_id) {
            sorted_track.insert(it.key(),it.value());
        }
        else{
            sorted_track.insert(QString::number((it.key().toInt() / 10) + 1) + QString::number((it.key().toInt() % 10) + 1), it.value());
        }
    }
    sorted_track.insert(QString::number(find_id) + QString::number(find_id + 1), insert_points1);
    sorted_track.insert(QString::number(find_id + 1) + QString::number(find_id + 2), insert_points2);
    sorting_track = sorted_track;
    /*//如果是41->51
    if(sorting_track.find(QString::number(sort_id) + "1") != sorting_track.end()){
        sorting_track.insert(QString::number(sort_id + 1) + "1", sorting_track.find(QString::number(sort_id) + "1").value());
        sorting_track.erase(sorting_track.find(QString::number(sort_id) + "1"));
    }
    else{
        for(auto it = sorting_track.find(QString::number(sort_id) + QString::number(sort_id + 1)); it != sorting_track.end(); it++){
            //如果是34->45
            sorting_track.insert(QString::number((it.key().toInt() / 10) + 1) + QString::number((it.key().toInt() % 10) + 1), it.value());
            sorting_track.erase(it);
        }
    }
    //将一段轨迹分为两段插入
    */
}

//判断点是否在边界函数
bool DataProcess::NearEdgeJudge(const QPointF* judge_point){
   if( judge_point->x() < 30 || 1875 - judge_point->x() < 30 || judge_point->y() < 30 || 749 - judge_point->y() < 30  ){
       return true;
   }
   else{
       return false;
   }
}

//--------------功能函数--------------//
//把新的id点和现有点进行正确的排序
bool DataProcess::NewPointSort(int i, QMap<QString,QVector<QPointF>> &track_map, QMap<int,QVector<QPointF>> &update_stop_pointf, QMap<int,QVector<QPointF>> &update_all_stop_pointf, QMap<int,QVector<QPointF>>& update_last_stop_pointf, QPointF compare_point, int &change_id){
    bool isincluded = false;
    int t = 1;
    QVector<QPointF> tmp_push;
    QVector<QPointF> insert_pointsf;
    QVector<QPointF> insert_pointsl;
    //先排序
    for(auto sort_it = track_map.begin(); sort_it != track_map.end(); sort_it++){
        sort_it.value() = QPointfOrderSort(sort_it.value(), sort_it.key(), update_stop_pointf);
        //QPointfSort(sort_it.value());
    }
    while(t < i - 1){
        QVector<QPointF>::iterator it = (track_map.find(QString::number(t) + QString::number(t + 1)).value()).begin();
        for(;it != track_map.find(QString::number(t) + QString::number(t + 1)).value().end(); it++){
            if(qAbs(X_Axis2World(compare_point.x()) - X_Axis2World(it->x())) < 100 && qAbs(X_Axis2World(compare_point.y()) - X_Axis2World(it->y())) < 100){ //说明这段轨迹中  包含了待测点
                //track_map.find(QString::number(t) + QString::number(t + 1)).value().clear();//清空轨迹点

                //给last里插入新的坐标
                if(update_last_stop_pointf.find(i) == update_last_stop_pointf.end()){
                    tmp_push.push_back(compare_point);
                    update_last_stop_pointf.insert(i, tmp_push);
                }

                //更新停止点
                MapPointSort(update_all_stop_pointf, t + 1);//交换坐标集
                MapPointSort(update_stop_pointf, t + 1);
                MapPointSort(update_last_stop_pointf, t + 1);

                //更新轨迹点
                MyQVectorCopy(insert_pointsf, track_map.find(QString::number(t) + QString::number(t + 1)).value().begin(), it);
                MyQVectorCopy(insert_pointsl, it, track_map.find(QString::number(t) + QString::number(t + 1)).value().end());
                MapTrackSort(track_map, insert_pointsf, insert_pointsl, t);

                isincluded = true;
                change_id = t + 1;
                return isincluded;
            }
        }
        t++;
    }
    return isincluded;
}

//获取最新的点
void DataProcess::GetPoint(){
    QMutexLocker locker(&mutex);
    double tmp_value;
    int i = 0;
    QPointF tmp_point;
    QVector<QPointF> g_tmp_pointf;
    QVector<double> g_tmp_angle;
    QString g_openstr;
    QString readline_data;
    QString data="";


    g_angle.clear();

    while(true){
        i++;
        g_openstr = "D:\\project\\test\\debug\\track" + QString::number(i) + ".txt";
        QFile file(g_openstr);
        g_tmp_pointf.clear();

        /*if(s_pointf.size() < i){
            s_pointf.push_back(empty_show_g_sub_pointf);
        }*/
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            break;
        }

        //普通点  读取
        QTextStream stream(&file);
        while(true){
            readline_data = stream.readLine(0);
            if(readline_data == ""){
                break;
            }
            else{
                QString::iterator it = readline_data.begin();
                for(;it != readline_data.end(); it++){
                    if(*it == ','){
                        tmp_value = data.toDouble();
                        tmp_value = X_World2Axis(tmp_value);
                        tmp_point.setX(tmp_value);
                        data.clear();
                        continue;
                    }
                    else if(*it == " "){
                        tmp_value = data.toDouble();
                        tmp_value = Y_World2Axis(tmp_value);
                        tmp_point.setY(tmp_value);
                        data.clear();
                    }
                    else{
                        data += *it;
                    }
                }
                g_tmp_angle.push_back(data.toDouble());
                g_tmp_pointf.push_back(tmp_point);
                data.clear();
            }
        }
        file.close();
        g_pointf.clear();
        g_pointf.push_back(g_tmp_pointf);
        g_angle.push_back(g_tmp_angle);
    }


    //将坐标容器中的size存储起来  为judge里面的判断做准备
    QVector<QVector<QPointF>>::iterator g_it = g_pointf.begin();
    QVector<int>::iterator g_int_it = g_size.begin();
    for(; g_it != g_pointf.end(); g_it++, g_int_it++){
        if(g_int_it != g_size.end()){//更新容器大小指标
            *(g_int_it) = (*(g_it)).size();
        }
        else if(g_int_it == g_size.end()){
            g_int_it = g_size.insert(g_int_it, (*g_it).size());
            new_pt_join = 1;
        }
    }

}

//循环判断函数：传递最新的点给主进程
void DataProcess::MainLoop(){
    QVector<int> tmp_g_size = g_size;

    int ExPointInsertLabel = 0;
    int t = 0;
    //int is_stop_point = 0;
    this->GetPoint(); //获取最新的坐标信息

    //绘出新的坐标点

    int g_pointf_size = g_pointf.size(); //获取坐标容器的size

    if(clear_pushed == 1){  //点击过清空按钮
        //新的坐标容器
        show_g_pointf.clear();
        //new_s_pointf.clear();
        empty_show_g_sub_pointf.clear();

        clear_pushed = 0;
    }
    if(new_pt_join == 1 && data_clear == 1){//如果有新的类型的点加入，根据新加入的不同类型的坐标的数量来push空的容器
        show_g_pointf.clear();
        //new_s_pointf.clear();
        empty_show_g_sub_pointf.clear();
        while(g_pointf_size--){
            show_g_pointf.push_back(empty_show_g_sub_pointf);
            //new_s_pointf.push_back(empty_show_g_sub_pointf);
        }
        data_clear = 0;
    }

    QVector<QPointF>::iterator a;
    QVector<QVector<QPointF>>::iterator g_it = g_pointf.begin();
    //QVector<QVector<QPointF>>::iterator new_s_it = s_pointf.begin();
    QVector<int>::iterator tmp_g_size_it = tmp_g_size.begin();
    QVector<int>::iterator g_size_it = g_size.begin();
    QVector<QVector<double>>::iterator g_angle_it = g_angle.begin();
    QVector<double>::iterator angle_it;
    for(; g_it != g_pointf.end(); g_it++, g_angle_it++){
        if(*g_size_it > *tmp_g_size_it){ //有新的坐标读入
            single_stop_label = 1;
            ExPointInsertLabel = 1;  //说明有已存在的类型的坐标新的坐标输入
            a = (*g_it).begin();
            angle_it = (*g_angle_it).begin();
            int tmp_g_size = *tmp_g_size_it;

            while(tmp_g_size--){ //使迭代器指向最新读取的第一个坐标处
                a++;
                angle_it++;
            }

            for(; a != (*g_it).end(); a++, angle_it++){
                //找出停止点坐标
                /*if(angle_it + 1 != (*g_angle_it).end()){
                    if( qAbs(X_Axis2World((a+1)->x()) - X_Axis2World(a->x())) < 0.5 && qAbs(Y_Axis2World((a+1)->y()) - Y_Axis2World(a->y())) < 0.5 && qAbs((*(angle_it+1)) - (*angle_it)) <= 0.01 ){
                        is_stop_point = 1;
                    }
                }

                //画出新轨迹
                if(is_stop_point == 1){
                    (*new_s_it).push_back(*a);
                    new_s_pointf[t].push_back(*a);
                    is_stop_point = 0;

                }
                else{

                    single_stop_label = 0;//这一次判断中，车是不静止的
                }*/

                show_g_pointf[t].push_back(*a);  //将新读取的坐标写入show_g_pointf
            }


        }
        g_size_it++;
        tmp_g_size_it++;
        t++;
    }

    if(single_stop_label == 1){//此次判断中，车是静止的
        stop_times++;
    }
    if(stop_times == 10){//停止超过1秒
        stop_times = 0;
        if(wechat_push == 1){
            //将信息写入txt供python程序读取，python程序读取信息正确后，就会发消息到微信上
        }
    }
    if(ExPointInsertLabel == 0 && data_clear == 0 && ever_clear == 0){  //如果没有点击过按钮   show_g_pointf 和 g_pointf 是相同的
        //new_s_pointf = s_pointf;
        show_g_pointf = g_pointf;
    }
    if(ExPointInsertLabel == 1){
        emit Senddata(show_g_pointf);
    }

    //重绘停止点 防止被其他点覆盖
    //paint_stop_points(new_s_pointf);
}

//获得停止点方差
double DataProcess::AnalyzeStopPoints(QVector<QPointF> ana_points){
    QPointF tmp;
    QVector<QPointF> tmp_points = ana_points;
    while(true){
        if(ana_points.size() <= 2){
            tmp = (ana_points.back() - ana_points.front()) / 2 + ana_points.front();
            //ana_points.clear();
            //ana_points.push_back(tmp);
            double sum = 0;
            for(auto it = tmp_points.begin(); it != tmp_points.end(); it++){
                sum += qPow((tmp.x() - it->x()), 2) + qPow((tmp.y() - it->y()), 2);
            }
            return sum / tmp_points.size();
        }
        else{
           tmp = (ana_points.back() - ana_points.front()) / 2 + ana_points.front();
           ana_points.pop_back();
           ana_points.pop_front();
           ana_points.push_back(tmp);
        }
    }
}

//获得轨迹方差
double DataProcess::AnalyzeTrackPoints(QVector<QPointF> ana_points){
    //先排序
    QPointfSort(ana_points);
    double sum = 0;
    int size = ana_points.size() / 20;
    if(!size){
        size = 1;
    }
    QPointF ave_p1;
    QPointF ave_p2;

    //取前5%和后5%的坐标平均后得到基准线
    for(int i = 0; i < size; i++){
        ave_p1 += ana_points[i];
    }
    ave_p1 = ave_p1/size;
    auto iter = ana_points.rbegin();
    for(int i = 0; i < size; i++, iter++){
        ave_p2 += *iter;
    }
    ave_p2 = ave_p2 / size;

    //删除前5%和后5%的点
    for(int i = 0 ; i < size; i++){
        ana_points.pop_front();
    }
    for(int i = 0; i < size; i++){
        ana_points.pop_back();
    }

    for(QPointF point : ana_points){
        if(ave_p1.x() == ave_p2.x()){
            sum += qPow(point.x() - ave_p1.x(), 2);
        }
        else{
            double angle = QLineF(ave_p1,ave_p2).angleTo((QLineF(ave_p1,point)));
            sum += qPow(sin(angle * pi / 180) * (QLineF(ave_p1,point).length()), 2);
        }
    }
    if(size == 0){
        return 0;
    }
    return sum / ana_points.size();
}

//将未识别到中间停止点的轨迹分割，正确插入容器中
void DataProcess::JumpPointInsert(QVector<QPointF> inserting_points, const int nowid, const QMap<int,QVector<QPointF>> &stop_pointf, QMap<QString,QVector<QPointF>>& push_track_pointf){
    int maxid = stop_pointf.lastKey();
    QVector<QPointF> insert_pointsf;
    QVector<QPointF> insert_pointsl;
    if(inserting_points.empty()){
        return;
    }
    inserting_points = QPointfOrderSortForJumpInsert(inserting_points, nowid, stop_pointf); //按照轨迹方向排序
    int it_now = nowid;
    int it_next = (nowid == maxid? 1: nowid + 1);
    //将获取到的轨迹段插入对应的容器中
    while(true){
        for(auto it = inserting_points.begin(); it != inserting_points.end(); it++){
            if(qAbs(X_Axis2World(stop_pointf.find(it_next).value()[0].x()) - X_Axis2World(it->x())) < 50 && qAbs(X_Axis2World(stop_pointf.find(it_next).value()[0].y()) - X_Axis2World(it->y())) < 50){
                MyQVectorCopy(insert_pointsf, inserting_points.begin(), it);
                MyQVectorCopy(insert_pointsl, it, inserting_points.end());
                inserting_points = insert_pointsl;
                push_track_pointf.find(QString::number(it_now) + QString::number(it_next)).value().append(insert_pointsf);
                break;
            }
        }
        if(insert_pointsf.empty()){
            break;
        }
        it_next = (it_next == maxid? 1 : it_next + 1);
        it_now   = (it_now == maxid? 1 : it_now + 1);
    }
}

//点集分析主函数
void DataProcess::AnalyzePoints(){

    QVector<QVector<QPointF>> tmp_g_pointf = this->g_pointf;
    int max_id = 1;
    //int cal_max_id;
    int i;
    int now_id;
    int next_id;
    int point_near = 0;
    int pointf_have_exist = 0;

    QPointF jump_point;

    QVector<QPointF>::iterator first;
    QVector<QPointF>::iterator last;

    QMap<int,QVector<QPointF>> uppush_all_stop_pointf;//存储所有停止点
    QVector<QPointF> push_all_stop_pointf;

    QMap<int,QVector<QPointF>> uppush_stop_pointf;//存储首停止点
    QVector<QPointF> push_stop_pointf;

    QMap<int,QVector<QPointF>> uppush_last_stop_pointf;//存储末停止点
    QVector<QPointF> push_last_stop_pointf;

    QMap<QString,QVector<QPointF>> push_track_pointf;//存储中间轨迹点
    QVector<QPointF> every_track_point;//存储每一段中间的轨迹点

    QPointF stop_tmp_pointf1;
    QPointF stop_tmp_pointf2;


    //-------------得到要分析的每一段轨迹坐标  以及 每个停止点坐标的集合--------//
    /* 需要获得的目标:
     * QVector<QMap<int,QVector<QPointF>>> stop_pointf;     : 到达新的目标区域的第一个停止点坐标
     * QVector<QMap<int,QVector<QPointF>>> all_stop_pointf  : 所有停止点坐标（形式：id-value）
     * QVector<QMap<QString,QVector<QPointF>>> track_pointf : 所有轨迹坐标    示例: "12",(558.221,241.704)...  指  id为 1 2 之间的轨迹坐标集
    */

    for(auto it = tmp_g_pointf.begin(); it != tmp_g_pointf.end(); it++){//大循环，考虑不同model_id的坐标 抓出要分析的坐标
        if(it->size() < 2){//如果坐标数目不够2个  则无需判断 退出
            continue;
        }
        first = it->begin();
        last = first + 1;
        now_id = 1;
        while(last != it->end()){  //先找出第一个停止点  并标记为1  存储在map中
            if((qAbs(X_Axis2World(last->x()) - X_Axis2World(first->x())) < 1.5) &&   (qAbs(Y_Axis2World(last->y()) - Y_Axis2World(first->y())) < 1.5)){
                stop_tmp_pointf2 = (*first);
                push_all_stop_pointf.push_back(stop_tmp_pointf2);
                uppush_stop_pointf.insert(1, push_all_stop_pointf);
                uppush_all_stop_pointf.insert(1, push_all_stop_pointf);
                first++;
                last++;
                break;
            }
            first++;
            last++;
        }
        for(;last != it->end(); first++, last++){//对剩下的坐标进行遍历
            //如果有坐标突变，则找到下一个接近的点开始记录
            if((X_Axis2World(last->x())-X_Axis2World(first->x()) > 500 || Y_Axis2World(last->y()) - Y_Axis2World(first->y()) > 500) && jump_point.isNull()){
                jump_point = *first;
                continue;
            }
            if(!jump_point.isNull()){
                if(GetDistanceOfPoints(&jump_point, first) > 50){
                    continue;
                }
                else{
                    jump_point = QPointF(0,0);
                }
            }

            push_stop_pointf.clear();//清空，为新id的坐标push做准备
            push_all_stop_pointf.clear();
            push_last_stop_pointf.clear();
            every_track_point.push_back(*first);//存储两个较远停止点之间的点数据

            //对接近边界的点进行记录
            if((qAbs(X_Axis2World(first->x()) - X_Axis2World(last->x())) > 300 || qAbs(Y_Axis2World(first->y()) - Y_Axis2World(last->y())) > 300) && NearEdgeJudge(first) && NearEdgeJudge(last)){
                for(auto it = edge_pointf.begin(); it != edge_pointf.end(); it++){
                    if(GetDistanceOfPoints(&it.value(), first) < 100){
                        point_near = 1;
                    }
                }
                if(point_near == 0){
                    edge_pointf.insert(QChar(97 + edge_pointf.size()), *first); //97:字母'a'
                    edge_pointf.insert(QChar(97 + edge_pointf.size()), *last);
                }
               point_near = 0;
            }

            //判断是否是停止点
            if(qAbs(X_Axis2World(last->x()) - X_Axis2World(first->x())) < 1.5 && qAbs(Y_Axis2World(last->y()) - Y_Axis2World(first->y())) < 1.5){
                stop_tmp_pointf1 = stop_tmp_pointf2;
                stop_tmp_pointf2 = (*first);

                //如果不是同一个id的点
                if(qAbs(X_Axis2World(stop_tmp_pointf1.x()) - X_Axis2World(stop_tmp_pointf2.x())) > 500 || qAbs(Y_Axis2World(stop_tmp_pointf1.y()) - Y_Axis2World(stop_tmp_pointf2.y())) > 500){
                    pointf_have_exist = 0;

                    for(i = 1; uppush_stop_pointf.find(i) != uppush_stop_pointf.end() ; i++){//遍历map中有无和当前同一id的点集合

                        //判断是否有当前id的点
                        if(qAbs(X_Axis2World((uppush_stop_pointf.find(i).value())[0].x()) - X_Axis2World(stop_tmp_pointf2.x())) < 500 && qAbs(Y_Axis2World((uppush_stop_pointf.find(i).value())[0].y()) - Y_Axis2World(stop_tmp_pointf2.y())) < 500){

                            //将应输入的点输入到容器中
                            if(uppush_last_stop_pointf.find(now_id) == uppush_last_stop_pointf.end()){
                                push_last_stop_pointf.push_back(stop_tmp_pointf1);
                                uppush_last_stop_pointf.insert(now_id,push_last_stop_pointf);
                            }
                            else{
                                uppush_last_stop_pointf.find(now_id).value().push_back(stop_tmp_pointf1);
                            }

                            uppush_all_stop_pointf.find(i).value().push_back(stop_tmp_pointf2);
                            uppush_stop_pointf.find(i).value().push_back(stop_tmp_pointf2);

                            pointf_have_exist = 1;//获得的点在map中已经存在
                            next_id = i;
                            break;
                        }

                    }


                    if(pointf_have_exist == 0){
                        //获得的点在map中还未存在　说明是新的停止点

                        next_id = i;
                        if(next_id > max_id){//设置最大id
                            max_id = next_id;
                        }

                        //将应输入的点输入到容器中
                        if(uppush_last_stop_pointf.find(now_id) != uppush_last_stop_pointf.end()){
                            uppush_last_stop_pointf.find(now_id).value().push_back(stop_tmp_pointf1);
                        }
                        else{
                            push_last_stop_pointf.push_back(stop_tmp_pointf1);
                            uppush_last_stop_pointf.insert(now_id,push_last_stop_pointf);
                        }

                        push_all_stop_pointf.push_back(stop_tmp_pointf2);
                        uppush_all_stop_pointf.insert(i,push_all_stop_pointf);

                        push_stop_pointf.push_back(stop_tmp_pointf2);
                        uppush_stop_pointf.insert(i,push_stop_pointf);


                        //--------------检验新id点的正确性--------------//
                        if(NewPointSort(next_id, push_track_pointf, uppush_stop_pointf, uppush_all_stop_pointf, uppush_last_stop_pointf, stop_tmp_pointf2, now_id)){
                            every_track_point.clear();
                        }
                    }

                    //检查是否有错误的轨迹数据集


                    //push 要分析的点坐标
                    if(push_track_pointf.find(QString::number(now_id) + QString::number(next_id)) != push_track_pointf.end()){//不支持2个点
                        if((now_id > next_id && now_id - next_id > 1 ) || (now_id < next_id && next_id - now_id == 1)){
                            (push_track_pointf.find(QString::number(now_id) + QString::number(next_id)).value()).append(every_track_point);
                        }

                    }
                    else{
                        if((now_id > next_id && next_id == 1 && now_id == max_id) || (now_id < next_id && next_id - now_id == 1)){
                            push_track_pointf.insert(QString::number(now_id) + QString::number(next_id),every_track_point);
                        }
                        else{
                            JumpPointInsert(every_track_point, now_id, uppush_stop_pointf, push_track_pointf);
                        }
                    }
                    /*cal_max_id = max_id;
                    while(--cal_max_id){
                        if(push_track_pointf.find(QString::number(cal_max_id) + "1") != push_track_pointf.end()){//说明存在错误的轨迹坐标集 删之
                            push_track_pointf.erase(push_track_pointf.find(QString::number(cal_max_id) + "1"));
                        }
                    }*/


                    if(!every_track_point.empty()){
                        now_id = i;
                    }
                }
                else{
                    //是同一个id的点
                    if(uppush_all_stop_pointf.find(now_id) != uppush_all_stop_pointf.end()){
                        uppush_all_stop_pointf.find(now_id).value().append(stop_tmp_pointf2);
                    }
                    else{
                        push_all_stop_pointf.append(stop_tmp_pointf2);
                        uppush_all_stop_pointf.insert(now_id, push_all_stop_pointf);
                    }
                }

                every_track_point.clear();
            }
        }

        //将应输入的点输入到容器中
        last_stop_pointf.push_back(uppush_last_stop_pointf);
        all_stop_pointf.push_back(uppush_all_stop_pointf);
        stop_pointf.push_back(uppush_stop_pointf);
        track_pointf.push_back(push_track_pointf);
        uppush_stop_pointf.clear();
        uppush_all_stop_pointf.clear();
        push_track_pointf.clear();
    }
}
