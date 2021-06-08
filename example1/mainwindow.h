#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "data_structure.h"
#include "processdata.h"
#include "Data_Model.h"
#include "datetimeedit.h"

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
<<<<<<< HEAD
    void on_pushButton_check_all_clicked(); // кнопка "Отметить всё"
 //   void on_pushButton_clean_all_clicked(); // кнопка "Очистить всё"
=======
    void on_pushButton_clean_clicked(); // кнопка "Очистить"
>>>>>>> 637613c7b49ca842e3bc63e1611c9443b29252cf
    void on_pushButton_2_clicked(); // кнопка "Сбросить"
    void ChangeTypeFilters(QTreeWidgetItem*, int);
    void ChangeDateTimeRange();

    void clear_chart();
    void clear_series();
    void expand_and_collapse_treeWidget();

    // вызываются в контроллере
    void GetDataAndFillWindow(Data_Model &data_model); //  принимает модель данных и заполняет график, таблицу и фильтры
    void RestoreDateTimeRange(QDateTime& start, QDateTime& finish); // сбрасывает временной диапазон
    void clear_window_contents(); // очищает таблицу, фильтры и график, если происходит ошибка чтения данных
    void RebuildChart_handler(const QMap<QDateTime, QMap<QString, int>>&,const int&,const int&);
    void GetDataAndRebuildWindow(Data_Model &data_model);

signals:
    void OpenFileClicked(const QString& filename);
<<<<<<< HEAD
    void Check_all_FiltersClicked(const QDateTime&,const QDateTime&);
=======
    void CleanFiltersClicked(const QDateTime&,const QDateTime&);
>>>>>>> 637613c7b49ca842e3bc63e1611c9443b29252cf
    void RestoreDataRange();
    void FiltersChanged(const QStringList&, const QDateTime&,const QDateTime&);
    void DateTimeChanged(QDateTime&, QDateTime&);

private:
    Ui::MainWindow *ui;

<<<<<<< HEAD
    QPushButton *check_all_filters_button;
    QPushButton *uncheck_all_filters_button;
=======
    QPushButton *clean_filters_button;
>>>>>>> 637613c7b49ca842e3bc63e1611c9443b29252cf
    QPushButton *restore_date_time_range_button;
    QPushButton *expand_button;

    QChartView *chartView;
    QTableWidget* tableWidget;
    QTreeWidget *treeWidget;

    DateTimeEdit *dateTimeEdit;
    DateTimeEdit *dateTimeEdit_2;
    QLabel *messages_counter;

    // инициализация таблицы, графика и дерева
    void create_chart();
    void create_table();
    void create_tree();
    void create_date_time_edit_elements();

    // функции для работы с таблицей, графиком и деревом
    void fill_chart(const QMap<QDateTime, QMap<QString, int>>& types_map,const int& range);
    void create_axisX(QBarCategoryAxis *axisX, const QVector<QDateTime>& dates_values,const int& range);

    void fill_table(const QVector<date_time_type_msg>& data_vector);

    void fill_filters(const Filters_structure& filters_struct);
    void fill_top_level_items(const QMap<QString, int>& map, const int& top_level_item_index);
    void add_top_level_item(const QString& name);

    void TypeFilterTable(const QStringList& types_filters_list);

    QVector<date_time_type_msg> data_vector; // вектор структур, содержащий все лог-записи из файла
};
#endif // MAINWINDOW_H
