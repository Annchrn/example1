#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <QDateTime>

struct date_time_type_msg {
    QDateTime date_time;
    QString type;
    QString message;
    QString user;
    QString session_level;
    QString server_name;
};

#endif // DATA_STRUCTURE_H
