#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QByteArray>
#include <QDialog>
#include "CLS_DlgCalendarRadioBox.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ParsingObjectStructure(const QObject *obj, int spaceCount = 0);
private slots:
    void on_btn_changestyle_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    CLS_DlgCalendarRadioBox * calendar;
};
#endif // MAINWINDOW_H
