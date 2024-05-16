#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QThreadPool>
#include <QCheckBox>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDropEvent>
#include <QMimeData>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QSplitter>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QDir>


#include "CLS_ResManager.h"
#include "CLS_ShowResEdit.h"
#include "CLS_WgtSshList.h"
#include "CLS_DlgBatchInput.h"
#include "CLS_RuleUpdateManager.h"
#include "CLS_LoadRuleManager.h"
#include "CLS_WgtSystemSetting.h"
#include "CLS_HtmlToPdf.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    /******************************************************
    函数 :  ExportPdfFromTabIndex
    功能描述 :  将主界面上执行结果下的一个tab页内容导出成pdf
    输入参数 :  int tab页的索引
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void ExportPdfFromTabIndex(int);

public slots:
    /******************************************************
    函数 :  SlotBatchAdd
    功能描述 : 批量增加ssh项
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotBatchAdd();

    //点击批量输入按钮响应的槽函数
    void on_btn_batchinput_clicked();

    //点击批量删除按钮响应的槽函数
    void on_btn_batchdel_clicked();

    /******************************************************
    函数 :  SlotBatchAdd
    功能描述 : 响应点击"开始执行"按钮，并执行规则(组)
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void on_btn_start_clicked();

    /******************************************************
    函数 :  on_chk_all_stateChanged
    功能描述 : 响应点击"全选"按钮
    输入参数 : int qt框架定义的check码  Qt::Checked
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void on_chk_all_stateChanged(int);

    //系统设置按钮响应
    /******************************************************
    函数 :  on_btn_systemset_clicked
    功能描述 : 响应点击"系统设置"按钮
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/

    /******************************************************
    函数 :  SlotAllUpdate
    功能描述 :  响应全量更新
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotAllUpdate();

    /******************************************************
    函数 :  SlotAddUpdate
    功能描述 :  响应增量更新
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotAddUpdate();

    /******************************************************
    函数 :  SlotSaveUserInfo
    功能描述 :  响应保存用户信息
    输入参数 : QVariant 用户信息 此数据时自定义的数据结构
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotSaveUserInfo(QVariant);


    /******************************************************
    函数 :  SlotEchoRes
    功能描述 : 响应信号显示对应的ssh项执行脚本的结果
    输入参数 : int ssh项的id号   QJsonObject 要显示json数据
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotEchoRes(int, QJsonObject);


    /******************************************************
    函数 :  SlotSshErrMsg
    功能描述 : 响应信号显示对应的ssh错误信息
    输入参数 : QString 错误信息
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotSshErrMsg(QString);

    /******************************************************
    函数 :  SlotExportFinish
    功能描述 :  接收m_expertPdf对象发出的信号，此槽函数主要是用于导出tabwidget中所有的tab页信息，
               每一个tab页会被导出一个pdf文件，直至遍历完毕
    输入参数 : 无
    输出参数 : 无
    返回值 : 无
    其它 : 无
    *******************************************************/
    void SlotExportFinish(int);

    //主界面尺寸最大化、正常化
    void on_btn_normal_clicked();

    //导出单个tab页为pdf
    void on_btn_exportcur_clicked();

    //导出多个所有tab页为pdf
    void on_btn_exportall_clicked();

    void on_btn_systemset_clicked();

    void on_btn_minsize_clicked();

    void on_btn_close_clicked();
private:

    Ui::MainWindow *ui;

    //主界面层叠布局 切换显示系统设置和ssh设置
    QStackedLayout * m_pStackedLayout;

    //批量输入界面指针
    CLS_DlgBatchInput * m_pdlgBatchInput;

    //ssh项列表界面的指针
    CLS_WgtSshList * m_pSshListView;

    //系统设置界面指针
    CLS_WgtSystemSetting  * m_pSystemSetting;

    //是否显示批量输入界面
    bool m_blIsBatchInputShow;

    //批量操作界面跟随位置参数
    QPoint m_MovePosition;
    QPoint m_TitleTopLeft;
    bool m_blMove;


    //ssh项id与回显执行结果界面树控件的对应关系
    //int ssh项id    QTreeWidget(树控件 存在于tabwidget中,每个tabwidget中有一个树控件)
    QMap<int, QTreeWidget *> m_mapEchoViewIndex;

    //用于html 转换 pdf 的类
    CLS_HtmlToPdf m_expertPdf;

    //用于导出所有pdf时使用，记录tab页索引
    int m_iTabIndex;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);
private slots:

    //打开pdf文件所在目录
    void on_btn_openpdfdir_clicked();
};
#endif // MAINWINDOW_H
