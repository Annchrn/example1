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

ReadData::ReadData(const QString& new_filename, const QString& new_combo_box_value)  //конструктор
{
    filename = new_filename;
    QStringList list = filename.split(".");
    file_type = list.at(list.count()-1);
    combo_box_value = new_combo_box_value;
}

//Функция открытия файла
//возвращает true в случае успешного открытия и исключение (диалоговое окно с сообщением) в случае неудачи
bool ReadData::file_open(){
    try {
        if (!filename.isEmpty()){
            QFile file(filename);

            if (file.exists()){
                if (file.open(QIODevice::ReadOnly)){ // если файл существует и открылся для чтения
                     if((file_type == "txt" && combo_box_value == "Документ (разделитель - ;) (*.txt)")
                             || (file_type == "json" && combo_box_value == "JSON (*.json)")){
                         return true;
                     } else{throw std::runtime_error("Выберите верный формат");}
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
//ссылку на массив вершин графика
void ReadData::file_read_json(QMap<QDateTime, int>& values){
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
                QDateTime date;
                date.setDate(QDate(date_values[0].toInt(), date_values[1].toInt(), date_values[2].toInt()));

                QString string_number = obj["Value"].toString();
                int number = string_number.toInt(); //int number = obj["Value"].toInt();

                values[date] = number;
            }
        }catch (std::exception& ex) {
            QMessageBox::critical(0, "Ошибка", ex.what());
        }
        file.close();
}

//Функция чтения файла формата txt
//ссылку на массив вершин графика
void ReadData::file_read_txt(QMap<QDateTime, int>& values){
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QTextStream stream(&file);

    try {
    while (!stream.atEnd()){

            QString line = stream.readLine();
            if(!line.contains(";")){throw std::runtime_error("Неправильный формат данных"); values.clear();}
            QStringList line_values = line.split(";");
            if(line_values.count() != 2){throw std::runtime_error("Неправильный формат данных");}
            QStringList date_values = line_values[0].split(".");
            if(date_values.count() != 3) {throw std::runtime_error("Неправильный формат данных");}
            QDateTime date;
            date.setDate(QDate(date_values[0].toInt(), date_values[1].toInt(), date_values[2].toInt()));
            values[date] = line_values[1].toInt();
         }
          }  catch (std::exception& ex) {
               QMessageBox::critical(0, "Ошибка", ex.what());
               values.clear();
             }
    file.close();
}

//Функция чтения файла, вызывающая file_read_json или file_read_txt в зависимости от типа файла
QMap<QDateTime, int> ReadData::file_read(){
    QMap<QDateTime, int> values;
        if(file_open()){
            if(file_type == "json"){
                file_read_json(values);
            }
            else if(file_type == "txt"){
                file_read_txt(values);
            }
        }
    return values;
}
