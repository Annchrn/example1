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


void WindowController::InitializeConnections(){  // соединяем сигналы и слоты
    // сигналы от mainwindow в контроллер
    connect(window, SIGNAL(OpenFileClicked(const QString&)), this, SLOT(OpenFileChicked_handler(const QString&)));
    connect(window, SIGNAL(CleanFiltersClicked()), this, SLOT(CleanFiltersClicked_handler()));  //кнопка "Очистить"
    connect(window, SIGNAL(RestoreDataRange()), this, SLOT(RestoreDataRange_handler())); // кнопка "Сбросить"

    //сингалы от контроллера в mainwindow
    connect(this, SIGNAL(SendDataModelToForm(Data_Model& )), window, SLOT(GetDataAndFillWindow(Data_Model&)));

}
void WindowController::OpenFileChicked_handler(const QString& filename){

    qDebug() << "нажата кнопка <Открыть файл>";
    ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    if(!data_vector.empty()){
        ProcessData counters(data_vector);
        Data_Model data_model;//инициализируем модель данных
        // заполняем модель для графика
        data_model.time_range = counters.get_time_range();
        data_model.chart_map = counters.get_chart_map();
        data_model.table_map = counters.get_table_map();
        data_model.data_vector = data_vector;
        data_model.filters_struct = counters.get_filters_struct();

        emit SendDataModelToForm(data_model); // отправляем модель данных в mainwindow
    } else{
        // сигнал о том, что нужно очистить таблицу и графики и фильтры
    }
}

void WindowController::CleanFiltersClicked_handler(){
    //тут должны быть нажаты все фильтры, а следовательно, возвращено состояние, как только был открыт файл
    qDebug() << "нажата кнопка <Очистить>";
}

void WindowController::RestoreDataRange_handler(){
    qDebug() << "нажата кнопка <Сбросить>";
}

