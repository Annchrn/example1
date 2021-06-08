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

private:
    QString filename;

    bool file_open();
    QVector<date_time_type_msg> read_txt_file();
    void process_reg_match(const QString& reg_match, date_time_type_msg& new_struct);
};

#endif // READDATA_H
