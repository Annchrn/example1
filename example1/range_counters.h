#ifndef RANGE_COUNTERS_H
#define RANGE_COUNTERS_H

#include <QMap>
#include <QDateTime>

struct range_counters {
    QMap<QDateTime, int> time_counter;
    QString type;
    QString message;
};


#endif // RANGE_COUNTERS_H
