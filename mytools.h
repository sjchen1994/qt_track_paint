#ifndef MYTOOLS_H
#define MYTOOLS_H
#include <QtMath>
#include <QPointF>
#include <QTime>
class MyTools
{
public:
    MyTools();

    double GetDistanceOfTwoPoints(const QPointF*, const QPointF*);
    double GetDistanceOfTwoPoints(const QPointF&, const QPointF&);

    void Sleep(unsigned int);

    //将两个迭代器之间的值拷贝到容器中，如果成功的话返回true，否则返回false
    template <typename container, typename iterator>
    bool ContainerCopy(container& v0, iterator it1, iterator it2){
        v0.clear();
        try{
            for(auto it = it1; it != it2; it++){
                v0.push_back(*it);
            }
        }catch(...){
            delete it1;
            delete it2;
            return false;
        }
        return true;
    }
};




#endif // MYTOOLS_H
