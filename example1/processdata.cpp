#include "processdata.h"
#include <QDebug>

ProcessData::ProcessData(QVector<date_time_type_msg> &data_vector)
{
    if(!data_vector.empty()){
        for (const auto& structure : data_vector){
            if (structure.type == "INF")
                INF_count ++;
            if (structure.type == "DBG")
                DBG_count ++;
            if (structure.type == "FTL")
                FTL_count ++;
            logs_count ++;
        }
    }
}

// Функции доступа
int ProcessData::get_logs_count()
{
    return logs_count;
}

int ProcessData::get_INF_count()
{
    return INF_count;
}

int ProcessData::get_DBG_count()
{
    return DBG_count;
}

int ProcessData::get_FTL_count()
{
    return FTL_count;
}

void ProcessData::count_dates_types(QMap<QDate, QMap<QString, int>>& types_map, QString& type, const QDate& temp_date)
{
    if(!types_map.contains(temp_date)){
        types_map[temp_date]["INF"] = 0;
        types_map[temp_date]["DBG"] = 0;
        types_map[temp_date]["FTL"] = 0;
    }
    if(type == "INF")
        types_map[temp_date]["INF"]++;
    if(type == "DBG")
        types_map[temp_date]["DBG"]++;
    if(type == "FTL")
        types_map[temp_date]["FTL"]++;
}

 QMap<QDate, QMap<QString, int>> ProcessData::make_date_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDate, QMap<QString, int>> values_map;

    for(auto& structure : data_vector){
        QDate date = structure.date_time.date();
        QString type = structure.type;
        count_dates_types(values_map, type, date);
    }
  //  for(auto key : values_map.keys())
   //     qDebug() << key << values_map.value(key);
    return values_map;
}

 //Функция, возвращающая структуру, содержащую соответствие между НЕДЕЛЕЙ и количеством логов + количество сообщений каждого типа за каждую неделю
// принимает вектор структур data_vector
QMap<QDate, QMap<QString, int>> ProcessData::make_week_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDate, QMap<QString, int>> types_map;

    QDate temp_day = data_vector[0].date_time.date();

    for(auto structure : data_vector){
        QDate date = structure.date_time.date();
        QString type = structure.type;
        if (date < temp_day.addDays(7)){
            count_dates_types(types_map, type, temp_day);
        } else {
            if(date < temp_day.addDays(7)){ // если не перескакиваем через неделю
                temp_day = temp_day.addDays(7);
                count_dates_types(types_map, type, temp_day);
            } else{
                temp_day = temp_day.addDays(7);
                while(date > temp_day){
                    if (date < temp_day.addDays(7))
                        break;
                    types_map[temp_day]["INF"] = 0;
                    types_map[temp_day]["DBG"] = 0;
                    types_map[temp_day]["FTL"] = 0;
                    temp_day = temp_day.addDays(7);
                }
                //записываем значение для temp_date
                count_dates_types(types_map, type, temp_day);
            }
        }
    }
    return types_map;
}


void ProcessData::count_hours_types(QMap<QDateTime, QMap<QString, int>>& types_map, QString& type, QDateTime& current_date_time)
{
    if(!types_map.contains(current_date_time)){
        types_map[current_date_time]["INF"] = 0;
        types_map[current_date_time]["DBG"] = 0;
        types_map[current_date_time]["FTL"] = 0;
    }
    if(type == "INF")
        types_map[current_date_time]["INF"]++;
    if(type == "DBG")
        types_map[current_date_time]["DBG"]++;
    if(type == "FTL")
        types_map[current_date_time]["FTL"]++;
}


//Функция, возвращающая соответствие между 8-ю часами и количеством логов
// принимает вектор структур data_vector
QMap<QDateTime, QMap<QString, int>> ProcessData::make_hours_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDateTime, QMap<QString, int>> types_map;

    QDateTime temp_day = data_vector[0].date_time;

    for(auto structure : data_vector){
        QDateTime date = structure.date_time;
        QString type = structure.type;
        if (date < temp_day.addSecs(28800)){
            count_hours_types(types_map, type, temp_day);
        } else {
            if(date < temp_day.addSecs(28800)){ // если не перескакиваем через неделю
                temp_day = temp_day.addSecs(28800);
                count_hours_types(types_map, type, temp_day);
            } else{
                temp_day = temp_day.addSecs(28800);
                while(date > temp_day){
                    if (date < temp_day.addSecs(28800))
                        break;
                    types_map[temp_day]["INF"] = 0;
                    types_map[temp_day]["DBG"] = 0;
                    types_map[temp_day]["FTL"] = 0;
                    temp_day = temp_day.addSecs(28800);
                }
                //записываем значение для temp_date
                count_hours_types(types_map, type, temp_day);
            }
        }
    }
    return types_map;
}

//Функция, возвращающая воктор сообщений с меткой INF
// get_INF_logs
