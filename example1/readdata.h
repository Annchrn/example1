#ifndef READDATA_H
#define READDATA_H

#include <QMainWindow>

#include "data_structure.h"

class ReadData
{
public:
    ReadData(const QString& new_filename);

    QVector<date_time_type_msg> file_read();
    QMap<QDate, int> make_date_number_map(QVector<date_time_type_msg> &data_vector);
    QMap<QDate, int> file_read_json();

    int get_logs_count();
    int get_INF_count();
    int get_DBG_count();
    int get_FTL_count();

private:
    QString filename, file_type;

    bool file_open();
    QVector<date_time_type_msg> read_txt_file();
};

#endif // READDATA_H
