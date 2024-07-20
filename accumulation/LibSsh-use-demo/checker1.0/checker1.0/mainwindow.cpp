#if _MSC_VER >= 1600 //vs2010及其以上版本
#pragma execution_character_set("utf-8")  //这句是关键
#endif

#include "CLS_WgtSshList.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_blIsBatchInputShow(false)
    , m_blMove(false)
    , m_iTabIndex(0)
{
    ui->setupUi(this);

    //【TODO 隐藏执行错误图标样例,以后需要再解开】
    ui->lab_fail->hide();
    ui->label_10->hide();

    //无边框
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

    //设置全局线程池
    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());

    //程序启动时加载本地缓存文件中的数据
    CLS_LoadRuleManager::getInstance()->LoadLoaclRules();

    //创建ssh列表界面
    m_pSshListView = new CLS_WgtSshList(this);
    m_pSshListView->setObjectName("wgt_hostinfolist");
    connect(m_pSshListView,SIGNAL(SigEchoRes(int,QJsonObject)),this,SLOT(SlotEchoRes(int,QJsonObject)));//ssh回显信号响应
    connect(m_pSshListView,SIGNAL(SigSshErrMsg(QString)),this,SLOT(SlotSshErrMsg(QString))); //ssh本身错误回显

    //将ssh列表界面插入到 【开始执行】【批量输入】【批量删除】一行和 【全选勾选框】一行 之间
    ui->verticalLayout->insertWidget(1, m_pSshListView);

    //初始化增加1行输入组件
    STRUCT_SSH info; //添加ssh项时填充默认的ssh信息
    m_pSshListView->AddItem(info);


    //批量输入界面创建   【TODO批量输入界面以前是有的，不过现在不需要，所以有关功能都隐藏起来，以后需要再调试 2022/7/14】
    m_pdlgBatchInput = new CLS_DlgBatchInput(this);
    m_pdlgBatchInput->setObjectName("wgt_batchinput");
    m_pdlgBatchInput->hide();
    ui->btn_batchinput->hide();//先将批量删除和批量添加按钮隐藏起来
    ui->btn_batchdel->hide();
    connect(m_pdlgBatchInput,SIGNAL(SignBatchExec()),this,SLOT(SlotBatchAdd()));



    //设置拆分布局，此布局是一个垂直方式分割，主要是用于主界面显示，上面是ssh信息界面 下面是显示执行结果界面
    ui->splitter->setSizes(QList<int>()<<const_SplitTopInitSize<<const_SplitBottomInitSize);


    //创建系统设置界面
    m_pSystemSetting = new CLS_WgtSystemSetting();
    m_pSystemSetting->SlotViewRule();  //程序启动时就要预览本地缓存规则
    m_pSystemSetting->setObjectName("wgt_systemset");
    connect(m_pSystemSetting,SIGNAL(SigAllUpdate()),this,SLOT(SlotAllUpdate())); //全量更新响应
    connect(m_pSystemSetting,SIGNAL(SigAddUpdate()),this,SLOT(SlotAddUpdate())); //增量更新响应
    connect(CLS_RuleUpdateManager::getInstance(),SIGNAL(SigUpdateOver()),m_pSystemSetting,SLOT(SlotViewRule())); //更新完毕信号响应
    connect(CLS_RuleUpdateManager::getInstance(),SIGNAL(SigUpdateErr()),m_pSystemSetting,SLOT(SlotUpdateErr())); //更新信号信号响应
    connect(m_pSystemSetting,SIGNAL(SigSaveUserInfo(QVariant)),this,SLOT(SlotSaveUserInfo(QVariant)));//保存设置响应


    //设置层叠布局，将主界面和系统设置界面放入其中，方便切换
    m_pStackedLayout = new QStackedLayout;
    m_pStackedLayout->addWidget(ui->wgt_sshmsg);
    m_pStackedLayout->addWidget(m_pSystemSetting);
    ui->verticalLayout_5->insertLayout(2,m_pStackedLayout);

    connect(&m_expertPdf,SIGNAL(SignExportFinish(int)),this,SLOT(SlotExportFinish(int)));

    CLS_SshManager::InitSshFun();

    //读取并设置样式表
    QFile file("./style.qss");
    if(file.open(QIODevice::ReadOnly))
    {
        QString qstrStyle = file.readAll();
        this->setStyleSheet(qstrStyle);
        file.close();
    }
}

