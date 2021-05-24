#include "processdata.h"
#include <QDebug>

ProcessData::ProcessData(QVector<date_time_type_msg> &data_vector)
{
    // строим модель для блока фильтров
    for(const auto& structure : data_vector){
        // заполняем данные для фильтров
        filters_struct.types_map[structure.type]++;
    }
    // строим модель для графика
    int days_range = data_vector[0].date_time.date().daysTo(data_vector.last().date_time.date()); // количество дней в лог-файле
    if(days_range <= 3){
        chart_map = make_hours_number_map(data_vector);
    } else {
        if(days_range > 3 && days_range < 31){
            chart_map = make_date_number_map(data_vector);
        } else {
            chart_map = make_week_number_map(data_vector);
        }
     }
    // диапазон времени между первой и последней записью в секундах
    time_range = data_vector[0].date_time.secsTo(data_vector.last().date_time);
}

//Функции доступа
Filters_structure ProcessData::get_filters_struct(){
    return filters_struct;
}

QMap<QDateTime, QMap<QString, int>> ProcessData::get_chart_map(){
    return chart_map;
}

int ProcessData::get_time_range(){
    return time_range;
}

// Функция заполнения соответствий между типами и количеством сообщений каждого типа
void ProcessData::count_types(QMap<QDateTime, QMap<QString, int>>& types_map, QString& type, QDateTime& current_date_time)
{
    if(!types_map.contains(current_date_time)){
        for(const auto& type_from_list : filters_struct.types_map.keys())
            types_map[current_date_time][type_from_list] = 0;
    }
    types_map[current_date_time][type] ++;
}

//Функция, возвращающая соответствие между датой (начиная с первого дня в лог-файле) и количеством сообщений каждого типа
// принимает вектор структур data_vector
 QMap<QDateTime, QMap<QString, int>> ProcessData::make_date_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDateTime, QMap<QString, int>> values_map;

    for(const auto& structure : data_vector){  // проходим по вектору структур (все сообщения лог-файла)
        QDate date = structure.date_time.date();
        QString type = structure.type;
        QDateTime date_time(date);
        count_types(values_map, type, date_time);
    }
    return values_map;
}

 //Функция, возвращающая соответствие между НЕДЕЛЯМИ (начиная с первого дня в лог-файле) и количеством сообщений каждого типа
// принимает вектор структур data_vector
QMap<QDateTime, QMap<QString, int>> ProcessData::make_week_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDateTime, QMap<QString, int>> types_map;

    QDate temp_day = data_vector[0].date_time.date();

    for(const auto& structure : data_vector){ // проходим по вектору структур (все сообщения лог-файла)
        QDate date = structure.date_time.date();
        QString type = structure.type;
        if (date < temp_day.addDays(7)){
            QDateTime temp_date_time(temp_day);
            count_types(types_map, type, temp_date_time);
        } else {
            if(date < temp_day.addDays(7)){ // если не перескакиваем через неделю
                temp_day = temp_day.addDays(7);
                QDateTime temp_date_time(temp_day);
                count_types(types_map, type, temp_date_time);
            } else{
                temp_day = temp_day.addDays(7);
                while(date > temp_day){
                    if (date < temp_day.addDays(7))
                        break;
                    for(const auto& type : filters_struct.types_map.keys()){
                        QDateTime temp_date_time(temp_day);
                        types_map[temp_date_time][type] = 0;
                    }
                    temp_day = temp_day.addDays(7);
                }
                //записываем значение для temp_day
                QDateTime temp_date_time(temp_day);
                count_types(types_map, type, temp_date_time);
            }
        }
    }
    return types_map;
}

//Функция, возвращающая соответствие между промежутками по 8 часов (начиная с первой записи в лог-файле) и количеством сообщений каждого типа
// принимает вектор структур data_vector
QMap<QDateTime, QMap<QString, int>> ProcessData::make_hours_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDateTime, QMap<QString, int>> types_map;

    QDateTime temp_date_time = data_vector[0].date_time;

    for(auto structure : data_vector){   // проходим по вектору структур (все сообщения лог-файла)
        QDateTime date = structure.date_time;
        QString type = structure.type;
        if (date < temp_date_time.addSecs(28800)){
            count_types(types_map, type, temp_date_time);
        } else {
            if(date < temp_date_time.addSecs(28800)){ // если не перескакиваем через неделю
                temp_date_time = temp_date_time.addSecs(28800);
                QString str_temp_date_time = temp_date_time.toString();
                count_types(types_map, type, temp_date_time);
            } else{
                temp_date_time = temp_date_time.addSecs(28800);
                while(date > temp_date_time){
                    if (date < temp_date_time.addSecs(28800))
                        break;
                    for(const auto& type : filters_struct.types_map.keys())
                        types_map[temp_date_time][type] = 0;
                    temp_date_time = temp_date_time.addSecs(28800);
                }
                //записываем значение для temp_date
                QString str_temp_date_time = temp_date_time.toString();
                count_types(types_map, type, temp_date_time);
            }
        }
    }
    return types_map;
}
