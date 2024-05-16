#ifndef CLS_DLGBATCHINPUT_H
#define CLS_DLGBATCHINPUT_H

#include <QDialog>
#include <QFileDialog>
namespace Ui {
class CLS_DlgBatchInput;
}

class CLS_DlgBatchInput : public QDialog
{
    Q_OBJECT

public:
    explicit CLS_DlgBatchInput(QWidget *parent = nullptr);
    ~CLS_DlgBatchInput();

     QString GetServerStartIp();
     QString GetServerEndIp();
     QString GetServerPort();
     QString GetUser();
     QString GetPwd();
     QString GetUpgradePack();

private slots:
    void on_btn_choose_clicked();

    void on_btn_ok_clicked();

private:
    Ui::CLS_DlgBatchInput *ui;

signals:
    void SignBatchExec();
};

#endif // CLS_DLGBATCHINPUT_H
