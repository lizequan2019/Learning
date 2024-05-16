#include "mainwindow.h"
#include "ui_mainwindow.h"

//要显示的节点内容 json
QString jsontest2 = R"([{"curnodename":"fileserverandclient","curnodetype":2,"curnodepath":"/home","curnodeisroot":1},{"curnodename":"设计文档","curnodetype":2,"curnodepath":"/home/fileserverandclient","curnodeisroot":0},{"curnodename":"设计图.drawio","curnodetype":1,"curnodepath":"/home/fileserverandclient/设计文档","curnodeisroot":0},{"curnodename":"设计记录.md","curnodetype":1,"curnodepath":"/home/fileserverandclient/设计文档","curnodeisroot":0},{"curnodename":"关于文件树的json协议.md","curnodetype":1,"curnodepath":"/home/fileserverandclient/设计文档","curnodeisroot":0},{"curnodename":"调试文档","curnodetype":2,"curnodepath":"/home/fileserverandclient","curnodeisroot":0},{"curnodename":"如何测试安卓客户端.md","curnodetype":1,"curnodepath":"/home/fileserverandclient/调试文档","curnodeisroot":0},{"curnodename":"学习adb调试.md","curnodetype":1,"curnodepath":"/home/fileserverandclient/调试文档","curnodeisroot":0},{"curnodename":"README.md","curnodetype":1,"curnodepath":"/home/fileserverandclient","curnodeisroot":0},{"curnodename":"source-code","curnodetype":2,"curnodepath":"/home/fileserverandclient","curnodeisroot":0},{"curnodename":"client-code","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code","curnodeisroot":0},{"curnodename":"Android-client","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code/client-code","curnodeisroot":0},{"curnodename":"client","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client","curnodeisroot":0},{"curnodename":"main.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"CLS_Socket2.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"CLS_Socket.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"CLS_Socket.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"libevent_h","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"include.am","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"event2","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"rpc_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"bufferevent.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"bufferevent_ssl.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"listener.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"thread.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"dns_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"buffer_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"http_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"bufferevent_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"dns.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"tag.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"event_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"visibility.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"rpc.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"buffer.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"rpc_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"http.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"tag_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"keyvalq_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"http_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"util.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"event-config.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"dns_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"bufferevent_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"event_compat.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"event.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h/event2","curnodeisroot":0},{"curnodename":"evdns.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"evutil.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"evhttp.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"evrpc.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"event.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_h","curnodeisroot":0},{"curnodename":"mainwindow.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"CLS_Socket2.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"mainwindow.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"libevent_a","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"libevent.a","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client/libevent_a","curnodeisroot":0},{"curnodename":"mainwindow.ui","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"client.pro","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client/client","curnodeisroot":0},{"curnodename":"keystore密码是111111.txt","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client","curnodeisroot":0},{"curnodename":"客户端采用android+qt+libevent的开发.txt","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/Android-client","curnodeisroot":0},{"curnodename":"demo-client","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code/client-code","curnodeisroot":0},{"curnodename":"linux版简易客户端编译.sh","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/demo-client","curnodeisroot":0},{"curnodename":"a.out","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/demo-client","curnodeisroot":0},{"curnodename":"demo-client.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/client-code/demo-client","curnodeisroot":0},{"curnodename":"server-code","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code","curnodeisroot":0},{"curnodename":"business.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"main.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"safe_queue.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"thread_pool.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"file_server.out","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"task.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"task.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"safe_queue.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"编译脚本.sh","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"thread_pool.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"business.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/server-code","curnodeisroot":0},{"curnodename":"public-header","curnodetype":2,"curnodepath":"/home/fileserverandclient/source-code","curnodeisroot":0},{"curnodename":"cJSON.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/public-header","curnodeisroot":0},{"curnodename":"public_fun.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/public-header","curnodeisroot":0},{"curnodename":"public_data.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/public-header","curnodeisroot":0},{"curnodename":"cJSON.c","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/public-header","curnodeisroot":0},{"curnodename":"public_fun.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/public-header","curnodeisroot":0},{"curnodename":"public_struct.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/source-code/public-header","curnodeisroot":0},{"curnodename":".gitignore","curnodetype":1,"curnodepath":"/home/fileserverandclient","curnodeisroot":0},{"curnodename":"functional-research","curnodetype":2,"curnodepath":"/home/fileserverandclient","curnodeisroot":0},{"curnodename":"filetreetojson","curnodetype":2,"curnodepath":"/home/fileserverandclient/functional-research","curnodeisroot":0},{"curnodename":"cJSON.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/filetreetojson","curnodeisroot":0},{"curnodename":"cJSON.c","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/filetreetojson","curnodeisroot":0},{"curnodename":"mytreetojson.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/filetreetojson","curnodeisroot":0},{"curnodename":"调研获取文件树目录并转换成json文件","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/filetreetojson","curnodeisroot":0},{"curnodename":"qt-treewgt-use","curnodetype":2,"curnodepath":"/home/fileserverandclient/functional-research","curnodeisroot":0},{"curnodename":"main.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/qt-treewgt-use","curnodeisroot":0},{"curnodename":"qt-treewgt-use.pro","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/qt-treewgt-use","curnodeisroot":0},{"curnodename":"mainwindow.h","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/qt-treewgt-use","curnodeisroot":0},{"curnodename":"mainwindow.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/qt-treewgt-use","curnodeisroot":0},{"curnodename":"mainwindow.ui","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/qt-treewgt-use","curnodeisroot":0},{"curnodename":"readfile","curnodetype":2,"curnodepath":"/home/fileserverandclient/functional-research","curnodeisroot":0},{"curnodename":"readfile.cpp","curnodetype":1,"curnodepath":"/home/fileserverandclient/functional-research/readfile","curnodeisroot":0}])";

