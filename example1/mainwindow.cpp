#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readdata.h"
#include "processdata.h"

#include <QTextStream>
#include <QDateTime>
#include <QMap>
#include <QFileDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidget>
#include <QString>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->gridLayoutWidget);
    // внешний вид окна
    ui->gridLayout->setMargin(15);
    ui->pushButton->setMaximumWidth(150);
    ui->label->setMaximumWidth(200);

    adjustSize();
    // график
    create_chart();
    // фильтры
    create_tree();
    //таблица
    create_table();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Инициализация графика
void MainWindow::create_chart()
{
    QChart *chart = new QChart();
    // отображение
    chartView = new QChartView(chart);
    ui->gridLayout->addWidget(chartView, 2, 0, 1, -1);
    chart->setTitle("Зависимость количества записей журнала от даты");
    chart->legend()->hide();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(1000,200);
}

// Инициализация таблицы
void MainWindow::create_table(){
    //таблица
    tableWidget = new QTableWidget(this);
    tableWidget->setMinimumSize(800,500);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // столбцы
    tableWidget->setColumnCount(2);
    QStringList name_table;
    name_table << "Дата" << "Сообщение";
    tableWidget->setHorizontalHeaderLabels(name_table);
    tableWidget->setColumnWidth(0, 200);
    tableWidget->horizontalHeader()->setStretchLastSection(true);

    tableWidget->setFont(QFont("Times", 9));
    tableWidget->horizontalHeader()->setFont(QFont("Times", 9));//, QFont::Bold));
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}" );

     ui->gridLayout->addWidget(tableWidget, 3, 1, -1, -1);
}

// Инициализаиция QTreeWidget
//
void MainWindow::create_tree()
{
    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabel("Фильтры");
    treeWidget->setMinimumSize(200,500);
    treeWidget->setMaximumWidth(200);
    ui->gridLayout->addWidget(treeWidget, 3, 0, -1, 1);
}

// Функция очистки графика
// удаляет series и axixX, axisY из графика
void MainWindow::clear_chart()
{
    chartView->chart()->removeAllSeries();
    // удаляем оси, если они уже были установлены
    auto *daxisX = chartView->chart()->axisX();
    auto *daxisY = chartView->chart()->axisY();
    if(daxisX && daxisY){
        chartView->chart()->removeAxis(daxisX);
        delete daxisX;
        chartView->chart()->removeAxis(daxisY);
        delete daxisY;
    }
}
/*
//Функция построения графика (зависимость даты от количества лог-записей)
//values - массив вершин графика
void MainWindow::build_chart(const QMap<QDate, int>& values){
    if(!values.empty()){        // если массив вершин графика непустой - строим график
        clear_chart(); // очищаем старое содержимое графика

        //заполнение значений графика
        QLineSeries* series = new QLineSeries(this);
        for(const auto& key : values.keys()){
            QDateTime k(key);
            series->append(k.toMSecsSinceEpoch(), values.value(key));
        }
        QChart *chart = chartView->chart();
        chart->addSeries(series);

        // оси х, у
        QDateTimeAxis *axisX = new QDateTimeAxis(this);
        axisX->setTickCount(12);
        axisX->setFormat("dd.MM.yyyy");
        axisX->setTitleText("Дата");
        chart->addAxis(axisX, Qt::AlignBottom);

        QValueAxis *axisY = new QValueAxis(this);
        axisY->setLabelFormat("%i");
        axisY->setTitleText("Значения");
        chart->addAxis(axisY, Qt::AlignLeft);

        series->attachAxis(axisX);
        series->attachAxis(axisY);

        // отображаем график
        if(chartView->isHidden())
            chartView->setHidden(false);
        ui->gridLayoutWidget->adjustSize();
        adjustSize();

} else{    // если массив вершин графика пустой, то очищаем старый график и таблицу
        QLayoutItem *child;
        if((child = ui->gridLayout->takeAt(4)) != nullptr){
            clear_chart();
            tableWidget->clearContents();
            tableWidget->setRowCount(0);
        }
        delete child;
    }
}
*/

