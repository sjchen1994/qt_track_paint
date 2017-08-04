#ifndef DOCKING_H
#define DOCKING_H

#include <QMainWindow>

namespace Ui {
class docking;
}

class docking : public QMainWindow
{
    Q_OBJECT

public:
    explicit docking(QWidget *parent = 0);
    ~docking();
signals:
    void SendSignal();
private slots:
    void on_quit_released();

private:
    Ui::docking *ui;
};

#endif // DOCKING_H
