#ifndef READDATA_H
#define READDATA_H

#include <QMainWindow>
#include <mainwindow.h>
#include "data_structure.h"

class ReadData
{
public:
    ReadData(const QString& new_filename);

    QVector<date_time_type_msg> file_read();
    QMap<QDate, int> file_read_json();

private:
    QString filename, file_type;

    bool file_open();
    QVector<date_time_type_msg> read_txt_file();
};

#endif // READDATA_H
