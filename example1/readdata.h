#ifndef READDATA_H
#define READDATA_H

#include <QMainWindow>

#include "data_structure.h"
// #include "data_structure"

class ReadData
{
public:
    ReadData(const QString& new_filename);

    QVector<date_time_type_msg> file_read();
    QMap<QDate, int> make_date_number_map(QVector<date_time_type_msg> &data_vector);
    QMap<QDate, int> file_read_json();

private:
    QString filename, file_type;

    bool file_open();
    QVector<date_time_type_msg> read_txt_file();


};

#endif // READDATA_H