//根据json内容创建树节点
bool AccordJsonCreateTreeNode(QTreeWidget * _ptreewgt, const QString & _json)
{
    //自定义的结构体
    typedef struct{
        QTreeWidgetItem * pwgtitem;
        QString  nodename;
        QString  nodepath;
        uint     nodetype;
        uint     nodeisroot;
    }nodemsg;

    QJsonDocument qjsondoc = QJsonDocument::fromJson(_json.toStdString().c_str());
    QJsonArray qjsonary;

    if(qjsondoc.isArray()){
        qjsonary = qjsondoc.array();
    }else{
        return false;
    }

    QQueue<nodemsg> nodequeue;
    for(int i = 0; i < qjsonary.size(); ++i){
        QJsonObject qjsonobj = qjsonary[i].toObject();

        QString curnodename   = qjsonobj["curnodename"].toString();
        QString curnodepath   = qjsonobj["curnodepath"].toString();
        uint    curnodetype   = qjsonobj["curnodetype"].toInt();
        uint    curnodeisroot = qjsonobj["curnodeisroot"].toInt();

        QTreeWidgetItem * pwgtitem = new QTreeWidgetItem;//每一个目录或文件都创建一个wgtitem,但是没有设置父
        nodemsg _nodemsg;

        _nodemsg.nodename = curnodename;
        _nodemsg.nodepath = curnodepath;
        pwgtitem->setText(0,curnodename);
        _nodemsg.pwgtitem = pwgtitem;
        _nodemsg.nodetype = curnodetype;
        _nodemsg.nodeisroot = curnodeisroot;

        nodequeue.push_back(_nodemsg);
    }

    for(int i = 0; i < nodequeue.size(); ++i){//向treewgt中添加根节点
        nodemsg _nodemsgfst = nodequeue[i];
        if(_nodemsgfst.nodeisroot == true && _nodemsgfst.nodetype == 2){
            _ptreewgt->addTopLevelItem(_nodemsgfst.pwgtitem);
        }
    }

    for(int i = 0; i < nodequeue.size(); ++i){
        nodemsg _nodemsgele = nodequeue[i];
        if(_nodemsgele.nodetype == 0 || _nodemsgele.nodetype == 1){
            continue;
        }

        for(int j = i+1; j < nodequeue.size(); ++j){
            nodemsg _nodemsgeleson = nodequeue[j];
            QString compare1 = _nodemsgele.nodepath + "/" + _nodemsgele.nodename;
            QString compare2 = _nodemsgeleson.nodepath;

            if(compare1.compare(compare2) == 0 &&
               _nodemsgeleson.nodetype != 0 &&
               _nodemsgele.pwgtitem != nullptr &&
               _nodemsgeleson.pwgtitem != nullptr){
                 _nodemsgele.pwgtitem->addChild(_nodemsgeleson.pwgtitem);
            }
        }
    }

    return true;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTreeWidget * ptreewgt = ui->treeWidget;
    ptreewgt->setColumnCount(2);                  //设置显示列数
    ptreewgt->setHeaderLabels({"Name","Level"});  //设置列标题为"Name","Level"

#if 1

    ui->btn_create_treeitem->setEnabled(false);
    ui->btn_delete_treeitem->setEnabled(false);

    /*  这一部分代码是调研qt-treewgt的基本用法 */

    //创建一个根节点China
    QTreeWidgetItem * root = new QTreeWidgetItem;
    root->setText(0, "China");   //设置节点在索引为0的节点
    root->setText(1, "country"); //设置节点在索引为1的节点
    ptreewgt->addTopLevelItem(root);

        //创建root的一级子节点 ZhgJiakou
        QTreeWidgetItem * root_child1 = new QTreeWidgetItem;
        root_child1->setText(0, "ZhangJiakou");
        root_child1->setText(1, "prefecture-level city");
        root->addChild(root_child1);

                //创建c1_child1的一级节点
                QTreeWidgetItem * root_child1_child1 = new QTreeWidgetItem;
                root_child1_child1->setText(0, "XuanHua");
                root_child1_child1->setText(1, "county");
                root_child1->addChild(root_child1_child1);

        //创建root的一级子节点 Beijing
        QTreeWidgetItem * root_child2 = new QTreeWidgetItem;
        root_child2->setText(0, "Beijing");
        root_child2->setText(1, "capital");
        root->addChild(root_child2);

    //===============================================================

    //创建另一个根节点USA
    QTreeWidgetItem * root2 = new QTreeWidgetItem;
    root2->setText(0, "USA");     //设置节点在索引为0的节点
    root2->setText(1, "country"); //设置节点在索引为1的节点
    ptreewgt->addTopLevelItem(root2);

        //创建root2的一级子节点ZhgJiakou
        QTreeWidgetItem * root2_child1 = new QTreeWidgetItem;
        root2_child1->setText(0, "California");
        root2_child1->setText(1, "prefecture-level city");
        root2->addChild(root2_child1);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}


