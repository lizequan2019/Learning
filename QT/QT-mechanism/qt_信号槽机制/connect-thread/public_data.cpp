#include "public_data.h"

QString CurTime(){

    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyyy-MM-dd hh:mm:ss");
}
