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
    label->setText("Применённые:");

    // фильтры
    create_tree();
    //таблица
    create_table();

    // сигналы
    connect(clean_filters_button, SIGNAL (clicked()), this, SLOT (on_pushButton_clean_clicked()));
    connect(ui->dateTimeEdit, SIGNAL(editingFinished()), this, SLOT(ChangeDateTimeRange()));
    connect(ui->dateTimeEdit_2, SIGNAL(editingFinished()), this, SLOT(ChangeDateTimeRange()));

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
    tableWidget->setColumnCount(3);
    tableWidget->setColumnHidden(2, true);

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
void MainWindow::create_tree()
{
    treeWidget = new QTreeWidget(this);
    treeWidget->setMinimumHeight(500);
    //treeWidget->setMaximumWidth(180);
    ui->gridLayout->addWidget(treeWidget, 4, 0, -1, 1);
  //  treeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    //treeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    treeWidget->setHeaderLabel("Фильтры");
    treeWidget->headerItem()->setIcon(0, QIcon(":/filter_icon.png"));

    // добавление элементов
/*    QTreeWidgetItem *itm = new QTreeWidgetItem(treeWidget);
    itm->setText(0, "Уровень сообщения");
    itm->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);

    QTreeWidgetItem *itm1 = new QTreeWidgetItem(treeWidget);
    itm1->setText(0, "Пользователь");
    itm1->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);

    QTreeWidgetItem *itm2 = new QTreeWidgetItem(treeWidget);
    itm2->setText(0, "Уровень доступа");
    itm2->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);
*/
}
//================================== работа с таблицей, графиками и фильтрами ==================================================================================================================
// Функция очистки содержимого окна
void MainWindow::clear_window_contents(){
    QLayoutItem *child;
    if((child = ui->gridLayout->takeAt(4)) != nullptr){
        clear_chart();
        tableWidget->clearContents();
        tableWidget->setRowCount(0);
        treeWidget->clear();
    }
    ui->label_3->setText("Всего записей: ");
    delete child;
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

// Функция заполнения и построения графика, где даты имеют тип QDateTime
// Принимает массив с данными для графика и промежуток времени в днях
void MainWindow::fill_chart(const QMap<QDateTime, QMap<QString, int>>& types_map, int& range)
{        clear_chart(); // очищаем старое содержимое графика
         set_map.clear();
        //инициализируем массив для QBarSet, где создаём указатели на BarSet для каждого типа сообщения в лог-файле
        QMap<QString, int> temp_map = types_map.value(types_map.firstKey());
        for(const auto& key : temp_map.keys()){
            QBarSet *set = new QBarSet(key);
            set_map[key] = (set);
        }
        // заполняем вектор с датами для оси x и данные для гистограммы
        QVector<QDateTime> values;
        for(const auto& date_time : types_map.keys()){ // проходим модели данных для графика
            values.append(date_time); // записываем дату-время для оси х
            for(const auto& type : types_map.value(date_time).keys()){ // проходим по массиву с типами и их количеством для каждого промежутка времени
                    *set_map[type] << types_map.value(date_time).value(type);
            }
        }
        //  добавляем "кусочки" гистограммы на график
        QStackedBarSeries* series = new QStackedBarSeries(this);
        for(const auto& type : set_map.keys()){
            series->append(set_map.value(type));
        }

        /*
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

        // посчитать количество логов и сделать число, по которому будет меняться цвет
        // если логов 10, то r-15 g -20 b-20
        // это число = 150 / количество логов
        int R=150, G=200, B=255;
       *  int k = int(150 / set_vector.size());
        for(auto& set : set_vector){
            set.setColor(QColor(R, G, B));
            if(R>=0)
                R-=15;
            if(G>=0)
                G+=;
            if(B>=0)
                B+=30;
        }
        // либо не устанавливать цвета вообще, чтобы были автоматические

        setINF->setColor(QColor(150, 200, 255));
        setDBG->setColor(QColor(91,146,208));
        setFTL->setColor(QColor(0,76,153));
*/
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
        chart->legend()->setVisible(true);
        if(chartView->isHidden())
            chartView->setHidden(false);
        ui->gridLayoutWidget->adjustSize();
        adjustSize();
}

//Функция заполнения оси Х значениями типа QDateTime. Также устанавливает заголовок оси и угол наклона надписей
// принимает указатель на ось, ссылку на вектор значений и промежуток времени в днях
void MainWindow::create_axisX(QBarCategoryAxis *axisX, const QVector<QDateTime>& dates_values, const int& range){
    QStringList categories;
    if(range <= 259200){
        axisX->setTitleText("Отрезки времени по 8 часов");
        for(auto temp_date_time : dates_values){
            if(temp_date_time != dates_values.last()){
                categories << "("+temp_date_time.date().toString("dd.MM")+") "+temp_date_time.toString("hh:mm:ss")+"-" +temp_date_time.addSecs(28799).toString("hh:mm:ss");
            } else{
                categories << "("+temp_date_time.date().toString("dd.MM")+") "+temp_date_time.toString("hh:mm:ss")+"-" +dates_values.first().addSecs(range).toString("hh:mm:ss");
            }
        }
    } else if(range > 259200 && range <= 2678400){
        axisX->setTitleText("Даты");
        for(auto temp_date_time : dates_values)
           categories << temp_date_time.toString("dd.MM.yy");
        if(categories.size() > 15)
            axisX->setLabelsAngle(-90);
    } else if(range > 2678400) {
        axisX->setTitleText("Недели");
        for(auto temp_date_time : dates_values){
            if(temp_date_time != dates_values.last()){
                categories << temp_date_time.toString("dd.MM")   + " - " +  temp_date_time.addSecs(518400).toString("dd.MM.yy");
            }else {
               categories << temp_date_time.toString("dd.MM")   + " - " +  dates_values.first().addSecs(range).toString("dd.MM.yy");
            }
        }
        if(categories.size() > 10)
            axisX->setLabelsAngle(-45);
    }
    axisX->append(categories);
    // менять размер шрифта, если нужно axisX->setLabelsFont(QFont("Times", 8));
}

// Функция заполнения таблицы
void MainWindow::fill_table(const QVector<date_time_type_msg>& data_vector){
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

            QTableWidgetItem* item3 = new QTableWidgetItem();
            item3->setText(data_vector[i].type);
            tableWidget->setItem(i, 2, item3);

            if(data_vector[i].type == "FTL" || data_vector[i].type == "FATAL"){
                tableWidget->item(i, 0)->setForeground(Qt::red);
                tableWidget->item(i, 1)->setForeground(Qt::red);
            }
            tableWidget->setHidden(false); // отображаем таблицу
        }
    }