//删除treewgt的节点
void removeTreeNode(QTreeWidget * _pTtreeWidget)
{
    if(_pTtreeWidget == nullptr){
        return;
    }

    //获取顶级item数量
    unsigned int topitemnum = _pTtreeWidget->topLevelItemCount();

    //遍历
    for(unsigned int i = 0; i < topitemnum; ++i){
        QTreeWidgetItem * topitem=_pTtreeWidget->topLevelItem(0);//每次都取最上面的节点,treewgt删除一个会自己顶上来
        if(topitem == nullptr){
            continue;
        }

        //获取每个顶级item下子item数量 下面这个for循环不写也许,删除节点会销毁子节点
        int childitemcount = topitem->childCount();
        for(unsigned int j = 0; j < childitemcount; ++j){
            QTreeWidgetItem * childitem = topitem->child(0);
            if(childitem == nullptr){
                 continue;
            }

            topitem->removeChild(childitem);
            delete childitem;
            childitem = nullptr;
        }

        _pTtreeWidget->removeItemWidget(topitem,0);
        delete topitem;
        topitem = nullptr;
    }
}

void MainWindow::on_btn_create_treeitem_clicked()
{
    QTreeWidget * ptreewgt = ui->treeWidget;

    for(int i = 0; i < 1000; ++i){
        AccordJsonCreateTreeNode(ptreewgt, jsontest2);
    }
}


void MainWindow::on_btn_delete_treeitem_clicked()
{
    QTreeWidget * ptreewgt = ui->treeWidget;
    removeTreeNode(ptreewgt);
}

