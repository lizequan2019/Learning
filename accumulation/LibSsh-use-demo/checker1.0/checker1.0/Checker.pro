QT       += core gui network webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Checker1.0

CONFIG(debug, debug|release) {

    win32:msvc
    {
        contains(QT_ARCH, i386){
            MOC_DIR = ./win32_msvc_release_moc_debug
            OBJECTS_DIR = ./win32_msvc_release_obj_debug
            DESTDIR = ./debug

        } else {
            DESTDIR = ./debug

        }
    }

}
CONFIG(release, debug|release) {

    win32:msvc {

        contains(QT_ARCH, i386) {
            MOC_DIR = ./release/win32_msvc_release_moc_release
            OBJECTS_DIR = ./release/win32_msvc_release_obj_release
            DESTDIR =  ./release

        } else {
           #DESTDIR = ./release
        }
    }
}

SOURCES += \
    CLS_DlgBatchInput.cpp \
    CLS_DownloadManager.cpp \
    CLS_HtmlToPdf.cpp \
    CLS_LoadRuleManager.cpp \
    CLS_MultiSelectComboBox.cpp \
    CLS_PublicFun.cpp \
    CLS_ResManager.cpp \
    CLS_RuleUpdateManager.cpp \
    CLS_ShowResEdit.cpp \
    CLS_SshManager.cpp \
    CLS_WebServiceManager.cpp \
    CLS_WgtSshItem.cpp \
    CLS_WgtSshList.cpp \
    CLS_WgtSystemSetting.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    CLS_DlgBatchInput.h \
    CLS_DownloadManager.h \
    CLS_HtmlToPdf.h \
    CLS_LoadRuleManager.h \
    CLS_MultiSelectComboBox.h \
    CLS_PublicFun.h \
    CLS_ResManager.h \
    CLS_RuleUpdateManager.h \
    CLS_ShowResEdit.h \
    CLS_SshManager.h \
    CLS_WebServiceManager.h \
    CLS_WgtSshItem.h \
    CLS_WgtSshList.h \
    CLS_WgtSystemSetting.h \
    PublicData.h \
    mainwindow.h \
    libssh2.h \          # libssh2 需要的头文件
    libssh2_config.h \
    libssh2_publickey.h \
    libssh2_sftp.h \

FORMS += \
    CLS_DlgBatchInput.ui \
    CLS_WgtSystemSetting.ui \
    mainwindow.ui

LIBS += -L./ -llibssh2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RC_ICONS = CherckerIcon.ico
