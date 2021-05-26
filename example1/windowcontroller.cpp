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
    connect(window, SIGNAL(CleanFiltersClicked()), this, SLOT(CleanFiltersClicked_handler()));  //кнопка "Очистить"
    connect(window, SIGNAL(RestoreDataRange()), this, SLOT(RestoreDataRange_handler())); // кнопка "Сбросить"
    connect(window, SIGNAL(TypeFiltersChanged(const QStringList&)), this, SLOT(TypeFiltersChanged_handler(const QStringList&)));
    connect(window, SIGNAL(DateTimeChanged(QDateTime&, QDateTime&)), this, SLOT(DateTimeChanged_handler(QDateTime&, QDateTime&)));

    //сингалы от контроллера в mainwindow
    connect(this, SIGNAL(SendDataModelToForm(Data_Model&)), window, SLOT(GetDataAndFillWindow(Data_Model&)));
    connect(this, SIGNAL(SendInitialDateTimeRangeToForm(QDateTime&, QDateTime&)), window, SLOT(RestoreDateTimeRange(QDateTime&, QDateTime&)));
    connect(this, SIGNAL(CleanWindowContents()), window, SLOT(clear_window_contents()));
    connect(this, SIGNAL (RebuildChart(const QMap<QDateTime, QMap<QString, int>>&,const int&, const int&)), window, SLOT(RebuildChart_handler(const QMap<QDateTime, QMap<QString, int>>&,const int&,const int&)));
    connect(this, SIGNAL(ClearSeries()), window, SLOT(clear_chart()));
    connect(this, SIGNAL(RebuildWindow(Data_Model&)), window, SLOT(GetDataAndRebuildWindow(Data_Model&)));
}
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

void WindowController::CleanFiltersClicked_handler(){
    //тут должны быть нажаты все фильтры, а следовательно, возвращено состояние, как только был открыт файл
    qDebug() << "нажата кнопка <Очистить>";
    // тут сбрасываются только фильтры по типам, но не
}
// для восстановления диапазона
void WindowController::RestoreDataRange_handler(){
    if(!data_model.data_vector.empty())
        emit SendInitialDateTimeRangeToForm(data_model.data_vector[0].date_time, data_model.data_vector.last().date_time);
}
// для перестроения графика
void WindowController::TypeFiltersChanged_handler(const QStringList& types_filters_list){
    QVector<date_time_type_msg> current_data_vector;
    for(const auto& structure : data_model.data_vector){
        if(types_filters_list.contains(structure.type))
            current_data_vector.append(structure);
    }
    if(!current_data_vector.empty()){
        ProcessData current_counters(current_data_vector, current_data_vector.first().date_time, current_data_vector.last().date_time);
        emit RebuildChart(current_counters.get_chart_map(), current_counters.get_time_range(), current_data_vector.size());
    } else{
        emit ClearSeries();
    }
}
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
                    emit RebuildWindow(data_model);
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

