#include "readdata.h"

#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QRegularExpression>


ReadData::ReadData(const QString& new_filename)  //конструктор
{
    filename = new_filename;
    QStringList list = filename.split(".");
    file_type = list.at(list.count()-1);
}

//Функция открытия файла
//возвращает true в случае успешного открытия и исключение (диалоговое окно с сообщением) в случае неудачи
bool ReadData::file_open(){
    try {
        if (!filename.isEmpty()){
            QFile file(filename);

            if (file.exists()){
                if (file.open(QIODevice::ReadOnly)){ // если файл существует и открылся для чтения
                           return true;
                   } else {throw std::runtime_error("Не удалось открыть файл");}
             } else {throw std::runtime_error("Файл не существует");}
            file.close();
        } else {throw std::runtime_error("Введите путь к файлу");}
    }  catch (std::exception& ex) {
        QMessageBox::critical(0, "Ошибка", ex.what());
        return false;
    }
}

//Функция чтения лог-файла в формате txt
// возвращает вектор структур записей лог-файла
QVector<date_time_type_msg> ReadData::read_txt_file(){
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);

    QVector<date_time_type_msg> v_data;
    // заполнение вектора типов лог-записей
    try {
    while (!stream.atEnd()){
            QString line = stream.readLine();
            QRegularExpression reg("(\\d{2}/\\d{2}/\\d{4}) (\\d{2}:\\d{2}:\\d{2}) (INF|DBG|FTL|WRN|ALL) (.*)"); // тут считывать просто слово
            QRegularExpressionMatch reg_match = reg.match(line);

            if(reg_match.hasMatch()){
                date_time_type_msg new_struct;
                // запись даты и времени
                QString date_str = reg_match.captured(1);
                QStringList date_list = date_str.split("/");
                new_struct.date_time.setDate(QDate(date_list[2].toInt(), date_list[1].toInt(), date_list[0].toInt()));
                QString time_str = reg_match.captured(2);
                QStringList time_list = time_str.split(":");
                new_struct.date_time.setTime(QTime(time_list[0].toInt(),time_list[1].toInt(),time_list[2].toInt()));
                // запись типа и сообщения
                new_struct.type = reg_match.captured(3);
                new_struct.message = reg_match.captured(4);
                v_data.push_back(new_struct);
              } else {
                  throw std::runtime_error("Ошибка чтения данных");
              }
         }
         }catch (std::exception& ex) {
               QMessageBox::critical(0, "Ошибка", ex.what());
         }
    file.close();
    return v_data;
}

//Функция чтения файла, вызывающая file_read_json или read_txt_file в зависимости от типа файла
QVector<date_time_type_msg> ReadData::file_read(){
    QVector<date_time_type_msg> data_vector;
    if(file_open()){
            if(file_type == "json"){
            }
            else if(file_type == "txt"){
                data_vector = read_txt_file();
            }
        }
        return data_vector;
}
