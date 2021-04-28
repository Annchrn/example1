#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <QMainWindow>
#include <QMap>
#include "data_structure.h"

class ProcessData
{
public:
    ProcessData(QVector<date_time_type_msg> &data_vector);

private:
    int logs_count = 0, INF_count = 0, DBG_count = 0, FTL_count = 0;

    QMap<QDate, int> make_date_number_map(QVector<date_time_type_msg> &data_vector);

};

#endif // PROCESSDATA_H