//Функция добавления значений на ось х
// принимает указатель на ось и данные для графика
void MainWindow::create_axisX(QBarCategoryAxis *axisX, const QMap<QDate, int>& dates_values){
    int dates_count = dates_values.count();
    if(dates_count < 12){
    // работаем с вектором значений, добавляем до 12

    } else if(dates_count > 12 && dates_count < 40){
        axisX->setLabelsAngle(-90);
    } else if(dates_count > 40) {

    }
    QStringList categories;
    for(auto temp_date : dates_values.keys()){
       categories << temp_date.toString("dd.MM.yy");
    }
    axisX->append(categories);
  }

//Функция построения графика (зависимость даты от количества лог-записей)
//values - массив вершин графика
void MainWindow::build_chart(const QMap<QDate, int>& values){
    if(!values.empty()){        // если массив вершин графика непустой - строим график
        clear_chart(); // очищаем старое содержимое графика

        //заполнение значений графика
        QBarSeries* series = new QBarSeries(this);
        QBarSet *set = new QBarSet("Количество сообщений"); // для добавления значений
      //  QStringList dates_values;
        QVector<QDate> dates;
        for(const auto& key : values.keys()){
           // series->append(k.toMSecsSinceEpoch(), values.value(key));
            //dates_values << key.toString();
            dates << key;
            *set << values.value(key);
        }
        series->append(set);
        QChart *chart = chartView->chart();
        chart->addSeries(series);

        // ось х
       QBarCategoryAxis *axisX = new QBarCategoryAxis(this);
        create_axisX(axisX, values);

        axisX->setTitleText("Дата");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        // оси х, у
    //    QDateTimeAxis *axisX = new QDateTimeAxis(this);
      // axisX->setR
      //  axisX->setTickCount(5);
       // axisX->setFormat("dd.MM.yyyy");
      //  axisX->setTitleText("Дата");
       // chart->addAxis(axisX, Qt::AlignBottom);
      // series->attachAxis(axisX);

        // ось у
        QValueAxis *axisY = new QValueAxis(this);
        axisY->setLabelFormat("%i");
        axisY->setTitleText("Значения");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        // отображаем график
        if(chartView->isHidden())
            chartView->setHidden(false);

        ui->gridLayoutWidget->adjustSize();
        adjustSize();

} else{    // если массив вершин графика пустой, то очищаем старый график и таблицу
        QLayoutItem *child;
        if((child = ui->gridLayout->takeAt(4)) != nullptr){
            clear_chart();
            tableWidget->clearContents();
            tableWidget->setRowCount(0);
        }
        delete child;
    }
}

// Функция заполнения таблицы
// принимает вектор структур записей из лог-файла
void MainWindow::build_table(const QVector<date_time_type_msg>& data_vector){
    if(!data_vector.isEmpty()){     // если вектор структур непустой, заполняем таблицу
        tableWidget->clearContents(); //удаляем старое содержимое из таблицы

        // добавляем в таблицу данные из вектора структур
        tableWidget->setRowCount(data_vector.size());
        for(int i = 0; i < data_vector.size(); i++){
            // 1-й столбец - "Дата"
            QTableWidgetItem* item0 = new QTableWidgetItem();
            item0->setTextAlignment(84);
            item0->setText(data_vector[i].date_time.toString("ddd dd.MM.yyyy hh:mm:ss"));
            tableWidget->setItem(i, 0, item0);
            // 2-й столбец - "Сообщение"
            QTableWidgetItem* item1 = new QTableWidgetItem();
            item1->setTextAlignment(84);
            item1->setText(data_vector[i].message);
            tableWidget->setItem(i, 1, item1);

            if(data_vector[i].type == "FTL"){
                tableWidget->item(i, 0)->setForeground(Qt::red);
                tableWidget->item(i, 1)->setForeground(Qt::red);
            }
            tableWidget->setHidden(false); // отображаем таблицу
        }
    }
}

//void MainWindow::update_Window(){

//}



void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Документ (разделитель - ;) (*.txt);; JSON (*.json)");
    QString label_file_name = filename.split("/").takeLast();
    ui->label->setText("Файл " + label_file_name);

    ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    ProcessData counters(data_vector);
    QMap<QDate, int> values = counters.make_date_number_map(data_vector);
    build_chart(values);
    build_table(data_vector);

    // отображение данных о лог-файле
    ui->label_3->setText("Считано записей: " + QString::number(counters.get_logs_count()));
}
