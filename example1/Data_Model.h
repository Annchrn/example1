#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <QString>
#include <QMap>
#include <QDateTime>
#include <data_structure.h>

struct Filters_structure{
    QMap<QString, int> types_map;
    QMap<QString, int> levels_map;
    QMap<QString, int> users_map;
    QMap<QString, int> servers_map;
};

struct Data_Model{
    int time_range;
    QMap<QDateTime, QMap<QString, int>> chart_map;
    Filters_structure filters_struct;
    QVector<date_time_type_msg> data_vector;
};

#endif // DATA_MODEL_H
