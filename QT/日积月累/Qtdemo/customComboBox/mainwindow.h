#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CLS_MultiSelectComboBox.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void on_pushButton_clicked();

    void SlotSelChange(SELECT_TYPE,const QString&);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    MultiSelectComboBox * m_pCombox;
};
#endif // MAINWINDOW_H
