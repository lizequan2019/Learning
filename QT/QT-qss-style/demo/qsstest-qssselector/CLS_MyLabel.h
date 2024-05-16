#ifndef CLS_MYLABEL_H
#define CLS_MYLABEL_H

#include <QObject>
#include <QLabel>
class CLS_MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CLS_MyLabel(QLabel *parent = nullptr);

signals:

};

#endif // CLS_MYLABEL_H
