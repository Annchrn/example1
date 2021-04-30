#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "data_structure.h"

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
    void build_chart(const QMap<QDate, int>& values);
    void build_table(const QVector<date_time_type_msg>& data_vector);
    void clear_chart();
    void create_axisX(QBarCategoryAxis *axisX, const QMap<QDate, int>& dates_values);

};
#endif // MAINWINDOW_H
