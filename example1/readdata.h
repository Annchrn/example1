#ifndef READDATA_H
#define READDATA_H

#include <QMainWindow>

class ReadData
{
public:
    ReadData(const QString& new_filename, const QString& new_combo_box_value);

    bool file_open();
    void file_read_json(QMap<QDateTime, int>& values);
    void file_read_txt(QMap<QDateTime, int>& values);
    QMap<QDateTime, int> file_read();

private:
    QString filename, file_type, combo_box_value;

};

#endif // READDATA_H
