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

    connect(clean_filters_button, SIGNAL (clicked()), this, SLOT (on_pushButton_clean_clicked()));


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
    treeWidget->setMinimumHeight(500);
    treeWidget->setMaximumWidth(180);
    ui->gridLayout->addWidget(treeWidget, 4, 0, -1, 1);
    //treeWidget->header()->hide();

    treeWidget->setHeaderLabel("Фильтры");
    treeWidget->headerItem()->setIcon(0, QIcon(":/filter_icon.png"));
  //  treeWidget->set
    // добавление элементов
    QTreeWidgetItem *itm = new QTreeWidgetItem(treeWidget);
    itm->setText(0, "Уровень сообщения");
    itm->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);

    QTreeWidgetItem *itm1 = new QTreeWidgetItem(treeWidget);
    itm1->setText(0, "Пользователь");
    itm1->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);

    QTreeWidgetItem *itm2 = new QTreeWidgetItem(treeWidget);
    itm2->setText(0, "Уровень доступа");
    itm2->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);
/*
    // сократить -> fill_filters(const QStrinList& types_list);
    QTreeWidgetItem *itm_child_INF = new QTreeWidgetItem(itm);
    itm_child_INF->setText(0, "INF");
    itm_child_INF->setCheckState(0, Qt::Checked);*/
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

// Функция очистки графика и таблицы
void MainWindow::clear_chart_and_table(){
    QLayoutItem *child;
    if((child = ui->gridLayout->takeAt(4)) != nullptr){
        clear_chart();
        tableWidget->clearContents();
        tableWidget->setRowCount(0);
    }
    delete child;
}

// Функция заполнения и построения графика, где даты имеют тип QDateTime
// Принимает массив с данными для графика и промежуток времени в днях
void MainWindow::fill_chart(const QMap<QString, QMap<QString, int>>& types_map, int& range)
{
    if(!types_map.empty()){        // если массив вершин графика непустой - строим график
        clear_chart(); // очищаем старое содержимое графика

        //заполнение значений графика
        QBarSet *setINF = new QBarSet("INF");
        QBarSet *setDBG = new QBarSet("DBG");
        QBarSet *setFTL = new QBarSet("FTL");

        QVector<QString> values;
        for(auto& key : types_map.keys()){
            values.append(key);
            for(auto types_key : types_map.value(key).keys()){
                if(types_key == "INF")
                    *setINF << types_map.value(key).value(types_key);
                if(types_key == "DBG")
                    *setDBG << types_map.value(key).value(types_key);
                if(types_key == "FTL")
                    *setFTL << types_map.value(key).value(types_key);
            }
        }

        setINF->setColor(QColor(153, 204, 255));
        setDBG->setColor(QColor(91,146,208));
        setFTL->setColor(QColor(0,76,153));

        QStackedBarSeries* series = new QStackedBarSeries(this);
        series->append(setINF);
        series->append(setDBG);
        series->append(setFTL);

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

        chart->legend()->setVisible(true);

        // отображаем график
        if(chartView->isHidden())
            chartView->setHidden(false);
        ui->gridLayoutWidget->adjustSize();
        adjustSize();

    } else{    // если массив вершин графика пустой, то очищаем старый график и таблицу
        clear_chart_and_table();
    }
}

//Функция заполнения оси Х значениями типа QDateTime. Также устанавливает заголовок оси и угол наклона надписей
// принимает указатель на ось, ссылку на вектор значений и промежуток времени в днях
void MainWindow::create_axisX(QBarCategoryAxis *axisX, const QVector<QString>& dates_values, int range){
    QStringList categories;
    if(range <= 3){
        axisX->setTitleText("Отрезки времени по 8 часов");
        for(auto temp_date_time : dates_values){
            //QDateTime temp_date_time = QDateTime::fromString(as,time_format);
            //categories << "("+temp_date_time.date().toString("dd.MM")+") "+temp_date_time.toString("hh:mm:ss")+"-"+temp_date_time.addSecs(28799).toString("hh:mm:ss");
            categories << temp_date_time;
        }
    } else if(range > 3 && range <= 31){
        axisX->setTitleText("Даты");
        for(auto temp_date_time : dates_values){
           // categories << temp_date.toString("dd.MM.yy");

            categories << temp_date_time;
        }
        if(categories.size() > 15)
            axisX->setLabelsAngle(-90);
    } else if(range > 31) {
        axisX->setTitleText("Недели");
        auto last_date = dates_values.last();
        for(auto temp_date_time : dates_values){
            if(temp_date_time != last_date){
               // categories << temp_date.toString("dd.MM")   + " - " +  temp_date.addDays(7).toString("dd.MM.yy");

            categories << temp_date_time;
            }else {
               //categories << temp_date.toString("dd.MM")   + " - " +  dates_values.first().addDays(range).toString("dd.MM.yy");
                categories << temp_date_time;
            }
        }
        if(categories.size() > 10)
            axisX->setLabelsAngle(-45);
    }
    axisX->append(categories);
    // менять размер шрифта, если нужно axisX->setLabelsFont(QFont("Times", 8));
}


// Функция построения графика
// принимает объект типа ProcessData, предназначенный для обработки вектора структур с записями из лог-файла
void MainWindow::build_chart(ProcessData& counters){
    int range = 0;
    if(!data_vector.empty())
        range = data_vector[0].date_time.date().daysTo(data_vector.last().date_time.date()); // количество дней в лог-файле
    if(range <= 3){
        QMap<QString, QMap<QString, int>> types_map = counters.make_hours_number_map(data_vector);
        fill_qdatetime_chart(types_map, range); // заполняем график значениями
    } else {
        QMap<QDate, int> values;
        QMap<QDate, QMap<QString, int>> types_map;
        if(range > 3 && range < 31){
            types_map = counters.make_date_number_map(data_vector);
        } else {
            types_map = counters.make_week_number_map(data_vector);
        }
        fill_qdate_chart(types_map, range); // заполняем график значениями
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

    emit OpenFileClicked(filename);

    // в контроллер, контроллер потом передаёт данные  в слот сюда
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

void MainWindow::GetDataAndFillWindow(Data_Model &data_model){
    // первую и последнюю дату можно взять, например, из какого-то map
    //build_chart(data_model.chart_map);
    //build_table(data_model.table_map);
    //fill_filters(data_model.filters_struct);
    /* filters_struct будет содержать qmap с типом и соотв. количеством лог-сообщений данного типа */
}

void MainWindow::on_pushButton_clean_clicked() // кнопка "Очистить"
{
    emit CleanFiltersClicked();
}
void MainWindow::on_pushButton_2_clicked() // кнопка "Сбросить"
{
    if(!data_vector.empty()){
        ui->dateTimeEdit->setDateTime(data_vector[0].date_time);
        ui->dateTimeEdit_2->setDateTime(data_vector.last().date_time);
    }
    // передать сигнал в контроллер
    emit RestoreDataRange();
    //
    // в контроллере взять первую и последнюю дату и сделать новый вектор с данными,
    // передать его обратно, т.е. вызвать сигнал для перерисовки окна , который написан в mainwindow
    // update_Window (data)
}



