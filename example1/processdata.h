#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <QMainWindow>
#include <QMap>
#include "data_structure.h"

class ProcessData
{
public:
    ProcessData(QVector<date_time_type_msg> &data_vector);

    int get_logs_count();
    int get_INF_count();
    int get_DBG_count();
    int get_FTL_count();

    QMap<QDate, int> make_date_number_map(const QVector<date_time_type_msg> &data_vector);

private:
    int logs_count = 0, INF_count = 0, DBG_count = 0, FTL_count = 0;

};

#endif // PROCESSDATA_H
