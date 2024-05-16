#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QDebug>

class myclass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int StudentNumber READ readNumber WRITE setNumber NOTIFY SignStudent)
    Q_PROPERTY(int TeacherNumber MEMBER m_iTeacherNumer NOTIFY SignTeacher)
    Q_PROPERTY(int ClassLevel READ getClassLevel)

public:
    myclass();

    int readNumber();
    void setNumber(int _iNum);

    int getClassLevel();

    int readTeacherNumer();
    void setTeacherNumer(int _iNum);

    void myprint();
public Q_SLOTS:

    void slotStudent(int _iNum);
    void slotTeacher(int _iNum);

private:
    int m_iTeacherNumer;
    int m_iStudentNumber;
    int m_iClassLevel;

signals:
    void SignStudent(int _iNum);
    void SignTeacher(int _iNum);
};

#endif // MYCLASS_H
