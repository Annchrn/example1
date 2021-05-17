#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <QMainWindow>
#include <QMap>

#include "data_structure.h"
#include "processed_data_struct.h"

class ProcessData
{
public:
    ProcessData(QVector<date_time_type_msg> &data_vector);

    int get_logs_count();
    int get_INF_count();
    int get_DBG_count();
    int get_FTL_count();

    processed_qdatetime make_hours_number_map(const QVector<date_time_type_msg> &data_vector);
    processed_qdate make_week_number_map(const QVector<date_time_type_msg> &data_vector);
    processed_qdate make_date_number_map(const QVector<date_time_type_msg> &data_vector);


private:
    int logs_count = 0, INF_count = 0, DBG_count = 0, FTL_count = 0;

    void count_hours_types(QMap<QDateTime, QMap<QString, int>>& types_map, QString& type, QDateTime& current_date_time);
    void count_dates_types(QMap<QDate, QMap<QString, int>>& types_map, QString& type, const QDate& temp_seventh_day);
};

#endif // PROCESSDATA_H
