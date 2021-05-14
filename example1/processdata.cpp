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

//Функция, возвращающая соответствие между НЕДЕЛЕЙ(месяцем) и количеством логов
// принимает вектор структур data_vector
QMap<QDate, int> ProcessData::make_week_number_map(const QVector<date_time_type_msg> &data_vector){  // возвращать структуру со счётчиками
    //получаем соотвествие между днём и количеством логов
    QMap<QDate, int> date_number;
    for(auto& structure : data_vector){
        QDate date = structure.date_time.date();
        if(date_number.contains(date)){
            date_number[date] ++;
        } else {
            date_number[date] = 1;
        }
    }
    // считаем количество логов по неделям с первого дня в векторе структур
    QMap<QDate, int> week_number;
    QDate temp_day = data_vector[0].date_time.date();
    QDate temp_seventh_day = temp_day.addDays(7);

    while (temp_seventh_day <= data_vector.last().date_time.date()){ // проходим по целым неделям в диапазоне дат вектора структур
        while(temp_day < temp_seventh_day){
            if(date_number.contains(temp_day)){
                week_number[temp_seventh_day.addDays(-7)] += date_number[temp_day];
                temp_day = temp_day.addDays(1);
            } else{
                week_number[temp_seventh_day.addDays(-7)] += 0;
                temp_day = temp_day.addDays(1);
            }
        }
        temp_seventh_day = temp_seventh_day.addDays(7);
    }
    while(temp_day <= data_vector.last().date_time.date()){    // добавляем значения для оставшихся дней
        if(date_number.contains(temp_day))
            week_number[temp_seventh_day.addDays(-7)] += date_number[temp_day];
        temp_day = temp_day.addDays(1);
    }
    return week_number;
}

//Функция, возвращающая соответствие между 8-ю часами и количеством логов
// принимает вектор структур data_vector
QMap<QDateTime, int> ProcessData::make_hours_number_map(const QVector<date_time_type_msg> &data_vector){  // возвращать структуру со счётчиками
    QMap<QDateTime, int> hours_number;
    if(!data_vector.empty()){
        QVector<QDateTime> time_values; // массив точек времени с промежутком 8 часов (первая точка - первое значение даты в векторе структур)
        QDateTime date_time = data_vector[0].date_time;
        while(date_time <= data_vector.last().date_time){
            time_values.append(date_time);
            date_time = date_time.addSecs(28800);
        }
        // заполняем массив соответствий между отрезками по 8 часов и количествои логов
        QVectorIterator<QDateTime> time_values_it(time_values);
        QDateTime current_date_time = time_values_it.next(); // присваиваем переменной первое значение из вектора с точками времени

        for(auto& structure : data_vector){
            QDateTime temp_date_time = structure.date_time; // дата и время каждого сообщения в лог-файле
            if(temp_date_time >= current_date_time && temp_date_time < current_date_time.addSecs(28800)){   // если значение ещё в текущем диапазоне
                if(hours_number.contains(current_date_time)){
                    hours_number[current_date_time] ++;
                } else {
                    hours_number[current_date_time] = 1;
                }
            }else if (temp_date_time < current_date_time.addSecs(57600) && current_date_time != time_values.last()){  // если значение уже вне диапазона и точка - не последнее значение вектора отрезков, переходим к след.т.
                current_date_time = time_values_it.next();
                if(hours_number.contains(current_date_time)){
                    hours_number[current_date_time] ++;
                } else {
                    hours_number[current_date_time] = 1;
                }
            } else if(temp_date_time >= current_date_time.addSecs(57600) && current_date_time != time_values.last()){ // если перепрыгивает через диапазон
                if(!hours_number.contains(current_date_time.addSecs(28800))){
                    hours_number[current_date_time.addSecs(28800)] = 0; // для данного диапазона отмечаем значение "0" и идём дальше
                }
                current_date_time = time_values_it.next();
                while(temp_date_time >= current_date_time.addSecs(28800) && current_date_time != time_values.last()){
                    current_date_time = time_values_it.next();
                    if(temp_date_time < current_date_time.addSecs(28800)){
                        hours_number[current_date_time] = 1;
                    } else {hours_number[current_date_time] = 0;}
                }
            }
        }
        return hours_number;
    }
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
