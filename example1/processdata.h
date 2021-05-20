#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <QMainWindow>
#include <QMap>

#include "data_structure.h"
#include "Data_Model.h"

class ProcessData
{
public:
    ProcessData(QVector<date_time_type_msg> &data_vector);

    Filters_structure get_filters_struct();
    QMap<QDateTime, QMap<QString, int>> get_chart_map();
    QMap<QDateTime, QString> get_table_map();

private:
    Filters_structure filters_struct;
    QMap<QDateTime, QMap<QString, int>> chart_map;
    QMap<QDateTime, QString> table_map;

    Filters_structure make_filters_structure(const QVector<date_time_type_msg>& data_vector);

    // функции построения данных для графика
    QMap<QDateTime, QMap<QString, int>> make_date_number_map(const QVector<date_time_type_msg>& data_vector);
    QMap<QDateTime, QMap<QString, int>> make_week_number_map(const QVector<date_time_type_msg>& data_vector);
    QMap<QDateTime, QMap<QString, int>> make_hours_number_map(const QVector<date_time_type_msg>& data_vector);
    void count_types(QMap<QDateTime, QMap<QString, int>>& types_map, QString& type, QDateTime& current_date_time);
};

#endif // PROCESSDATA_H
