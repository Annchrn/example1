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

    void SendDataModelToForm(Data_Model &data_model);
signals:
    // контроллер отправляет сигналы о том, что нужно обоработать модель данных

private:
    MainWindow *window;
    void InitializeConnections();

};

#endif // WINDOWCONTROLLER_H
