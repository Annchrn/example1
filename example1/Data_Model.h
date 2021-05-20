#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <QString>
#include <QMap>
#include <QDateTime>

struct Filters_structure{
    QMap<QString, int> types_map;
    QMap<QString, int> levels_map;
    QMap<QString, int> users_map;
};

struct Data_Model{
    QMap<QDateTime, QMap<QString, int>> chart_map;
    Filters_structure filters_struct;
    QMap<QDateTime, QString> table_map;
};


#endif // DATA_MODEL_H
