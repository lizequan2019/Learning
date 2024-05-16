QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = qt-Inprocess

INCLUDEPATH += ../breakpadInclude

LIBS += -L../breakpadlib -lcrash_generation_server \
        -L../breakpadlib -lcommon \
        -L../breakpadlib -lexception_handler \
        -L../breakpadlib -lcrash_generation_client \
        -L../breakpadlib -lcrash_report_sender


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../breakpadInclude/abstract_class.h \
    ../breakpadInclude/breakpad_types.h \
    ../breakpadInclude/client_info.h \
    ../breakpadInclude/crash_generation_app.h \
    ../breakpadInclude/crash_generation_server.h \
    ../breakpadInclude/exception_handler.h \
    ../breakpadInclude/ipc_protocol.h \
    ../breakpadInclude/minidump_cpu_amd64.h \
    ../breakpadInclude/minidump_cpu_arm.h \
    ../breakpadInclude/minidump_cpu_arm64.h \
    ../breakpadInclude/minidump_cpu_mips.h \
    ../breakpadInclude/minidump_cpu_ppc.h \
    ../breakpadInclude/minidump_cpu_ppc64.h \
    ../breakpadInclude/minidump_cpu_sparc.h \
    ../breakpadInclude/minidump_cpu_x86.h \
    ../breakpadInclude/minidump_exception_fuchsia.h \
    ../breakpadInclude/minidump_exception_linux.h \
    ../breakpadInclude/minidump_exception_mac.h \
    ../breakpadInclude/minidump_exception_ps3.h \
    ../breakpadInclude/minidump_exception_solaris.h \
    ../breakpadInclude/minidump_exception_win32.h \
    ../breakpadInclude/minidump_format.h \
    ../breakpadInclude/minidump_size.h \
    ../breakpadInclude/scoped_ptr.h \
    ../breakpadInclude/minidump_generator.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
