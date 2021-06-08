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
    void Check_all_FiltersClicked_handler(const QDateTime&,const QDateTime&);
    void RestoreDataRange_handler();
    void FiltersChanged_handler(const QStringList& types_filters_list, const QDateTime& begining,const QDateTime& ending); // при изменении фильтров "Уровень сообщения"
    void DateTimeChanged_handler(QDateTime&, QDateTime&);

signals:
    // контроллер отправляет сигналы о том, что нужно обоработать модель данных
    void SendDataModelToForm(Data_Model &data_model);
    void SendInitialDateTimeRangeToForm(QDateTime&, QDateTime&);
    void CleanWindowContents();
    void RebuildChart(const QMap<QDateTime, QMap<QString, int>>&,const int&, const int&);
    void ClearSeries();
    void RebuildWindow(Data_Model& data_model);

private:
    MainWindow *window;
    Data_Model data_model;

    void InitializeConnections();

};

#endif // WINDOWCONTROLLER_H
