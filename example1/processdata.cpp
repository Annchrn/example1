#include "processdata.h"
#include <QDebug>

ProcessData::ProcessData(QVector<date_time_type_msg> &data_vector, const QDateTime& begining, const QDateTime& ending)
{
    // строим модель для блока фильтров
    for(const auto& structure : data_vector){
        // заполняем данные для фильтров
        filters_struct.types_map[structure.type]++;
        if(structure.user != "")
            filters_struct.users_map[structure.user]++;
        if(structure.session_level != "")
            filters_struct.levels_map[structure.session_level]++;
        if(structure.server_name != "")
            filters_struct.servers_map[structure.server_name]++;
    }
    // строим модель для графика
    // диапазон времени между первой и последней записью в секундах
    time_range = begining.secsTo(ending);
    if(time_range <= 259200){
        chart_map = make_hours_number_map(data_vector, begining, ending);
    } else {
        if(time_range > 259200 && time_range < 2678400){
            chart_map = make_date_number_map(data_vector, begining, ending);
        } else {
            chart_map = make_week_number_map(data_vector, begining, ending);
        }
     }
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
void ProcessData::count_types(QMap<QDateTime, QMap<QString, int>>& types_map, QString& type, const QDateTime& current_date_time)
{
    if(!types_map.contains(current_date_time)){
        for(const auto& type_from_list : filters_struct.types_map.keys())
            types_map[current_date_time][type_from_list] = 0;
    }
    types_map[current_date_time][type] ++;
}

void ProcessData::fill_map_with_nulls(QMap<QDateTime, QMap<QString, int>>& types_map, const QDateTime& temp_date_time)
{
    for(const auto& type : filters_struct.types_map.keys()){
        types_map[temp_date_time][type] = 0;
    }
}

//Функция, возвращающая соответствие между датой (начиная с первого дня в лог-файле) и количеством сообщений каждого типа
// принимает вектор структур data_vector
 QMap<QDateTime, QMap<QString, int>> ProcessData::make_date_number_map(const QVector<date_time_type_msg>& data_vector, const QDateTime& begining, const QDateTime& ending){
    QMap<QDateTime, QMap<QString, int>> values_map;

    QDateTime temp_date = begining;
    while(temp_date.date() < data_vector[0].date_time.date()){
        fill_map_with_nulls(values_map, temp_date);
        temp_date = temp_date.addDays(1);
    }

    for(const auto& structure : data_vector){  // проходим по вектору структур (все сообщения лог-файла)
        QDate date = structure.date_time.date();
        QString type = structure.type;
        QDateTime date_time(date);
        count_types(values_map, type, date_time);
    }

    temp_date = data_vector.last().date_time.addDays(1);
    while(temp_date.date() <= ending.date()){
        fill_map_with_nulls(values_map, temp_date);
        temp_date = temp_date.addDays(1);
    }
    return values_map;
}

 //Функция, возвращающая соответствие между НЕДЕЛЯМИ (начиная с первого дня в лог-файле) и количеством сообщений каждого типа
// принимает вектор структур data_vector
QMap<QDateTime, QMap<QString, int>> ProcessData::make_week_number_map(const QVector<date_time_type_msg>& data_vector, const QDateTime& begining, const QDateTime& ending){
    QMap<QDateTime, QMap<QString, int>> types_map;
    QDate temp_day = begining.date();

    // если начальная дата меньше, чем дата первого сообщения в лог-файле, заполняем недостающие промежутки нулевыми значениями
    while(temp_day < data_vector[0].date_time.date()){
        QDateTime temp_date_time(temp_day);
        fill_map_with_nulls(types_map, temp_date_time);
        if(temp_day.addDays(7) < data_vector[0].date_time.date()){
            temp_day = temp_day.addDays(7);
        } else {
            break;
        }
    }
    for(const auto& structure : data_vector){ // проходим по вектору структур (все сообщения лог-файла)
        QDate date = structure.date_time.date();
        QString type = structure.type;
        if (date < temp_day.addDays(7)){ // если значение вектора структур попадает в текущую "неделю"
            QDateTime temp_date_time(temp_day);
            count_types(types_map, type, temp_date_time);
        } else {
            if(date < temp_day.addDays(7)){ // если начинается след. неделя, но не перескакиваем через неделю
                temp_day = temp_day.addDays(7);
                QDateTime temp_date_time(temp_day);
                count_types(types_map, type, temp_date_time);
            } else{         //  если перескакиваем через неделю или больше
                temp_day = temp_day.addDays(7);
                while(date > temp_day){
                    if (date < temp_day.addDays(7))
                        break;
                    QDateTime temp_date_time(temp_day);
                    fill_map_with_nulls(types_map, temp_date_time);
                    temp_day = temp_day.addDays(7);
                }
                //записываем значение для temp_day
                QDateTime temp_date_time(temp_day);
                count_types(types_map, type, temp_date_time);
            }
        }
    }    
    // если диапазон не закончился, заполним оставшиеся промежутки в types_map нулевыми значениями
     if(types_map.keys().last().addDays(7) < ending){
         while(temp_day.addDays(7) <= ending.date()){
             temp_day = temp_day.addDays(7);
             QDateTime temp_date_time(temp_day);
             fill_map_with_nulls(types_map, temp_date_time);
         }
     }
    return types_map;
}

//Функция, возвращающая соответствие между промежутками по 8 часов (начиная с первой записи в лог-файле) и количеством сообщений каждого типа
// принимает вектор структур data_vector
QMap<QDateTime, QMap<QString, int>> ProcessData::make_hours_number_map(const QVector<date_time_type_msg>& data_vector, const QDateTime& begining, const QDateTime& ending){
    QMap<QDateTime, QMap<QString, int>> types_map;
    QDateTime temp_date_time = begining;

    // если начальная дата меньше, чем дата первого сообщения в лог-файле, заполняем недостающие промежутки нулевыми значениями
    while(temp_date_time < data_vector[0].date_time){
        fill_map_with_nulls(types_map, temp_date_time);
        if(temp_date_time.addSecs(28800) < data_vector[0].date_time){
            temp_date_time = temp_date_time.addSecs(28800);
        } else {
            break;
        }
    }

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

    // если диапазон не закончился, заполним оставшиеся промежутки в types_map нулевыми значениями
     if(types_map.keys().last().addSecs(28800) < ending){
         while(temp_date_time.addSecs(28800) <= ending){
             temp_date_time = temp_date_time.addSecs(28800);
             fill_map_with_nulls(types_map, temp_date_time);
         }
     }

    return types_map;
}
