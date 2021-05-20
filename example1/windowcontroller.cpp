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

  /*  ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    ProcessData counters(data_vector);

    Data_Model data_model;//инициализируем модель данных

    // заполняем модель для графика
    data_model.chart_map = counters.get_chart_map();
    */

   // emit SendDataModelToForm(data_model);

    qDebug() << "нажата кнопка <Открыть файл>";
}

void WindowController::CleanFiltersClicked_handler(){
    qDebug() << "нажата кнопка <Очистить>";
}

void WindowController::RestoreDataRange_handler(){
    qDebug() << "нажата кнопка <Сбросить>";
}

