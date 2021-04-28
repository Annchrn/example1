#include "processdata.h"



ProcessData::ProcessData(QVector<date_time_type_msg> &data_vector)
{


}

/*
                if (new_struct.type == "INF")
                    INF_count ++;
                if (new_struct.type == "DBG")
                    DBG_count ++;
                if (new_struct.type == "FTL")
                    FTL_count ++;
logs_count ++;
*/

//Функция, возвращающая соответствие между датой и количеством логов
// принимает вектор структур data_vector
QMap<QDate, int> ProcessData::make_date_number_map(QVector<date_time_type_msg> &data_vector){
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
