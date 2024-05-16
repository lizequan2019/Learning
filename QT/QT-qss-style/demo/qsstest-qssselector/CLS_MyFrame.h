#ifndef CLS_MYFRAME_H
#define CLS_MYFRAME_H

#include <QObject>
#include <QFrame>
class CLS_MyFrame : public QFrame
{
    Q_OBJECT
public:
    explicit CLS_MyFrame(QFrame *parent = nullptr);

signals:

};

#endif // CLS_MYFRAME_H
