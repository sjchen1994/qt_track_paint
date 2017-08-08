#include "mytools.h"

MyTools::MyTools()
{

}



/*
 * 函数名：GetDistanceOfTwoPoints
 * 输入：QPointF*, QPointF*
 * 输出：double
 * 功能：输出两个点之间的直线距离
 */
double MyTools::GetDistanceOfTwoPoints(const QPointF* first, const QPointF* second){
    return qSqrt(qPow(first->x() - second->x(), 2) + qPow(first->y() - second->y(), 2));
}
//重载
double MyTools::GetDistanceOfTwoPoints(const QPointF& first, const QPointF& second){
    return qSqrt(qPow(first.x() - second.x(), 2) + qPow(first.y() - second.y(), 2));
}


/*
 * 函数名：Sleep
 * 输入：unsigned int msec
 * 功能：sleep msec毫秒
 * */
void MyTools::Sleep(unsigned int msec){
    QTime reachtime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < reachtime){
    }
}

