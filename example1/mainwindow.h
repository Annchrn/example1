#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "data_structure.h"
#include "processdata.h"
#include "Data_Model.h"

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
    void on_pushButton_clicked(); // кнопка "Открыть файл"
    void on_pushButton_clean_clicked(); // кнопка "Очистить"
    void on_pushButton_2_clicked(); // кнопка "Сбросить"

    void GetDataAndFillWindow(Data_Model &data_model);
    void RestoreDateTimeRange(QDateTime& start, QDateTime& finish);
    void ChangeDateTimeRange();
    void ChangeFilters(QTreeWidgetItem*, int);
    void clear_window_contents();

signals:
    void OpenFileClicked(const QString& filename);
    void CleanFiltersClicked();
    void RestoreDataRange();
    void ApplyTypeFilters(const QStringList& types_list);

private:
    Ui::MainWindow *ui;

    QPushButton *clean_filters_button;

    QChartView *chartView;
    QTableWidget* tableWidget;
    QTreeWidget *treeWidget;

    //============
    QMap<QString, QBarSet*> set_map;
    QStackedBarSeries* series;
    //===================
    // инициализация таблицы, графика и дерева
    void create_chart();
    void create_table();
    void create_tree();

    // функции для работы с таблицей, графиком и деревом
    void fill_table(const QVector<date_time_type_msg>& data_vector);
    void fill_filters(const Filters_structure& filters_struct);
    void fill_chart(const QMap<QDateTime, QMap<QString, int>>& types_map, int& range);
    void create_axisX(QBarCategoryAxis *axisX, const QVector<QDateTime>& dates_values, const int& range);
    void clear_chart();
    void FilterTable(const QStringList& types_filters_list);
    void FilterChart(const QStringList& types_filters_list);

    QVector<date_time_type_msg> data_vector; // вектор структур, содержащий все лог-записи из файла


};
#endif // MAINWINDOW_H
