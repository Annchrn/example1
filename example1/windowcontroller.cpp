#include "windowcontroller.h"
#include "readdata.h"

#include <QTextStream>
#include <QDateTime>
#include <QMap>
#include <QString>


WindowController::WindowController(MainWindow *w)
{
    window = w;
    InitializeConnections();
}

// соединяем сигналы и слоты
void WindowController::InitializeConnections(){
    // сигналы от mainwindow в контроллер
    connect(window, SIGNAL(OpenFileClicked(const QString&)), this, SLOT(OpenFileChicked_handler(const QString&)));
    connect(window, SIGNAL(Check_all_FiltersClicked(const QDateTime&,const QDateTime&)), this, SLOT(Check_all_FiltersClicked_handler(const QDateTime&,const QDateTime&)));  //кнопка "Очистить"
    connect(window, SIGNAL(RestoreDataRange()), this, SLOT(RestoreDataRange_handler())); // кнопка "Сбросить"
    connect(window, SIGNAL(FiltersChanged(const QStringList&, const QDateTime&,const QDateTime&)), this, SLOT(FiltersChanged_handler(const QStringList&, const QDateTime&,const QDateTime&)));
    connect(window, SIGNAL(DateTimeChanged(QDateTime&, QDateTime&)), this, SLOT(DateTimeChanged_handler(QDateTime&, QDateTime&)));

    //сингалы от контроллера в mainwindow
    connect(this, SIGNAL(SendDataModelToForm(Data_Model&)), window, SLOT(GetDataAndFillWindow(Data_Model&)));
    connect(this, SIGNAL(SendInitialDateTimeRangeToForm(QDateTime&, QDateTime&)), window, SLOT(RestoreDateTimeRange(QDateTime&, QDateTime&)));
    connect(this, SIGNAL(CleanWindowContents()), window, SLOT(clear_window_contents()));
    connect(this, SIGNAL (RebuildChart(const QMap<QDateTime, QMap<QString, int>>&,const int&, const int&)), window, SLOT(RebuildChart_handler(const QMap<QDateTime, QMap<QString, int>>&,const int&,const int&)));
    connect(this, SIGNAL(ClearSeries()), window, SLOT(clear_series()));
    connect(this, SIGNAL(RebuildWindow(Data_Model&)), window, SLOT(GetDataAndRebuildWindow(Data_Model&)));
}

// обработка сигнала открытия файла
void WindowController::OpenFileChicked_handler(const QString& filename){
    ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    if(!data_vector.empty()){
        ProcessData counters(data_vector, data_vector.first().date_time, data_vector.last().date_time);
        // заполняем модель данных
        data_model.time_range = counters.get_time_range();
        data_model.chart_map = counters.get_chart_map();
        data_model.data_vector = data_vector;
        data_model.filters_struct = counters.get_filters_struct();
        // отправляем модель данных в mainwindow
        emit SendDataModelToForm(data_model);
    } else{
        // сигнал о том, что нужно очистить таблицу и графики и фильтры
        emit CleanWindowContents();
    }
}

// обработка сигнала нажатия кнопки "Очистить"
void WindowController::Check_all_FiltersClicked_handler(const QDateTime& begining, const QDateTime& ending){
    qDebug() << "нажата кнопка <Очистить>";
    ProcessData counters(data_model.data_vector, begining, ending);
    emit RebuildChart(counters.get_chart_map(), counters.get_time_range(), data_model.data_vector.size());
}

// обработка сигнала нажатия кнопки "Сбросить"
void WindowController::RestoreDataRange_handler(){
    if(!data_model.data_vector.empty()){
        emit SendInitialDateTimeRangeToForm(data_model.data_vector[0].date_time, data_model.data_vector.last().date_time);
        emit RebuildWindow(data_model);
    }
}

// обработка сигнала изменения фильтров
void WindowController::FiltersChanged_handler(const QStringList& types_filters_list, const QDateTime& begining, const QDateTime& ending){
    QVector<date_time_type_msg> current_data_vector;
    for(const auto& structure : data_model.data_vector){
        if(types_filters_list.contains(structure.type)){
            current_data_vector.append(structure);
        } else if(structure.user != "" && types_filters_list.contains(structure.user)){
            current_data_vector.append(structure);
        } else if(structure.session_level != "" && types_filters_list.contains(structure.session_level)){
            current_data_vector.append(structure);
        } else if(structure.server_name != "" && types_filters_list.contains(structure.server_name)){
            current_data_vector.append(structure);
        }
    }
    if(!current_data_vector.empty()){
        ProcessData current_counters(current_data_vector, begining, ending);
        emit RebuildChart(current_counters.get_chart_map(), begining.secsTo(ending), current_data_vector.size());
    } else{
        emit ClearSeries();
    }
}

// обработка сигнала изменения диапазона даты/времени
void WindowController::DateTimeChanged_handler(QDateTime& start, QDateTime& finish){
    try {
        if(!data_model.data_vector.empty()){
        if (start < finish){
               QVector<date_time_type_msg> current_data_vector;
               for (const auto& structure : data_model.data_vector){
                   if (structure.date_time >= start && structure.date_time <= finish)
                       current_data_vector.append(structure);
               }
               if(!current_data_vector.empty()){
                    ProcessData current_counters(current_data_vector, start, finish);
                    Data_Model current_data_model;
                    // заполняем модель данных
                    current_data_model.time_range = current_counters.get_time_range();
                    current_data_model.data_vector = current_data_vector;
                    current_data_model.filters_struct = current_counters.get_filters_struct();
                    current_data_model.chart_map = current_counters.get_chart_map();

                    // отправляем модель данных в mainwindow
                    emit RebuildWindow(current_data_model);
               } else {
                   emit CleanWindowContents();
                   throw std::runtime_error("Недопустимый диапазон");
               }
          } else {
                emit CleanWindowContents();
                throw std::runtime_error("В данном диапазоне нет сообщений");
          }
        } else {
            throw std::runtime_error("Нет данных для отображения");
            emit CleanWindowContents();
        }
    }  catch (std::exception& ex) {
        QMessageBox::critical(0, "Ошибка", ex.what());
    }
}
