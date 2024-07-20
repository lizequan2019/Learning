#ifndef PUBLICDATA_H
#define PUBLICDATA_H

#include  <QColor>
#include  <QMap>
#include  <QString>
#include  <QVector>
#include  <QObjectUserData>


const int const_SysTemTreeMinWidthSize = 1174;
const int const_SysTemTreeMinHighSize  = 513;

const int const_SysTemTreeMaxWidthSize = 1896;
const int const_SysTemTreeMaxHighSize  = 800;

const int const_SplitTopInitSize    = 30;   //主界面分割布局上部分初始化尺寸
const int const_SplitBottomInitSize = 500;  //主界面分割布局下部分初始化尺寸


enum STACKED_WIDGET_TYPE
{
    STACKED_WIDGET_TYPE_SSH,    //ssh设置界面
    STACKED_WIDGET_TYPE_SYSTEM  //系统设置界面
};


const QColor const_black  = QColor(0,0,0);     //黑色 #000000
const QColor const_red    = QColor(192,10,10); //红色 #C00A0A   致命错误
const QColor const_green  = QColor(1,125,1);   //绿色 #017D01   正常
const QColor const_yellow = QColor(168,168,0); //黄色 #A8A800   轻微警告
const QColor const_blue   = QColor(0,0,168);   //蓝色 #0000A8
const QColor const_purple = QColor(94,0,94);   //紫色 #5E005E   严重警告
const QColor const_cyan   = QColor(0,168,168); //青色 #00A8A8
const QColor const_white  = QColor(255,255,255); //白色 #FFFFFF

//等级颜色与html颜色的映射
const QString const_HtmlRed_Fatal      = "#C00A0A";  //QColor(192,10,10);
const QString const_HtmlGreen_Normal   = "#017D01";  //QColor(1,125,1);
const QString const_HtmlYellow_Slight  = "#A8A800";  //QColor(168,168,0);
const QString const_HtmlPurple_Serious = "#5E005E";  //QColor(94,0,94);


//shell 标准前景色号
const QMap<int,QColor> const_SHELLCOLOR {
    {30,const_black},  //黑
    {31,const_red},    //红
    {32,const_green},  //绿
    {33,const_yellow}, //黄
    {34,const_blue},   //蓝
    {35,const_purple}, //紫
    {36,const_cyan},   //青
    {37,const_white}   //白
};

//界面UI上勾选框绑定的用户数据
struct STRUCT_CK_USER_DATA
{
     QString m_sid;
};
Q_DECLARE_METATYPE(STRUCT_CK_USER_DATA)

//ssh 数据结构
struct STRUCT_SSH
{
    QString m_qstrIp;
    int m_iPort;
    QString m_qstrUsername;
    QString m_qstrPassword;

    STRUCT_SSH()
    {
        m_qstrIp = "";
        m_qstrUsername = "root";
        m_qstrPassword = "";
        m_iPort = 22;
    }
};

//规则基类
struct STRUCT_BASERULE
{
    QString m_sid;          //id号
    QString m_name;         //名称
    QString m_version;      //版本
    QString m_description;  //描述
    QString m_updateUser;   //更新者
    QString m_updateTime;   //更新时间
    QString m_createTime;   //创建时间
    QString m_createUser;   //创建者
    int m_isDel;            //是否删除标志  1表示已经被删除  0表示没有删除
};

//规则结构体
struct STRUCT_RULE : public STRUCT_BASERULE
{
    //将从m_url下载的文件重命名为  md5值+原文件名，程序中使用整个文件名
    //此字段的主要防止文件名重复而添加，注意此数据服务器上没有此字段
    QString m_scriptfilename;
    QString m_url;
    QString m_md5;
};

//规则组结构体
struct STRUCT_BATRULE : public STRUCT_BASERULE
{
    QVector<QString> m_rulesid;         //QString 存储的是m_containrules 和 m_inheritrules 叠加之后去掉重复的规则集合
    QVector<QString> m_containrulesid;  //QString 存储的是规则组本身自带规则的id  对应字段[ruleVOS]
    QVector<QString> m_inheritrulesid;  //QString 存储的是继承其他规则组的规则的id
                                        //此变量对应的字段是[parentRuleVOS]值是一个数组，程序每次更新数据后m_inheritrule中的内容应该和parentRuleVOS数组中
                                        //的内容一致。
                                        //例如更新后 m_inheritrule = [1,2,3,4]，则表示此处规则组关联额外的1、2、3、4四个规则
                                        //再次更新后 m_inheritrule = [], 则表示此处规则组没有关联额外的规则
                                        //注意 parentRuleVOS数组中规则没有字段delFlg，所以此数组内容的改变不会触发本地缓存规则文件的删除增加，只是表示规则组与规则是否还有关联关系。

    //获取合并去重后的规则集合
    QVector<QString> GetMergeRules()
    {
        for(int i = 0; i < m_containrulesid.size(); ++i)
        {
            m_rulesid.push_back(m_containrulesid[i]);
        }

        for(int i = 0; i < m_inheritrulesid.size(); ++i)
        {
            if(m_containrulesid.contains(m_inheritrulesid[i]))
            {//重复的就不加了
                continue;
            }

            m_rulesid.push_back(m_inheritrulesid[i]);
        }

        return m_rulesid;
    }

};


//用户信息结构体
struct STRUCT_USER
{
    QString m_id;
    QString m_name;
    QString m_passwd;
    QString m_note;
    QString m_serverIp;
    QString m_port;
};
Q_DECLARE_METATYPE(STRUCT_USER)