MainWindow::~MainWindow()
{
    RemovePdfDir();
    delete ui;
}

void MainWindow::ExportPdfFromTabIndex(int _tabindex)
{
    //指针判空操作
    if(m_pSystemSetting == nullptr)
    {
        return;
    }

    //获取当前被选中的tab页签索引
    int Index =  _tabindex;

    //从tab页获取服务器ip
    QString qstrServerIp = ui->tabWidget->tabText(Index);

    //通过服务器ip获取当前tab页对应的ssh的id项
    int sshId = CLS_ResManager::getInstance()->GetSshIdFromSerIp(qstrServerIp);

    //通过ssh项id获取对应的pdf基础信息
    STRUCT_RES basemsg = CLS_ResManager::getInstance()->m_mapSshBasicMsg[sshId];


    //处理详情表格将被替换成的文本
    QString qstrSubTitle;
    QString qstrRuleName;
    QString qstrRuleVersion;
    QString qstrRuleRes;
    QString qstrTotalDetail;

    QVector<QString> vecbatruleid = CLS_ResManager::getInstance()->m_mapSshBatRule[sshId];
    for(int i = 0; i < vecbatruleid.size(); ++i)
    {
       QString batruleid = vecbatruleid[i];
       STRUCT_BATRULE strtbatrule = CLS_LoadRuleManager::getInstance()->GetBatRule()[batruleid];
       QVector<QString> vecrulesid = strtbatrule.GetMergeRules();

       for(int j = 0; j < vecrulesid.size(); ++j)
       {
           if(j == 0)
           {
               qstrSubTitle = QString("2.%1 %2(%3)").arg(QString::number(i+1),strtbatrule.m_name,strtbatrule.m_version);
           }
           else
           {
               qstrSubTitle = "";
           }

           QString ruleid = vecrulesid[j];
           STRUCT_RULE strrule = CLS_LoadRuleManager::getInstance()->GetRule()[ruleid];
           qstrRuleName = strrule.m_name;
           qstrRuleVersion = strrule.m_version;
           qstrRuleRes = CLS_ResManager::getInstance()->m_mapRuleRes[batruleid+ruleid];

           //判断颜色信息来增加等级类型的累计量
           if(qstrRuleRes.contains(const_HtmlRed_Fatal, Qt::CaseInsensitive))
           {
               basemsg.m_iFatalNum++;
           }
           else if(qstrRuleRes.contains(const_HtmlPurple_Serious, Qt::CaseInsensitive))
           {
               basemsg.m_iSeriousNum++;
           }
           else if(qstrRuleRes.contains(const_HtmlYellow_Slight, Qt::CaseInsensitive))
           {
               basemsg.m_iSlightNum++;
           }
           else
           {
               basemsg.m_iNormalNum++;
           }

           qstrTotalDetail += QString("<div class=\"pageDetail_subtitle\">%1</div> \
                                      <div class=\"pageDetail_table\"> \
                                         <table border=\"1\"> \
                                             <tr> \
                                                 <td>规则名称</td> <td>%2</td> \
                                             </tr> \
                                             <tr> \
                                                 <td>规则版本</td> <td>%3</td> \
                                             </tr>\
                                             <tr> \
                                                 <td>执行结果</td> <td>%4</td> \
                                             </tr>\
                                         </table>\
                                     </div>").arg(qstrSubTitle,qstrRuleName,qstrRuleVersion,qstrRuleRes);
       }
    }



    //获取以下信息 操作用户、服务器ip、开始检测时间、检测耗时、所有规则组名称、致命、严重、轻微、正常个数
    QString qstrname = basemsg.m_qstrUser;
    QString qstrIp   = basemsg.m_qstrTargetIp;
    QString qstrStartTime = basemsg.m_qstrStartTime;
    QString qstrTakeTime  = CalcTimeDiff(basemsg.m_qstrStartTime,basemsg.m_qstrEndTime);

    QStringList qlistBatRuleName =  CLS_ResManager::getInstance()->GetBatRuleNameFromSshId(sshId);
    QString qstrBatRuleName;
    for(int i = 0; i < qlistBatRuleName.size(); ++i)
    {//列出规则组名称
        qstrBatRuleName +=  QString::number(i+1) + ". " + qlistBatRuleName[i] + "<br/>";
    }

//    QString qstrErrNum   = QString::number(basemsg.m_iErrNum);
    QString qstrFatalNum = QString::number(basemsg.m_iFatalNum);
    QString qstrSeriousNum = QString::number(basemsg.m_iSeriousNum);
    QString qstrSlight = QString::number(basemsg.m_iSlightNum);
    QString qstrNormal = QString::number(basemsg.m_iNormalNum);


    //综述表格将被替换成的文本
    QString qstrOverViewTable;
    qstrOverViewTable = QString("<div class=\"pageOverview_table\">  \
                                        <table border=\"1\">\
                                            <tr>\
                                                <td>检测操作者</td>  <td>%1</td>\
                                            </tr>\
                                            <tr> \
                                                <td>目标机器ip</td>  <td>%2</td>\
                                            </tr>\
                                            <tr> \
                                                <td>开始检测时间</td> <td>%3</td>\
                                            </tr>\
                                            <tr> \
                                                <td>检测耗时</td> <td>%4</td>\
                                            </tr>\
                                            <tr> \
                                                <td>检测规则组</td> <td>%5</td>\
                                            </tr>\
                                            <tr> \
                                                <td><img src=\"./pic/err.png\" width=\"12\" height=\"12\"> <font color=\"#C00A0A\">致命</td> <td>%6个</td>\
                                            </tr>\
                                            <tr> \
                                                <td><img src=\"./pic/serious.png\" width=\"12\" height=\"12\"> <font color=\"#5E005E\">严重</td> <td>%7个</td>\
                                            </tr>\
                                            <tr> \
                                                <td><img src=\"./pic/slight.png\" width=\"12\" height=\"12\"> <font color=\"#A8A800\">轻微</td> <td>%8个</td>\
                                            </tr>\
                                            <tr> \
                                                <td><img src=\"./pic/ok.png\" width=\"12\" height=\"12\"> <font color=\"#017D01\">正常</td> <td>%9个</td>\
                                            </tr>\
                                        </table>\
                                    </div>").arg(qstrname).arg(qstrIp).arg(qstrStartTime).arg(qstrTakeTime).\
                                             arg(qstrBatRuleName).arg(qstrFatalNum).arg(qstrSeriousNum).\
                                             arg(qstrSlight).arg(qstrNormal);



    //基于html模板文件重新生成新的html文件
    QString qstrpath = QCoreApplication::applicationDirPath();
    QFile htmlfile;
    if(QFile::exists(qstrpath + const_Html))
    {
        htmlfile.remove(qstrpath + const_Html);//删除上次使用的html
    }
    htmlfile.copy(qstrpath + const_HtmlTemplate, qstrpath + const_Html); //从模板拷贝一个新的html


    //读取html文件内容
    QFile file(qstrpath + const_Html);
    file.open(QIODevice::ReadOnly);
    QString filecontent = file.readAll();
    file.close();

    //根据替换标记替换文本
    filecontent = filecontent.replace(QRegExp(const_OverViewTableField),qstrOverViewTable);
    filecontent = filecontent.replace(QRegExp(const_DetailTableField),qstrTotalDetail);
    filecontent = filecontent.replace(QRegExp(const_ReportTimeField),PublicGetCurrentTime());

    //保存替换后的html文本
    file.open(QIODevice::WriteOnly);
    file.write(filecontent.toStdString().data());
    file.close();

    //设置导出pdf 源文件 输出文件
    m_expertPdf.SetHtmlPath(qstrpath + const_Html);

    qstrServerIp = qstrServerIp.replace(".","-"); //将ip 的格式从xxx.xxx.xxx.xx 转换为xxx-xxx-xxx 方便写入到文件名中
    m_expertPdf.SetPdfPath(QString("./") + const_PdfDir + QString("/") + QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")+ " "+qstrServerIp+".pdf");

    //转化
    m_expertPdf.Convert();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{ 
    if (event->button() == Qt::LeftButton)
    {
       m_MovePosition = event->globalPos(); //鼠标左键点击的坐标
       m_TitleTopLeft = ui->lab_title->frameGeometry().topLeft(); //标题栏的左上角坐标
       m_TitleTopLeft = this->mapToGlobal(m_TitleTopLeft);

       int imousex = m_MovePosition.x();
       int imousey = m_MovePosition.y();

       int ileftRange  = m_TitleTopLeft.x();
       int itopRange   = m_TitleTopLeft.y();
       int irightRange = m_TitleTopLeft.x() + ui->lab_title->width();
       int ibottomRange =  m_TitleTopLeft.y() + ui->lab_title->height();

       if ((imousex < irightRange) && (imousex > ileftRange) && (imousey > itopRange) && (imousey < ibottomRange))
       {
           m_blMove = true;
       }
     }
     return QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_blMove)
    {
       int xMove = event->globalPos().x() - m_MovePosition.x();
       int yMove = event->globalPos().y() - m_MovePosition.y();

       this->move(m_TitleTopLeft + QPoint(xMove, yMove));
       if(m_pdlgBatchInput)
       {
           m_pdlgBatchInput->hide();
           m_blIsBatchInputShow = false;
       }
    }

    return QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_blMove = false;
    return QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::SlotAllUpdate()
{
     CLS_RuleUpdateManager::getInstance()->SetOperaType(OP_TYPE_UPDATE_ALL);
     QThreadPool::globalInstance()->start(CLS_RuleUpdateManager::getInstance());
}

void MainWindow::SlotAddUpdate()
{
     CLS_RuleUpdateManager::getInstance()->SetOperaType(OP_TYPE_UPDATE_ADD);
     QThreadPool::globalInstance()->start(CLS_RuleUpdateManager::getInstance());
}

void MainWindow::SlotSaveUserInfo(QVariant _qvar)
{
     STRUCT_USER userinfo = _qvar.value<STRUCT_USER>();
     CLS_RuleUpdateManager::getInstance()->SetUserInfo(userinfo);
}

void MainWindow::SlotEchoRes(int _id, QJsonObject _qjson)
{
    /*  树控件每一级节点的叫法
     *  一级节点(规则组节点)
     *  二级节点(规则节点)
     *  三级节点(规则执行结果节点 文本编辑框)
     */

    //解析_qjson
    QJsonObject jsonObj    = _qjson["userdata"].toObject();
    int state              = _qjson["stat"].toInt();             //执行脚本状态
    QString qstrRuleRes    = _qjson["res"].toString();           //获取规则执行结果

    QString ruleId         = jsonObj["ruleid"].toString();       //规则id
    QString batruleId      = jsonObj["batruleid"].toString();    //规则组id


    //根据ssh线程发送的执行状态保存CLS_ResManager内容
    if(state == EXEC_SCRIPT_STAT_GETRES)
    {
        //解析执行结果，按照\n符分割
        QStringList qstrlist = qstrRuleRes.split("\n");
        for(int i = 0; i < qstrlist.size(); ++i)
        {
            QString qstrsingle = qstrlist[i];
            qstrsingle = qstrsingle.trimmed();

            if(qstrsingle.isEmpty())
            {
                continue;
            }

            QString qstrHtmlRes = TransShellToHtml(qstrsingle);
            CLS_ResManager::getInstance()->m_mapRuleRes[batruleId+ruleId] += qstrHtmlRes; //存储规则对应的执行结果(注意要累加结果)
        }
    }
    else if(state == EXEC_SCRIPT_STAT_DONE)
    {
        CLS_ResManager::getInstance()->m_mapSshBasicMsg[_id].m_qstrEndTime = PublicGetCurrentTime();
    }



    //填充主界面执行结果tab页
    QTreeWidget * ptreewgt = m_mapEchoViewIndex[_id];//获取ssh项对应的树状控件
    QTreeWidgetItemIterator treewgtIter(ptreewgt);
    while (*treewgtIter)
    {//遍历树控件上的每一项
        if(((*treewgtIter)->text(0) == CLS_LoadRuleManager::getInstance()->GetRule()[ruleId].m_name) &&
           ((*treewgtIter)->parent()->text(0) == CLS_LoadRuleManager::getInstance()->GetBatRule()[batruleId].m_name))
        {//当前树节点的规则名相同，当前树节点的父节点的规则组名相同

            QTreeWidgetItem * textWgtItem = (*treewgtIter)->child(0);
            CLS_ShowResEdit * ptextEdit = (CLS_ShowResEdit *)ptreewgt->itemWidget(textWgtItem, 0);
            if(ptextEdit)
            {//在第三级节点上填充信息
                ptextEdit->ParseRes(qstrRuleRes); //解析ssh返回的echo信息
                ptextEdit->OutputRes();  //在界面上显示echo信息
            }

           if(state == EXEC_SCRIPT_STAT_DONE)
           {//脚本执行完毕时，判断脚本输出的内容来显示执行脚本功能的图标
                (*treewgtIter)->setIcon(0,QIcon(ptextEdit->CalcExecResState()));
           }
           else if(state == EXEC_SCRIPT_STAT_RUNNING)
           {//脚本正在运行中
                (*treewgtIter)->setIcon(0,QIcon(const_RunningStatePicPath));
           }
           else if(state == EXEC_SCRIPT_STAT_FAIL)
           {//脚本执行错误状态则不设置图标
                (*treewgtIter)->setIcon(0,QIcon());

                 QString msg = "标签页 " + QString::number(_id) + " [规则组名]"+ CLS_LoadRuleManager::getInstance()->GetBatRule()[batruleId].m_name \
                             + " [规则名称]" + CLS_LoadRuleManager::getInstance()->GetRule()[ruleId].m_name + " 脚本上传或执行错误";

                   qDebug()<<"sshid = "
                           <<_id<<"   [规则组名]="
                           <<CLS_LoadRuleManager::getInstance()->GetBatRule()[batruleId].m_name<<" "
                           <<"        [规则名称]="
                           <<CLS_LoadRuleManager::getInstance()->GetRule()[ruleId].m_name<<" "
                           <<" 脚本上传或执行错误";

                   QMessageBox::critical(this,"错误",msg);
           }

           break;
        }
        ++treewgtIter;
    }
}

void MainWindow::SlotSshErrMsg(QString _msg)
{
     QMessageBox::critical(this,"错误",_msg);
}


void MainWindow::SlotBatchAdd()
{
    if(m_pdlgBatchInput == nullptr || m_pSshListView == nullptr)
    {
        return;
    }

    //批量输入，确认按钮
    QString qstrStartIp = m_pdlgBatchInput->GetServerStartIp();
    QString qstrEndIp = m_pdlgBatchInput->GetServerEndIp();

    QRegExp regExpIP("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");

    QRegExpValidator regval(regExpIP);

    int iPos = 0;

    QValidator::State valstate = regval.validate(qstrStartIp, iPos);

    if (valstate != QValidator::Acceptable) {
        QMessageBox::information(this, "提示","起始IP格式不正确", QMessageBox::Ok);
        return;
    }

    valstate = regval.validate(qstrEndIp, iPos);

    if (valstate != QValidator::Acceptable) {
        QMessageBox::information(this, "提示","结束IP格式不正确", QMessageBox::Ok);
        return;
    }

    QStringList startlist = qstrStartIp.split('.');
    QStringList endlist = qstrEndIp.split('.');

    if(startlist.size() < 3 || endlist.size() < 3)
    {
        return;
    }

    int istart = startlist[3].toInt();
    int iend   = endlist[3].toInt();


    for(int i = istart; i <= iend; i++)
    {
        STRUCT_SSH info;
        info.m_qstrIp = QString("%1.%2.%3.%4").arg(startlist[0]).arg(startlist[1]).arg(startlist[1]).arg(i);
        info.m_iPort = m_pdlgBatchInput->GetServerPort().toInt();
        info.m_qstrUsername = m_pdlgBatchInput->GetUser();
        info.m_qstrPassword = m_pdlgBatchInput->GetPwd();
        m_pSshListView->AddItem(info);
    }
}


void MainWindow::on_btn_start_clicked()
{
    if(m_pSshListView == nullptr || m_pSystemSetting == nullptr)
    {//判空操作
        return;
    }

    //获取所有ssh项集合数据
    QMap<int, CLS_WgtSshItem *> pSshItems = m_pSshListView->GetSshItems();


    //遍历pSshItems 判断 m_pSshListView 中是否有被勾选的Ssh项
    auto iter = pSshItems.begin();
    bool blAnySel = false; //是否有任何项被选择  true有选择项   false没有选中项
    while(iter != pSshItems.end())
    {
        if(iter.value() && iter.value()->IsSelect())
        {//如果有一个ssh项被选中 blAnySel置true
            blAnySel = true;
        }
        ++iter;
    }

    if(!blAnySel)
    {
        QMessageBox::warning(this, "提示","没有选中任何条目", QMessageBox::Ok);
        return;
    }




    //首先清理上次tabwidget(显示执行结果的)中的子控件内容，为下次显示做准备
    int iTabCount = this->ui->tabWidget->count();
    for(int i = 0; i < iTabCount; i++)
    {
         QWidget * pWidget = this->ui->tabWidget->widget(i);
         if(pWidget)
         {
             pWidget->deleteLater();
         }
    }
    this->ui->tabWidget->clear();

    auto iterEchoViewIndex = m_mapEchoViewIndex.begin();
    if(iterEchoViewIndex != m_mapEchoViewIndex.end())
    {
         iterEchoViewIndex.value()->clear();
         iterEchoViewIndex.value()->deleteLater();
    }
    m_mapEchoViewIndex.clear();


    //再次遍历ssh项执行选中的项的脚本
    iter = pSshItems.begin();
    while(iter != pSshItems.end())
    {
        CLS_WgtSshItem * pSshItem = iter.value();

        if(pSshItem && pSshItem->IsSelect())
        {//如果ssh项被选中

            CLS_SshManager * pSshMsg = pSshItem->m_sshManager;
            if(pSshMsg == nullptr)
            {
                ++iter;
                continue;
            }

            STRUCT_SSH sshinfo = pSshItem->GetSsh();

            pSshMsg->SetPasswd(sshinfo.m_qstrPassword);
            pSshMsg->SetUserName(sshinfo.m_qstrUsername);
            pSshMsg->SetServerIp(sshinfo.m_qstrIp);
            pSshMsg->SetServerPort(sshinfo.m_iPort);
            pSshMsg->SetSshId(iter.key()); //设置ssh项的id

           //添加tabwidget
           QWidget *tab = new QWidget();
           QVBoxLayout *verticalLayout = new QVBoxLayout(tab);
           verticalLayout->setContentsMargins(0,0,0,0);  //设置控件的外边距

           //在tab页中加入树状控件
           QTreeWidget * ptreewgt = new QTreeWidget(tab);
           ptreewgt->headerItem()->setHidden(true); //隐藏树的表头

           //从CLS_SshManager对象中获取已经选择的规则组
           QVector<QString> vecBatRule = pSshMsg->GetSelBatRule();

           //在树状控件上增加已选择的规则组节点、规则组包含的规则节点、显示规则执行结果的文本编辑框节点
           for(int i = 0; i < vecBatRule.size(); ++i)
           {//遍历已经选择的规则组，并从中获取规则

               //获取规则组
               STRUCT_BATRULE batrule = CLS_LoadRuleManager::getInstance()->GetBatRule()[vecBatRule[i]];

               //创建规则组节点
               QTreeWidgetItem * pItemOne = new QTreeWidgetItem(ptreewgt,QStringList()<<batrule.m_name);

               ptreewgt->expandItem(pItemOne);//设置展开当前节点
               ptreewgt->addTopLevelItem(pItemOne);//将节点加入到树控件中

               //获取规则组包括的规则
               QVector<QString> vecrulesid = batrule.GetMergeRules();
               for(int i = 0; i < vecrulesid.size(); ++i)
               {//遍历规则

                   STRUCT_RULE rule = (CLS_LoadRuleManager::getInstance()->GetRule())[vecrulesid[i]];

                   //创建二级节点
                   QTreeWidgetItem * pItemTwo = new QTreeWidgetItem(pItemOne,QStringList()<<rule.m_name);

                   //设置节点图标为等待状态
                   pItemTwo->setIcon(0,QIcon(const_WaitingStatePicPath));

                   //插入三级节点
                   QTreeWidgetItem * pItemThree = new QTreeWidgetItem(pItemTwo);
                   CLS_ShowResEdit * pedt = new CLS_ShowResEdit(); //三级节点需要文本编辑框存储内容
                   pedt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                   pedt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

                   ptreewgt->setItemWidget(pItemThree, 0, pedt);
                   pItemTwo->addChild(pItemThree);
                   pItemOne->addChild(pItemTwo);
               }
           }

           verticalLayout->addWidget(ptreewgt);
           this->ui->tabWidget->addTab(tab,sshinfo.m_qstrIp); //tab页显示远程服务器的ip
           m_mapEchoViewIndex[iter.key()] = ptreewgt;         //记录ssh项id和显示执行结果tab的关系


           //为导出pdf信息做准备，填充相关信息
           CLS_ResManager::getInstance()->Clear(); //首先清理类成员变量内容，为此次保存信息做准备
           STRUCT_RES pdfres;
           pdfres.m_qstrUser      = m_pSystemSetting->GetOperatorName();
           pdfres.m_qstrTargetIp  = sshinfo.m_qstrIp;
           pdfres.m_qstrStartTime = PublicGetCurrentTime();
           CLS_ResManager::getInstance()->m_mapSshBasicMsg[iter.key()] = pdfres; //填充部分基础信息

           QVector<QString> vectemp = pSshMsg->GetSelBatRule();
           CLS_ResManager::getInstance()->m_mapSshBatRule[iter.key()]  = vectemp;//填充已勾选规则组映射关系

           for(int i = 0; i < vectemp.size(); ++i)  //填充所有勾选规则组对应的规则与执行结果的映射关系
           {
               STRUCT_BATRULE batrule = (CLS_LoadRuleManager::getInstance()->GetBatRule())[vectemp[i]];
               QVector<QString> vecrulesid = batrule.GetMergeRules();
               for(int j = 0; j < vecrulesid.size(); ++j)
               {
                   STRUCT_RULE rule = (CLS_LoadRuleManager::getInstance()->GetRule())[vecrulesid[j]];
                   //因为还没有开始执行所以结果是空，等待执行结束后再填充
                   CLS_ResManager::getInstance()->m_mapRuleRes[batrule.m_sid+rule.m_sid] = QString("");
               }
           }


           //配置ssh并运行(包括上传脚本到对应服务器以及执行上传的文件)
           QThreadPool::globalInstance()->start(pSshMsg);
        }
        ++iter;
    }
}

void MainWindow::on_btn_batchinput_clicked()
{
    if(m_pdlgBatchInput == nullptr)
    {
        return;
    }

    //批量输入
    if(!m_blIsBatchInputShow)
    {//当没有显示界面的时候 点击按钮 显示界面
        QPoint qpos = ui->btn_start->pos();
        qpos = ui->wgt_serverinfo->mapToGlobal(qpos);
        int xpos = qpos.x();
        int ypos = qpos.y() + ui->btn_start->height() + 6;  //这个6是按照UI切图增加
        m_pdlgBatchInput->setGeometry(xpos,ypos,m_pdlgBatchInput->width(),m_pdlgBatchInput->height());
        m_pdlgBatchInput->show();

        m_blIsBatchInputShow = true;
    }
    else
    {//隐藏界面
        m_pdlgBatchInput->hide();
        m_blIsBatchInputShow = false;
    }
}

void MainWindow::on_btn_batchdel_clicked()
{
    if(m_pSshListView == nullptr)
    {
        return;
    }
    //批量删除功能
    m_pSshListView->DeleteSelectItem();
}


void MainWindow::on_btn_minsize_clicked()
{
     this->showMinimized();
}

void MainWindow::on_btn_close_clicked()
{
    this->close();
}

void MainWindow::on_chk_all_stateChanged(int _checkcode)
{
    if(m_pSshListView == nullptr)
    {
        return;
    }

    m_pSshListView->AllSelect((Qt::Checked == _checkcode));
}

void MainWindow::on_btn_systemset_clicked()
{    
    if(m_pStackedLayout == nullptr)
    {
        return;
    }

    int nIndex = m_pStackedLayout->currentIndex();


    //目前层叠布局中就需要切换两个界面，所以这里的逻辑简单实现
    if(nIndex == STACKED_WIDGET_TYPE_SSH)
    {
        nIndex = STACKED_WIDGET_TYPE_SYSTEM;
        ui->lab_serverinfo->setText("系统设置");
        ui->btn_systemset->setText("返回");
    }
    else if(nIndex == STACKED_WIDGET_TYPE_SYSTEM)
    {
        nIndex = STACKED_WIDGET_TYPE_SSH;
        ui->lab_serverinfo->setText("主机设置");
        ui->btn_systemset->setText("系统设置");
    }
    else
    {
        nIndex = STACKED_WIDGET_TYPE_SSH;
        ui->lab_serverinfo->setText("主机设置");
        ui->btn_systemset->setText("系统设置");
    }

    m_pStackedLayout->setCurrentIndex(nIndex);
}


void MainWindow::on_btn_normal_clicked()
{
    if(m_pSystemSetting == nullptr)
    {
        return;
    }

    if (MainWindow::isMaximized())
    {//如果现在主界面处于最大化则在点击按钮后将界面更改为窗口化显示
        MainWindow::showNormal();
        m_pSystemSetting->SetTreeMinSize();
    }
    else
    {//如果现在主界面处于窗口化则在点击按钮后将界面更改为最大化显示
        MainWindow::showMaximized();
        m_pSystemSetting->SetTreeMaxSize();
    }

    //每次切换都要重置拆分布局的尺寸
    ui->splitter->setSizes(QList<int>()<<const_SplitTopInitSize<<const_SplitBottomInitSize);
}



void MainWindow::on_btn_exportcur_clicked()
{
    ui->btn_exportcur->setText("正在导出");
    m_expertPdf.m_blexportAll = false;
    ExportPdfFromTabIndex(ui->tabWidget->currentIndex());
}

void MainWindow::SlotExportFinish(int _type)
{
    if(_type == EXPORT_PDF_NUM_TYPE_MULTI)
    {//当导出多个pdf文件时，还需要判断再继续导出pdf
        if (m_iTabIndex >=  ui->tabWidget->count())
        {
            m_iTabIndex = 0;
            ui->btn_exportall->setText("导出所有");
            QMessageBox::information(this,"提示","pdf文件导出完毕,请及时备份");
            return;
        }
       ExportPdfFromTabIndex(m_iTabIndex);
       m_iTabIndex++;
    }
    else if(_type == EXPORT_PDF_NUM_TYPE_SINGLE)
    {//当导出单个pdf文件时
       QMessageBox::information(this,"提示","pdf文件导出完毕,请及时备份");
       ui->btn_exportcur->setText("导出当前");
    }
}


void MainWindow::on_btn_exportall_clicked()
{
    if (ui->tabWidget->count() == 0)
    {
        m_iTabIndex = 0;
        return;
    }

    ui->btn_exportall->setText("正在导出");
    m_expertPdf.m_blexportAll = true;
    //这里肯定是从tab页签的第一个(索引是0)开始的
    ExportPdfFromTabIndex(0);
    m_iTabIndex++;
}


void MainWindow::on_btn_openpdfdir_clicked()
{
   QString qstrpath = QCoreApplication::applicationDirPath() + QString("/") + const_PdfDir;

   QDir qdir;
   if(!qdir.exists(qstrpath))
   {
      qdir.mkdir(qstrpath);
   }

   QDesktopServices::openUrl(QUrl(qstrpath, QUrl::TolerantMode));
}

