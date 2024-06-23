#include "mainwindow.h"
#include "ui_mainwindow.h"



#if _MSC_VER >= 1600 //VS2015>VS>VS2010, MSVC VER= 10.0 -14.0
#pragma execution_character_set("utf-8")
#endif

/*
 *   递归遍历目录下的文件
 *   _qstr_root_path   根目录
 *   _qlist_file       存储遍历到的文件(路径+文件名)
 *   _qstr_exclude_dir 不包含的目录名 可以是多个形式如 AAA,BBB
 */
void listFilesRecursively(const QString & _qstr_root_path, QStringList & _qlist_filepath_store, const QStringList _qstr_exclude_dir){

    QDir dir(_qstr_root_path);

    // 获取当前目录下所有文件和目录
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (QFileInfo entry : entries) {
        if (entry.isFile()) {
            bool bl_skip = false;

            for(int i = 0; i < _qstr_exclude_dir.size(); ++i){//判断文件所在的是不是不包含目录,是的话下面就不存储了
                if(!_qstr_exclude_dir[i].isEmpty() && entry.filePath().contains(_qstr_exclude_dir[i])){
                    bl_skip = true;
                }
            }

            if(!bl_skip){
                _qlist_filepath_store << entry.filePath();
            }

        } else if (entry.isDir()) {
            //如果是目录就递归下去
            listFilesRecursively(entry.filePath(),_qlist_filepath_store,_qstr_exclude_dir);
        }
    }
}

/*
 *   获取文件内容中的链接文件
 *   _qstr_file_name         文件名(路径+名称)
 *   _qlist_linkfile_store   存储查到的链接路径(路径+文件名)
 */
void getLocalLinkFileFromFile(const QString & _qstr_file_name, QStringList & _qlist_linkfile_store){

    QFile file(_qstr_file_name);
    QFileInfo fileinfo(file);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<__FUNCTION__<<" "<<__LINE__<<" "<<_qstr_file_name<<" file open error";
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");//设置内容编码

    while (!in.atEnd()) {
        QString line = in.readLine();//按行读取
        if((line.contains("<img src") || line.contains("<a href")) && !line.contains("http")){

            //匹配的字符串大概都是这个形式
            //<p><a href="../../../../C&C++Basicknowledge/C++语法专题/C++自身语法/【C++】const特性总结.html">【本地链接】【C++】const特性总结</a></p>

            QRegExp regex(R"(".+")"); //正则表达式是 ".+" 就是获取 "../../../../C&C++Basicknowledge/C++语法专题/C++自身语法/【C++】const特性总结.html" 这部分
            QStringList matchingFiles;

            int pos = 0; // 从字符串的开头开始搜索
            if((pos = regex.indexIn(line, pos)) != -1) {//line中只会有一个子项,所以不用需要循环查找
               QString match = line.mid(pos, regex.matchedLength());

               match.remove(0,1); //去除头尾的引号
               match.chop(1);

               QString linkfilepath = fileinfo.absolutePath() + "/" + match;//拼接字符串
               _qlist_linkfile_store<<linkfilepath;
            }
        }
    }
    file.close();
}




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->edt_res->setReadOnly(true);
    ui->btn_markdown_ch->setToolTip(QString::fromUtf8("检测笔记中存在的markdown笔记,需要将md格式的笔记转换成html"));
    ui->btn_filelink_ch->setToolTip(QString::fromUtf8("检测笔记中(html格式文件)本地链接文件是否存在"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_markdown_ch_clicked()
{
    QString qstr_path = ui->edt_filecheck_path->text();

    QStringList qlist_file;

    //遍历文件
    listFilesRecursively(qstr_path, qlist_file,  ui->edt_ch_exclude->text().split(","));

    QRegExp regex(".+\\.md");
    QStringList matchingFiles;

    //过滤文件名
    for (const QString &fileName : qlist_file) {
        if (regex.exactMatch(fileName)) {
            matchingFiles << fileName;
        }
    }

    //显示以.md为后缀的文件路径
    ui->edt_res->clear();
    for (const QString &file : matchingFiles) {
        ui->edt_res->append(file);
    }
}


void MainWindow::on_btn_filelink_ch_clicked()
{
    //要查找的目录
    QString qstr_path = ui->edt_filecheck_path->text();

    //遍历出来的文件
    QStringList qlist_file;

    //链接文件
    QStringList linkfilepath;

    listFilesRecursively(qstr_path, qlist_file,  ui->edt_ch_exclude->text().split(","));

    for(int i = 0; i < qlist_file.size(); ++i){
        if(qlist_file[i].contains(".html")){//只处理html文件
            getLocalLinkFileFromFile(qlist_file[i],linkfilepath);
        }
    }

    ui->edt_res->clear();
    for(int i = 0; i < linkfilepath.size(); ++i){
        qDebug()<<linkfilepath[i];
        if(!QFile::exists(linkfilepath[i])){
            ui->edt_res->append("文件 "+linkfilepath[i]+" 不存在");
        }
    }
}

