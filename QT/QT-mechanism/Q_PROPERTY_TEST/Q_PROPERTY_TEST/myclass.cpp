#include "myclass.h"

myclass::myclass()
{
    connect(this,SIGNAL(SignStudent(int)),this,SLOT(slotStudent(int)));
    connect(this,SIGNAL(SignTeacher(int)),this,SLOT(slotTeacher(int)));
    myprint();
}

int myclass::readNumber()
{
    return m_iStudentNumber;
}

void myclass::setNumber(int _iNum)
{
    m_iStudentNumber = _iNum;
}



int myclass::getClassLevel()
{
    return m_iClassLevel;
}



int myclass::readTeacherNumer()
{
    return m_iTeacherNumer;
}

void myclass::setTeacherNumer(int _iNum)
{
    m_iTeacherNumer = _iNum;
}




void myclass::myprint()
{
    m_iTeacherNumer = 1000;
    qDebug()<<" m_iTeacherNumer = "<<m_iTeacherNumer
            <<" QObject::property(\"TeacherNumber\") = "<<QObject::property("TeacherNumber")
            <<" readTeacherNumer= "<<readTeacherNumer();

    m_iStudentNumber = 2000;
    qDebug()<<" m_iStudentNumber = "<<m_iStudentNumber
            <<" QObject::property(\"StudentNumber\") = "<<QObject::property("StudentNumber")
            <<" readNumber = "<<readNumber();

    m_iClassLevel = 3000;
    qDebug()<<" m_iClassLevel = "<<m_iClassLevel
            <<" QObject::property(\"ClassLevel\") = "<<QObject::property("ClassLevel")
            <<" getClassLevel = "<<getClassLevel();

    setNumber(500);
    this->setProperty("StudentNumber", QVariant(4));

    setTeacherNumer(600);
    this->setProperty("TeacherNumber", QVariant(4));

}

void myclass::slotStudent(int _iNum)
{
    qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__<<endl;
}

void myclass::slotTeacher(int _iNum)
{
    qDebug()<<" "<<__LINE__<<" "<<__FUNCTION__<<"_iNum = "<<_iNum<<endl;
}
