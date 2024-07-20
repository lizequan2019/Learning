#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_DlgBatchInput.h"
#include "ui_CLS_DlgBatchInput.h"

CLS_DlgBatchInput::CLS_DlgBatchInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLS_DlgBatchInput)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
}

CLS_DlgBatchInput::~CLS_DlgBatchInput()
{
    delete ui;
}

QString CLS_DlgBatchInput::GetServerStartIp()
{
    return ui->edt_startip->text();
}

QString CLS_DlgBatchInput::GetServerEndIp()
{
    return ui->edt_endip->text();
}

QString CLS_DlgBatchInput::GetServerPort()
{
    return ui->edt_port->text();
}

QString CLS_DlgBatchInput::GetUser()
{
    return ui->edt_user->text();
}

QString CLS_DlgBatchInput::GetPwd()
{
    return ui->edt_pwd->text();
}

QString CLS_DlgBatchInput::GetUpgradePack()
{
    return ui->edt_upgradepack->text();
}

void CLS_DlgBatchInput::on_btn_choose_clicked()
{
    //定义文件对话框类
    QFileDialog fileDialog;

    //定义文件对话框标题
    fileDialog.setWindowTitle(tr("打开文件"));

    //设置默认文件路径
    fileDialog.setDirectory("./");

    //设置文件过滤器
    fileDialog.setNameFilter(("zip(*.zip)"));

    //设置为只能选择一个文件
    fileDialog.setFileMode(QFileDialog::ExistingFile);

    //设置视图模式
    fileDialog.setViewMode(QFileDialog::Detail);

    //打印所有选择的文件的路径
    QStringList fileNames;
    if (fileDialog.exec())
    {
        fileNames = fileDialog.selectedFiles();
    }

    for (auto tmp : fileNames)
    {//选多个文件只有第一个有效  防止用户选择多个文件
        this->ui->edt_upgradepack->setText(tmp);
        break;
    }
}

void CLS_DlgBatchInput::on_btn_ok_clicked()
{
    emit SignBatchExec();
    this->hide();
}

