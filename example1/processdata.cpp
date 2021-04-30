#include "processdata.h"

ProcessData::ProcessData(QVector<date_time_type_msg> &data_vector)
{
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

//Функция, возвращающая соответствие между датой и количеством логов
// принимает вектор структур data_vector
QMap<QDate, int> ProcessData::make_date_number_map(const QVector<date_time_type_msg> &data_vector){  // возвращать структуру со счётчиками
    QMap<QDate, int> date_number;
    for(auto& structure : data_vector){
        QDate date = structure.date_time.date();
        if(date_number.contains(date)){
            date_number[date] ++;
        } else {
            date_number[date] = 1;
        }
    }
    return date_number;
}

// Функция группировки сообщений по 8 часов
// возвращает структуру qmap(время(начало каждых 8 часов) - количество сообщений за промежуток) и счётчики


// функция группировки по месяцам
// возвращает структуру qmap и счётчики

// функция группировки по неделям
// возвращает структуру qmap и счётчики


//Функция, возвращающая воктор сообщений с меткой INF
// get_INF_logs


//Функция, возвращающая воктор сообщений с меткой
// get_DBG_logs


//Функция, возвращающая воктор сообщений с меткой
// get_FTL_logs


//Функция, возвращающая воктор сообщений с меткой
// get_INF_DBG_logs


//Функция, возвращающая воктор сообщений с меткой
// get_INF_FTL_logs


//Функция, возвращающая воктор сообщений с меткой
// get_DBG_FTL_logs