// Функция заполнения панели фильтров
void MainWindow::fill_filters(const Filters_structure& filters_struct){
    disconnect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem* , int)), this, SLOT(ChangeFilters()));
   /* int type_item_index = treeWidget->itemAt(0,1)->childCount();
    while(type_item_index != -1){
        treeWidget->itemAt(0,1)->removeChild(treeWidget->itemAt(0,1)->takeChild(type_item_index));
        type_item_index--;
    }
    for(const auto& type : filters_struct.types_map.keys()){
        QTreeWidgetItem *type_item = new QTreeWidgetItem(treeWidget->itemAt(0,1));
        type_item->setText(0, type + " (" + QString::number(filters_struct.types_map.value(type)) + ")");
        type_item->setCheckState(0, Qt::Unchecked);
    }
    treeWidget->itemAt(0,1)->setExpanded(true);
*/
    treeWidget->clear();
    for(const auto& type : filters_struct.types_map.keys()){
        QTreeWidgetItem *type_item = new QTreeWidgetItem(treeWidget);
        type_item->setText(0, type + " (" + QString::number(filters_struct.types_map.value(type)) + ")");
        type_item->setCheckState(0, Qt::Unchecked);
    }
    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(ChangeFilters()));
}
// ==================== функции для фильтрации ====================

void MainWindow::ChangeFilters(){  // при изменении фильтров
    QStringList types_filters_list;
    for(int i = 0; i < treeWidget->topLevelItemCount(); i++){
        if(treeWidget->topLevelItem(i)->checkState(0)){
            types_filters_list.append(treeWidget->topLevelItem(i)->text(0).split(" ")[0]);
        }
    }
    FilterTable(types_filters_list);
    //FilterChart(types_filters_list);
}

void MainWindow::FilterTable(const QStringList& types_filters_list){
    // фильтруем содержимое таблицы в соответствии с types_filters_list()
    for(int i = 0; i < tableWidget->rowCount(); i++){
        if(types_filters_list.contains(tableWidget->item(i, 2)->text())){
            if (tableWidget->isRowHidden(i)){
                tableWidget->setRowHidden(i, false);
            }
        } else{
            tableWidget->setRowHidden(i, true);
        }
    }
}
/*
// приходится хранить set_map и ещё series ,чтобы добавлять или удалять
void MainWindow::FilterChart(const QStringList& types_filters_list){
    for(auto type : set_map.keys()){
        if(types_filters_list.contains(type)){
            if(chartView->chart().));
        } else
    }
}
*/
//================================================private slots:============================================================================================================

// =============== посылают сигналы в контроллер ====================

void MainWindow::on_pushButton_clicked()  // нажата кнопка "Открыть файл"
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Документ (разделитель - ;) (*.txt);; JSON (*.json)");
    QString label_file_name = filename.split("/").takeLast();
    ui->label->setText("Файл " + label_file_name);

    emit OpenFileClicked(filename);
}

void MainWindow::on_pushButton_clean_clicked() // кнопка "Очистить"
{
    emit CleanFiltersClicked();
}

void MainWindow::on_pushButton_2_clicked() // кнопка "Сбросить"
{
    emit RestoreDataRange();
}

void MainWindow::ChangeDateTimeRange(){  // при изменении диапазона дат

}

// =============== вызываются в контроллере ====================
void MainWindow::GetDataAndFillWindow(Data_Model &data_model){  // заполнение окна после нажатия кнопки "Открыть"
    fill_chart(data_model.chart_map, data_model.time_range);
    fill_table(data_model.data_vector);
    fill_filters(data_model.filters_struct);

    // отображение данных о лог-файле
    ui->label_3->setText("Всего записей: " + QString::number(data_model.data_vector.size()));
    ui->dateTimeEdit->setDateTime(data_model.data_vector[0].date_time);
    ui->dateTimeEdit_2->setDateTime(data_model.data_vector.last().date_time);
}

void MainWindow::RestoreDateTimeRange(QDateTime& start, QDateTime& finish){
    ui->dateTimeEdit->setDateTime(start);
    ui->dateTimeEdit_2->setDateTime(finish);
}
