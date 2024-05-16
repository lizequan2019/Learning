#ifndef MOVETOTHREAD_H
#define MOVETOTHREAD_H
/*
 *   本类将自己的对象移动到 QThread创建的线程中
 */
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>

class MoveToThread : public QObject
{
    Q_OBJECT
public:
    explicit MoveToThread(QObject *parent = nullptr);

    void ThWorkTimeout();

public slots:
    void Dowork();

signals:
    void SigTimeout();
};

#endif // MOVETOTHREAD_H
