#include "welcome.h"
#include <QApplication>
#include <QMetaType>

//打日志
void message_output(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    QFile file("log.txt");
    file.open(QIODevice::Text|QIODevice::WriteOnly|QIODevice::Append);
    QTextStream in(&file);
    QByteArray local_msg = msg.toLocal8Bit();
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        in<<"debug"<<local_msg.constData()<< " " << context.file << " " << context.line << " "<< context.function<< " " <<current_date_time<<"\n";
        break;
          case QtInfoMsg:
             in<<"info"<<local_msg.constData()<< " " << context.file << " " << context.line << " "<< context.function<< " " <<current_date_time<<"\n";
              break;
          case QtWarningMsg:
             in<<"warning"<<local_msg.constData()<< " " << context.file << " " << context.line << " "<< context.function<< " " <<current_date_time<<"\n";
              break;
          case QtCriticalMsg:
              in<<"critical"<<local_msg.constData()<< " " << context.file << " " << context.line << " "<< context.function<< " " <<current_date_time<<"\n";
              break;
          case QtFatalMsg:
              in<<"fatal"<<local_msg.constData()<< " " << context.file << " " << context.line << " "<< context.function<< " " <<current_date_time<<"\n";
              abort();
          }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(message_output);
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<QVector<QPointF>> >("QVector<QVector<QPointF>>");//注册类型，使其能用信号-槽来传输
    welcome w;
    w.show();
    return a.exec();
}
