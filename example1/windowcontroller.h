#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QMainWindow>
#include "mainwindow.h"

class WindowController : public QObject
{
    Q_OBJECT

public:
    WindowController(MainWindow *w);

private slots:
    // контроллер обрабатывает данные из лог-файла, принимает ссылку на считанный вектор структур
    void OpenFileChicked_handler(const QString& filename);
    void CleanFiltersClicked_handler();
    void RestoreDataRange_handler();
    void ApplyTypeFilters_handler(const QStringList& types_filters_list);

signals:
    // контроллер отправляет сигналы о том, что нужно обоработать модель данных
    void SendDataModelToForm(Data_Model &data_model);
    void SendInitialDateTimeRangeToForm(QDateTime&, QDateTime&);
    void CleanWindowContents();

private:
    MainWindow *window;
    Data_Model data_model;

    void InitializeConnections();

};

#endif // WINDOWCONTROLLER_H
