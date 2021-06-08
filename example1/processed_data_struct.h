#ifndef PROCESSED_DATA_STRUCT_H
#define PROCESSED_DATA_STRUCT_H

#include <QDateTime>
#include <QMap>

struct processed_qdate {
    QMap<QDate, int> values;
    QMap<QDate, QMap<QString, int>> type_counters;
};

struct processed_qdatetime {
    QMap<QDateTime, int> values;
    QMap<QDateTime, QMap<QString, int>> type_counters;
};


#endif // PROCESSED_DATA_STRUCT_H
