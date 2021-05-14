#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "data_structure.h"
#include "processdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private slots:
    void on_pushButton_clicked();
    void on_pushButton_clean_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QPushButton *clean_filters_button;

    QChartView *chartView;
    QTableWidget* tableWidget;

    void update_Window(const QVector<date_time_type_msg>& data_vector);

    void create_chart();
    void create_table();
    void create_tree();

    void build_table();

    void build_chart(ProcessData& counters);
    void fill_qdate_chart(const QMap<QDate, int>& values, int& range);
    void fill_qdatetime_chart(const QMap<QDateTime, int>& values, int& range);
    void create_date_axisX(QBarCategoryAxis *axisX, const QMap<QDate, int>& dates_values, int range);
    void create_date_time_axisX(QBarCategoryAxis *axisX, const QMap<QDateTime, int>& dates_values, int range);


    void clear_chart();
    void clear_chart_and_table();

    QVector<date_time_type_msg> data_vector; // вектор структур, содержащий все лог-записи из файла


};
#endif // MAINWINDOW_H
