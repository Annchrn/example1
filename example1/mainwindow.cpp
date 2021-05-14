#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readdata.h"

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
    // добавление кнопки "Очистить"
    clean_filters_button = new QPushButton("Очистить", this);
    connect(clean_filters_button, SIGNAL (clicked()), this, SLOT (on_pushButton_clean_clicked()));
    ui->gridLayout->addWidget(clean_filters_button, 3, 7);
    clean_filters_button->setMaximumWidth(80);

    // Добавление метки "Применённые фильтры"
    QLabel *label = new QLabel(this);
    ui->gridLayout->addWidget(label, 3, 0);
    label->setText("Применённые фильтры:");

    // фильтры
    create_tree();
    //таблица
    create_table();

    // сигналы
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
    chart->setTitle("Зависимость количества записей журнала от временного диапазона");
    chart->legend()->hide();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(1000,250);
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

     ui->gridLayout->addWidget(tableWidget, 4, 1, -1, -1);
}

// Инициализаиция QTreeWidget
//
void MainWindow::create_tree()
{
    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabel("Фильтры");
    treeWidget->setMinimumSize(200,500);
    treeWidget->setMaximumWidth(200);
    ui->gridLayout->addWidget(treeWidget, 4, 0, -1, 1);
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

//Функция добавления значений на ось х. Также устанавливает заголовок оси и угол наклона надписей
// принимает указатель на ось и данные для графика
template <typename T>
void MainWindow::create_axisX(QBarCategoryAxis *axisX, const QMap<T, int>& dates_values, int range){
    QStringList categories;
    if(range <= 3){
        axisX->setTitleText("Отрезки времени по 8 часов");
        qDebug() << "для случая, когда 3 дня и меньше пока нет функции по 8 часов";
    } else if(range > 3 && range <= 31){
        axisX->setTitleText("Даты");
        for(auto temp_date : dates_values.keys()){
           categories << temp_date.toString("dd.MM.yy");
        }
        if(categories.size() > 15)
            axisX->setLabelsAngle(-90);
    } else if(range > 31) {
        axisX->setTitleText("Недели");
        auto last_date = dates_values.keys().last();
        for(auto temp_date : dates_values.keys()){
            if(temp_date != last_date){
                categories << temp_date.toString("dd.MM")   + " - " +  temp_date.addDays(7).toString("dd.MM.yy");
            }else {
                categories << temp_date.toString("dd.MM")   + " - " +  dates_values.keys().first().addDays(range).toString("dd.MM.yy");
            }
        }
        if(categories.size() > 10) // не количество днец а количество недель, которое выводится
            axisX->setLabelsAngle(-45);
    }
    axisX->append(categories);
  }

//Функция заполнения графика значениями
//values - массив вершин графика, где значения для оси х имеют тип QDate
template <typename T>
void MainWindow::fill_chart(const QMap<T, int>& values, int& range)
{
    if(!values.empty()){        // если массив вершин графика непустой - строим график
        clear_chart(); // очищаем старое содержимое графика

        //заполнение значений графика
        QBarSeries* series = new QBarSeries(this);
        QBarSet *set = new QBarSet("Количество сообщений");
        QVector<T> dates;
        for(const auto& key : values.keys()){
            dates << key;
            *set << values.value(key);
        }
        series->append(set);
        QChart *chart = chartView->chart();
        chart->addSeries(series);

        // ось х
        QBarCategoryAxis *axisX = new QBarCategoryAxis(this);
        create_axisX(axisX, values, range);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

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

//Функция построения графика
//принимает объект типа ProcessData, предназначенный для обработки вектора структур с записями из лог-файла
void MainWindow::build_chart(ProcessData& counters){
    int range = 0;
    if(!data_vector.empty())
        range = data_vector[0].date_time.date().daysTo(data_vector.last().date_time.date()); // количество дней в лог-файле
    if(range < 3){
        QMap<QDateTime, int> values;
        values = counters.make_hours_number_map(data_vector);
        fill_chart(values, range); // заполняем график значениями
    } else {
        QMap<QDate, int> values;
        if(range > 3 && range < 31){
            values = counters.make_date_number_map(data_vector);
        } else {
            values = counters.make_week_number_map(data_vector);
        }
        fill_chart(values, range); // заполняем график значениями
    }
}

// Функция заполнения таблицы
void MainWindow::build_table(){
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

void MainWindow::update_Window(const QVector<date_time_type_msg>& data_vector)
{
    // принимает данные и каждый раз перестраивает таблицу и график по новому вектору данных
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Документ (разделитель - ;) (*.txt);; JSON (*.json)");
    QString label_file_name = filename.split("/").takeLast();
    ui->label->setText("Файл " + label_file_name);

    ReadData data_read(filename);
    data_vector = data_read.file_read();
    ProcessData counters(data_vector);

    build_chart(counters);
    build_table();

    // отображение данных о лог-файле
    ui->label_3->setText("Всего записей: " + QString::number(counters.get_logs_count()));
    if(!data_vector.empty()){
        QDateTime start = data_vector[0].date_time;
        QDateTime finish = data_vector.last().date_time;
        ui->dateTimeEdit->setDateTime(start);
        ui->dateTimeEdit_2->setDateTime(finish);
    }
}

void MainWindow::on_pushButton_clean_clicked() // кнопка "Очистить"
{

}
void MainWindow::on_pushButton_2_clicked() // кнопка "Сбросить"
{

}
