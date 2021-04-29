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

private:
    Ui::MainWindow *ui;

    QChartView *chartView;
    QTableWidget* tableWidget;

    void create_chart();
    void create_table(QHBoxLayout *tree_table_layout);
    void create_tree(QHBoxLayout *tree_table_layout);
    void build_chart(const QMap<QDate, int>& values);
    void build_table(const QVector<date_time_type_msg>& data_vector);
    void clear_chart();
};
#endif // MAINWINDOW_H
