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
        in<<local_msg.constData()<< " " << context.file << " " << context.line << " "<< context.function<< " " <<current_date_time<<"\n";
        break;
          case QtInfoMsg:
              fprintf(stderr, "Info: %s (%s:%u, %s)\n", local_msg.constData(), context.file, context.line, context.function);
              break;
          case QtWarningMsg:
              fprintf(stderr, "Warning: %s (%s:%u, %s)\n", local_msg.constData(), context.file, context.line, context.function);
              break;
          case QtCriticalMsg:
              fprintf(stderr, "Critical: %s (%s:%u, %s)\n", local_msg.constData(), context.file, context.line, context.function);
              break;
          case QtFatalMsg:
              fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", local_msg.constData(), context.file, context.line, context.function);
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
