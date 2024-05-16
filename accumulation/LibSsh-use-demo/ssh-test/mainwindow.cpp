#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pSshmagr = new CLS_SshManager();


    CLS_SshManager::InitSshFun(); //初始化ssh库
    connect(m_pSshmagr,SIGNAL(SigEchoRes(QString)),this,SLOT(SlotRecMsg(QString)));
}

MainWindow::~MainWindow()
{
    if(m_pSshmagr)
    {
        delete m_pSshmagr;
        m_pSshmagr = nullptr;
    }

    delete ui;
}

void MainWindow::SlotRecMsg(QString _qstrMsg)
{
    ui->textEdit->append(_qstrMsg);
}

void MainWindow::on_btn_execshell_clicked()
{
    if(m_pSshmagr == nullptr)
    {
        return;
    }

    if(ui->edt_command->text().isEmpty())
    {
        qDebug()<<"请填写命令";
        return;
    }

    //设置ssh信息
    m_pSshmagr->SetServerIp(ui->edt_ip->text());
    m_pSshmagr->SetServerPort(22);
    m_pSshmagr->SetUserName(ui->edt_username->text());
    m_pSshmagr->SetPasswd(ui->edt_passwd->text());
    m_pSshmagr->SetShellCommand(ui->edt_command->text());

    int num = 0; //尝试次数
    int ret = 0;
    bool biIsSuccess = true;

    do
    {
        ret = m_pSshmagr->ExecCommand();

        if(ret != 0)
        {
            if(num < 5)
            {
                num++;
                qDebug()<<" 执行命令失败，正在尝试再次执行，第"<<num<<"次";
            }
            else
            {
                qDebug()<<" 执行命令失败，已经放弃";
                biIsSuccess = false;
                break;
            }
        }
    }while(ret != 0);

    if(biIsSuccess)
    {
        qDebug()<<" 执行命令成功";
    }
}



void MainWindow::on_btn_uploadfile_clicked()
{
    if(m_pSshmagr == nullptr)
    {
        return;
    }

    if(ui->edt_serverfile->text().isEmpty())
    {
        QMessageBox::warning(this,"注意","请填写要上传的路径");
        return;
    }

    //设置ssh信息
    m_pSshmagr->SetServerIp(ui->edt_ip->text());
    m_pSshmagr->SetServerPort(22);
    m_pSshmagr->SetUserName(ui->edt_username->text());
    m_pSshmagr->SetPasswd(ui->edt_passwd->text());
    m_pSshmagr->SetSftpRemotePath(ui->edt_serverfile->text()+"/"+m_pSshmagr->GetUploadFile());

    qDebug()<<"开始上传时间 "<<QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    int num = 0;
    int ret = 0;

    bool biIsSuccess = true;
    do
    {
        ret = m_pSshmagr->UploadFile();

        if(ret != 0)
        {
            if(num < 5)
            {
                num++;
                qDebug()<<" 上传文件失败，正在尝试上传，第"<<num<<"次";
            }
            else
            {
                qDebug()<<" 上传文件失败，已经放弃";
                biIsSuccess = false;
                break;
            }
        }
    }while(ret != 0);

    if(biIsSuccess)
    {
        qDebug()<<"上传成功";
    }
    qDebug()<<"上传结束时间 "<<QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
}

void MainWindow::on_btn_opendir_clicked()
{
    if(m_pSshmagr == nullptr)
    {
        return;
    }

    //定义文件对话框类
       m_pfileDlg = new QFileDialog(this);

       //定义文件对话框标题
       m_pfileDlg->setWindowTitle(QStringLiteral("选择文件"));

       //设置打开的文件路径
       m_pfileDlg->setDirectory(QCoreApplication::applicationDirPath());

       //设置文件过滤器,只显示.ui .cpp 文件,多个过滤文件使用空格隔开
       m_pfileDlg->setNameFilter(tr("File(*)"));

       //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
       m_pfileDlg->setFileMode(QFileDialog::ExistingFiles);

       //设置视图模式
       m_pfileDlg->setViewMode(QFileDialog::Detail);

       //获取选择的文件的路径
       QStringList fileNames;
       if (m_pfileDlg->exec())
       {
           QString filename = (m_pfileDlg->selectedFiles())[0];

           m_pSshmagr->SetSftpLoacalFile(filename);
       }
}
