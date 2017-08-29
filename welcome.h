#ifndef WELCOME_H
#define WELCOME_H
#include <tracking.h>
#include <docking.h>
#include <QWidget>

namespace Ui {
class welcome;
}

class welcome : public QWidget
{
    Q_OBJECT

public:
    explicit welcome(QWidget *parent = 0);
    ~welcome();


private slots:
    void on_call_tracking_released();

    void reshow();

    void on_call_docking_released();

private:
    tracking* track;
    docking* dock;
    Ui::welcome *ui;
};

#endif // WELCOME_H
