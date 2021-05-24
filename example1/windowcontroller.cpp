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
    connect(window, SIGNAL(ApplyTypeFilters(const QStringList&)), this, SLOT(ApplyTypeFilters_handler(const QStringList&)));

    //сингалы от контроллера в mainwindow
    connect(this, SIGNAL(SendDataModelToForm(Data_Model& )), window, SLOT(GetDataAndFillWindow(Data_Model&)));
    connect(this, SIGNAL(SendInitialDateTimeRangeToForm(QDateTime&, QDateTime&)), window, SLOT(RestoreDateTimeRange(QDateTime&, QDateTime&)));
    connect(this, SIGNAL(CleanWindowContents()), window, SLOT(clear_window_contents()));

}
void WindowController::OpenFileChicked_handler(const QString& filename){
    ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    if(!data_vector.empty()){
        ProcessData counters(data_vector);
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

void WindowController::RestoreDataRange_handler(){
    emit SendInitialDateTimeRangeToForm(data_model.data_vector[0].date_time, data_model.data_vector.last().date_time);
}

void WindowController::ApplyTypeFilters_handler(const QStringList& types_filters_list){
    // объявляем модель данных
}
