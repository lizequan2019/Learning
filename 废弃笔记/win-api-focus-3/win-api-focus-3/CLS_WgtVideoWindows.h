#ifndef CLS_WGTVIDEOWINDOWS_H
#define CLS_WGTVIDEOWINDOWS_H

#include <QWidget>

namespace Ui {
class CLS_WgtVideoWindows;
}

class CLS_WgtVideoWindows : public QWidget
{
    Q_OBJECT

public:
    explicit CLS_WgtVideoWindows(QWidget *parent = nullptr);
    ~CLS_WgtVideoWindows();

private:
    Ui::CLS_WgtVideoWindows *ui;
};

#endif // CLS_WGTVIDEOWINDOWS_H
