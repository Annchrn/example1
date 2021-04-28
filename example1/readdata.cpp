#include "readdata.h"

#include <QFile>
#include <QMessageBox>
#include <QMap>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QRegularExpression>


ReadData::ReadData(const QString& new_filename)  //конструктор
{
    filename = new_filename;
    QStringList list = filename.split(".");
    file_type = list.at(list.count()-1);
}

// Функции доступа
int ReadData::get_logs_count()
{
    return logs_count;
}

int ReadData::get_INF_count()
{
    return INF_count;
}

int ReadData::get_DBG_count()
{
    return DBG_count;
}

int ReadData::get_FTL_count()
{
    return FTL_count;
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

//Функция чтения файла формата json
//принимает ссылку на массив вершин графика
QMap<QDate, int> ReadData::file_read_json(){
        QMap<QDate, int> values;
        QFile file(filename);
        file.open(QIODevice::ReadOnly);

        QByteArray data = file.readAll();
        try{
            QJsonParseError error;
            QJsonDocument jsonDocument = QJsonDocument::fromJson((data), &error);

            if(error.errorString() != "no error occurred"){ // если обнаружена ошибка в считывании файла
                throw std::runtime_error(error.errorString().toStdString());
            }
            QJsonObject jsonObject = jsonDocument.object();
            QJsonArray jsonArray = jsonObject["Dependence"].toArray();  //получаем массив из пар "дата"-"значение"

            foreach (const QJsonValue& value, jsonArray){
                QJsonObject obj = value.toObject();

                QString string_date = obj["Date"].toString();
                QStringList date_values = string_date.split("/");
                QDate date;
                date.setDate(date_values[0].toInt(), date_values[1].toInt(), date_values[2].toInt());

                QString string_number = obj["Value"].toString();
                int number = string_number.toInt(); // int number = obj["Value"].toInt();

                values[date] = number;
            }
        }catch (std::exception& ex) {
            QMessageBox::critical(0, "Ошибка", ex.what());
        }
        file.close();
        return std::move(values);
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
            QRegularExpression reg("(\\d{2}/\\d{2}/\\d{4}) (\\d{2}:\\d{2}:\\d{2}) (INF|DBG|FTL) (.*)");
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

//Функция, возвращающая соответствие между датой и количеством логов
QMap<QDate, int> ReadData::make_date_number_map(QVector<date_time_type_msg> &data_vector){
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

//Функция чтения файла, вызывающая file_read_json или read_txt_file в зависимости от типа файла
QVector<date_time_type_msg> ReadData::file_read(){
    QVector<date_time_type_msg> data_vector;
    if(file_open()){
            if(file_type == "json"){
              // values = file_read_json(values);
            }
            else if(file_type == "txt"){
                data_vector = read_txt_file();
            }
        }
        return data_vector;
}