//线程操作类型
enum OP_TYPE
{
    OP_TYPE_UPDATE_ALL,  //全量更新类型
    OP_TYPE_UPDATE_ADD,  //增量更新类型
};

//访问webservice结果
enum ACCESS_WEBSER_RES
{
    ACCESS_WEBSER_RES_OK, //访问正确
    ACCESS_WEBSER_RES_TIMEOUT,//访问响应超时
    ACCESS_WEBSER_RES_ERR, //服务器返回错误
};


//自定义下拉框显示数据类型
enum SHOW_DATA_TYPE
{
    SHOW_DATA_TYPE_NO,        //不显示任何类型数据
    SHOW_DATA_TYPE_RULE,      //显示规则类数据
    SHOW_DATA_TYPE_BATRULE,   //显示规则组类数据
};

//自定义下拉框对勾选框的操作
enum SELECT_TYPE
{
    SELECT_TYPE_SEL,       //勾选
    SELECT_TYPE_REMOVESEL, //去除勾选
};

//执行脚本状态
enum EXEC_SCRIPT_STAT
{
    EXEC_SCRIPT_STAT_RUNNING, //脚本运行中
    EXEC_SCRIPT_STAT_GETRES,  //接收到远程服务器返回的执行结果
    EXEC_SCRIPT_STAT_DONE,    //脚本运行完毕
    EXEC_SCRIPT_STAT_OK,      //脚本运行成功
    EXEC_SCRIPT_STAT_FAIL     //脚本运行失败(注意是脚本本身执行，而不是其中的功能执行)
};

//保存着规则和规则组的json缓存文件，文件内容的获取和更新都是通过服务器获得
//如果文件存在则可以进行全量更新或增量更新，不存在只能进行全量更新
const char const_CacheFilePath[] = "./rule.json";

//本地sh脚本文件按存储的路径
const char const_LocalShFilePath[] = "./ScriptFile/";

//远程服务器存放文件目录 注意最后的/符号
const char const_ServerShFilePath[] = "/tmp/ShellFileForChecker/";

//服务器返回报文中状态码
const char const_ServerRetCodeSuccess[] = "API-COMMON-INF-OK";

//界面允许最小存在的Ssh项控件个数
const int  const_AllowMinExistNum = 1;

//执行结果界面显示的状态图标路径
const char const_RunningStatePicPath[] = "./pic/running.svg"; //运行中
const char const_WaitingStatePicPath[] = "./pic/waiting.svg"; //等待

const char const_OkStatePicPath[] = "./pic/ok.png";           //正常
const char const_SlightStatePicPath[] = "./pic/slight.png";   //轻微
const char const_SeriousStatePicPath[] = "./pic/serious.png"; //严重
const char const_ErrStatePicPath[] = "./pic/err.png";         //致命
//const char const_FailStatePicPath[] = "./pic/fail.png";       //脚本执行失败 【TODO 此图标目前不使用，等到有需要再使用】

//判断脚本执行成功失败的echo字段
//const char const_SSH_EXEC_SUCCESS[] = "SSH_EXEC_SUCCESS";
//const char const_SSH_EXEC_FAIL[] = "SSH_EXEC_FAIL";



//html模板文件,  此文件不能被程序修改
const char const_HtmlTemplate[] = "/pageTemplate.html";

//html实例化文件，此文件从html模板文件拷贝来，程序通过替换其中内容再使用
const char const_Html[] = "/page.html";

//html文件中综述表格替换字段
const char const_OverViewTableField[] = "<!--PAGEOVERVIEW REPLACE-->";

//html文件中详情表格替换字段
const char const_DetailTableField[] = "<!--PAGEDETAIL REPLACE-->";

//html文件中报表生成时间字段
const char const_ReportTimeField[] = "<!--REPORTTIME REPLACE-->";

//导出pdf存放的目录  为了方便其他位置方便使用，这里只是把目录写出来了
const char const_PdfDir[] = "PdfDir";

//"delFlg"的值等于1 表示规则(组)数据已弃用，
const int const_Deleted = 1;

//libssh2 上传 执行脚本失败 尝试的最大次数
const int const_TryLimitNum = 5;

//导出pdf需要的结构体信息
struct STRUCT_RES
{
    //检测者
    QString m_qstrUser;

    //目标机器Ip
    QString m_qstrTargetIp;

    //开始检测时间
    QString m_qstrStartTime;

    //检测结束时间
    QString m_qstrEndTime;

    //执行持续时间
    QString m_qstrTakeTime;

    //执行错误脚本的个数
    //int m_iErrNum;     【TODO 目前不使用，等到有需要再使用】

    //致命错误的个数
    int m_iFatalNum;

    //严重警告的个数
    int m_iSeriousNum;

    //轻微警告的个数
    int m_iSlightNum;

    //正常的个数
    int m_iNormalNum;

    STRUCT_RES()
    {
//        m_iErrNum     = 0;
        m_iFatalNum   = 0;
        m_iSeriousNum = 0;
        m_iSlightNum  = 0;
        m_iNormalNum  = 0;
    }
};


//ip 格式检测结果
enum CHECK_IP_RES
{
    CHECK_IP_RES_OK,
    CHECK_IP_RES_EMPTY = -1,
    CHECK_IP_RES_ERR = -2,
};


//导出pdf 数量类型
enum EXPORT_PDF_NUM_TYPE
{
    EXPORT_PDF_NUM_TYPE_SINGLE,   //导出当前的pdf(1个)
    EXPORT_PDF_NUM_TYPE_MULTI     //导出所有的pdf(多个)
};

#endif // PUBLICDATA_H
