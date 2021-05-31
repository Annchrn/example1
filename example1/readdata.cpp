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
            QRegularExpression reg("(\\d{2}/\\d{2}/\\d{4}) (\\d{2}:\\d{2}:\\d{2}) (\\w+) (.*)");
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
               // new_struct.message = reg_match.captured(4);
               process_reg_match(reg_match.captured(4), new_struct);
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

void ReadData::process_reg_match(const QString& reg_match, date_time_type_msg& new_struct){
    QRegularExpression user_reg ("(User=) (.*) (SessionLevel=)"); // проверяем, есть ли запись с тэгом "User"
    QRegularExpressionMatch user_reg_match = user_reg.match(reg_match);
    if(user_reg_match.hasMatch()){
        new_struct.user = user_reg_match.captured(2);
    } else{
        QRegularExpression user_reg1 ("(User=) (.*) (ServerName=)");
        QRegularExpressionMatch user_reg_match1 = user_reg.match(reg_match);
        if(user_reg_match1.hasMatch()){
            new_struct.user = user_reg_match1.captured(2);
        } else {
            QRegularExpression user_reg2 ("(User=) (.*) (Message=)");
            QRegularExpressionMatch user_reg_match2 = user_reg.match(reg_match);
            if(user_reg_match2.hasMatch())
                new_struct.user = user_reg_match2.captured(2);
        }
    }
    QRegularExpression session_level_reg ("(SessionLevel=) (.*) (ServerName=)");    // проверяем, есть ли запись с тэгом "SessionLevel"
    QRegularExpressionMatch session_level_reg_match = session_level_reg.match(reg_match);
    if(session_level_reg_match.hasMatch()){
        new_struct.session_level = session_level_reg_match.captured(2);
    } else {
        QRegularExpression session_level_reg1 ("(SessionLevel=) (.*) (Message=)");    // проверяем, есть ли запись с тэгом "SessionLevel"
        QRegularExpressionMatch session_level_reg_match1 = session_level_reg1.match(reg_match);
        if(session_level_reg_match1.hasMatch())
            new_struct.session_level = session_level_reg_match1.captured(2);
    }

    QRegularExpression server_name_reg ("(ServerName=) (.*) (Message=)");    // проверяем, есть ли запись с тэгом "ServerName"
    QRegularExpressionMatch server_name_reg_match = server_name_reg.match(reg_match);
    if(server_name_reg_match.hasMatch())
        new_struct.server_name = server_name_reg_match.captured(2);

    QRegularExpression reg("(Message=) (.*)");
    QRegularExpressionMatch message_reg_match = reg.match(reg_match);
    new_struct.message = message_reg_match.captured(2);
}

//Функция чтения файла, вызывающая, проверяющая успешность открытия файла и возвращающая вектор структур данных из лог-файла
QVector<date_time_type_msg> ReadData::file_read(){
    QVector<date_time_type_msg> data_vector;
    if(file_open())
        data_vector = read_txt_file();
    return data_vector;
}
