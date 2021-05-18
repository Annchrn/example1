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

//Функция, возвращающая соответствие между датой и количеством логов
// принимает вектор структур data_vector
processed_qdate ProcessData::make_date_number_map(const QVector<date_time_type_msg>& data_vector){
    processed_qdate processed_data_structure;
    QMap<QDate, int> date_number;
    QMap<QDate, QMap<QString, int>> types_map;

    for(auto& structure : data_vector){
        QDate date = structure.date_time.date();
        QString type = structure.type;
        if(date_number.contains(date)){
            date_number[date] ++;
        } else {
            date_number[date] = 1;
        }
        count_dates_types(types_map, type, date);
    }

    processed_data_structure.values = date_number;
    processed_data_structure.type_counters = types_map;
    return processed_data_structure;
}


 QMap<QDate, QMap<QString, int>> ProcessData::emake_date_number_map(const QVector<date_time_type_msg>& data_vector){
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
QMap<QDate, QMap<QString, int>> ProcessData::emake_week_number_map(const QVector<date_time_type_msg>& data_vector){
    QMap<QDate, int> week_number;
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

    for(auto key : types_map.keys())
        qDebug() << key << types_map.value(key);
    return types_map;
}


 //Функция, возвращающая структуру, содержащую соответствие между НЕДЕЛЕЙ и количеством логов + количество сообщений каждого типа за каждую неделю
 // принимает вектор структур data_vector
 processed_qdate ProcessData::make_week_number_map(const QVector<date_time_type_msg>& data_vector){
     processed_qdate processed_data_structure;
     QMap<QDate, int> week_number;
     QMap<QDate, QMap<QString, int>> types_map;

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
     QDate temp_day = data_vector[0].date_time.date();
     QDate temp_seventh_day = temp_day.addDays(7);

     while (temp_seventh_day <= data_vector.last().date_time.date()){ // проходим по неделям в диапазоне дат вектора структур
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

    for(auto key : week_number.keys())
         qDebug() << key << week_number.value(key);

     //
     QList<QDate> list_dates = week_number.keys(); // даты по неделям
     QListIterator<QDate> temp_date_it(list_dates);
     QDate temp_date = temp_date_it.next();
     for(auto& structure : data_vector){
         QString type = structure.type;
         if (structure.date_time.date() <= temp_date || temp_date==list_dates.last()){
             count_dates_types(types_map, type, temp_date);
         } else {
             temp_date = temp_date_it.next();
             count_dates_types(types_map, type, temp_date);
         }
     }


     for(auto key : types_map.keys())
         qDebug() << key << types_map.value(key);

     processed_data_structure.values = week_number;
     processed_data_structure.type_counters = types_map;
     return processed_data_structure;
 }



 //Функция, возвращающая соответствие между 8-ю часами и количеством логов
// принимает вектор структур data_vector
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

processed_qdatetime ProcessData::make_hours_number_map(const QVector<date_time_type_msg> &data_vector){
    processed_qdatetime processed_data_structure;
    QMap<QDateTime, int> hours_number;
    QMap<QDateTime, QMap<QString, int>> types_map;

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
            QString type = structure.type;

            if(temp_date_time >= current_date_time && temp_date_time < current_date_time.addSecs(28800)){   // если значение ещё в текущем диапазоне
                if(hours_number.contains(current_date_time)){
                    hours_number[current_date_time] ++;
                } else {
                    hours_number[current_date_time] = 1;
                }
                count_hours_types(types_map, type, current_date_time);
            }else if (temp_date_time < current_date_time.addSecs(57600) && current_date_time != time_values.last()){  // если значение уже вне диапазона и точка - не последнее значение вектора отрезков, переходим к след.т.
                current_date_time = time_values_it.next();
                if(hours_number.contains(current_date_time)){
                    hours_number[current_date_time] ++;
                } else {
                    hours_number[current_date_time] = 1;
                }
                count_hours_types(types_map, type, current_date_time);
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
                count_hours_types(types_map, type, current_date_time);
            }
        }

        processed_data_structure.values = hours_number;
        processed_data_structure.type_counters = types_map;
    }
           return processed_data_structure;
}

//Функция, возвращающая воктор сообщений с меткой INF
// get_INF_logs
