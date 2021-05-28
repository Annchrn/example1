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

    create_chart(); // график
    create_date_time_edit_elements();

    create_tree(); // фильтры
    create_table(); //таблица

    // добавление кнопки "Очистить"
    clean_filters_button = new QPushButton("Очистить", this);
    ui->gridLayout->addWidget(clean_filters_button, 4, 0, 1, 1);

    expand_button = new QPushButton(this);
    ui->gridLayout->addWidget(expand_button, 4, 0, 1, 1);
    expand_button->setIcon(QIcon(":/arrows_icon.jpg"));
    expand_button->setFixedSize(45, 15);
    expand_button->setIconSize(expand_button->size());
    //expand_button->setStyleSheet("   border-width: 12px;     border-color: black;");

    // сигналы
    connect(clean_filters_button, SIGNAL (clicked()), this, SLOT (on_pushButton_clean_clicked()));
    connect(restore_date_time_range_button, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(dateTimeEdit, SIGNAL(EnterPressed()), this, SLOT(ChangeDateTimeRange()));
    connect(dateTimeEdit_2, SIGNAL(EnterPressed()), this, SLOT(ChangeDateTimeRange()));
    connect(expand_button, SIGNAL(clicked()), this, SLOT(expand_and_collapse_treeWidget()));

   // a = new QGridLayout(this);
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
    tableWidget->setColumnCount(6);
    for(int i = 2; i < 6; i++)
        tableWidget->setColumnHidden(i, true);
    tableWidget->verticalHeader()->setVisible(false);

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
void MainWindow::create_tree()
{
    treeWidget = new QTreeWidget(this);
    treeWidget->setMinimumHeight(500);
    treeWidget->setMaximumWidth(200);
    ui->gridLayout->addWidget(treeWidget, 3, 0, 1, 1);
   // treeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
   // treeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    treeWidget->setHeaderLabel("Фильтры");
    treeWidget->headerItem()->setIcon(0, QIcon(":/filter_icon.png"));
   // treeWidget->headerItem()->setBackground(0, )
   // treeWidget->headerItem()->setBackgroundColor(0, Qt::blue);
    // добавление элементов
}

void MainWindow::create_date_time_edit_elements(){
    // Начало
    QLabel *label = new QLabel(this);
    label->setText("Начало:");
    ui->gridLayout->addWidget(label, 1, 0);

    // начало диапазона
    dateTimeEdit = new DateTimeEdit(ui->gridLayoutWidget);
    dateTimeEdit->setObjectName(QString::fromUtf8("dateTimeEdit"));
    dateTimeEdit->setEnabled(true);
    dateTimeEdit->setCalendarPopup(true);
    dateTimeEdit->setDisplayFormat("dd:MM:yyyy hh:mm:ss");
    dateTimeEdit->setMaximumWidth(180);
    //ui->gridLayout->addWidget(dateTimeEdit, 1, 2, 1, 1);
    ui->gridLayout->addWidget(dateTimeEdit, 1, 1);

    // конец
    QLabel *label1 = new QLabel(this);
    label1->setText("Конец:");
    ui->gridLayout->addWidget(label1, 1, 2);

    // конец диапазона
    dateTimeEdit_2 = new DateTimeEdit(ui->gridLayoutWidget);
    dateTimeEdit_2->setObjectName(QString::fromUtf8("dateTimeEdit_2"));
    dateTimeEdit_2->setCalendarPopup(true);
    dateTimeEdit_2->setDisplayFormat("dd:MM:yyyy hh:mm:ss");
    dateTimeEdit_2->setMaximumWidth(180);
    //ui->gridLayout->addWidget(dateTimeEdit_2, 1, 4, 1, 1);
    ui->gridLayout->addWidget(dateTimeEdit_2, 1, 3);

    // кнопка "Сбросить"
    restore_date_time_range_button = new QPushButton("Сбросить", this);
    ui->gridLayout->addWidget(restore_date_time_range_button, 1, 4);
    restore_date_time_range_button->setMaximumWidth(80);

    // всего сообщений
    messages_counter = new QLabel(this);
    messages_counter->setText("Всего записей:");
    ui->gridLayout->addWidget(messages_counter, 1, 5);

}

//================================== работа с таблицей, графиками и фильтрами =====================================================================================================================================
// Функция очистки содержимого окна
void MainWindow::clear_window_contents(){
    QLayoutItem *child;
    if((child = ui->gridLayout->takeAt(4)) != nullptr){
        clear_chart();
        tableWidget->clearContents();
        tableWidget->setRowCount(0);
        // treeWidget->clear();
        for(int i = 0; i < treeWidget->topLevelItemCount(); i++){
            for(int k = treeWidget->topLevelItem(i)->childCount(); k >= 0; k--){
                treeWidget->topLevelItem(i)->removeChild(treeWidget->topLevelItem(i)->child(k));
            }
        }
    }
    messages_counter->setText("Всего записей: ");
    delete child;
}

// ==================== график ====================

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
void MainWindow::fill_chart(const QMap<QDateTime, QMap<QString, int>>& types_map,const int& range)
{        clear_chart(); // очищаем старое содержимое графика
         QMap<QString, QBarSet*> set_map;
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
void MainWindow::create_axisX(QBarCategoryAxis *axisX, const QVector<QDateTime>& dates_values,const int& range){
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

// ==================== таблица и фильтры ====================

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

            QTableWidgetItem* item3 = new QTableWidgetItem();
            item3->setText(data_vector[i].type);
            tableWidget->setItem(i, 2, item3);

            QString message;
            if(data_vector[i].user != ""){
                QTableWidgetItem* item4 = new QTableWidgetItem();
                item4->setText(data_vector[i].user);
                tableWidget->setItem(i, 3, item4);
                message += (" User= " + data_vector[i].user);
            }

            if(data_vector[i].session_level != ""){
                QTableWidgetItem* item5 = new QTableWidgetItem();
                item5->setText(data_vector[i].session_level);
                tableWidget->setItem(i, 4, item5);
                message += (" SessionLevel= " + data_vector[i].session_level);
            }

            if(data_vector[i].server_name != ""){
                QTableWidgetItem* item6 = new QTableWidgetItem();
                item6->setText(data_vector[i].server_name);
                tableWidget->setItem(i, 5, item6);
                message += (" ServerName= " + data_vector[i].server_name);
            }

            // 2-й столбец - "Сообщение"
            QTableWidgetItem* item1 = new QTableWidgetItem();
            item1->setTextAlignment(84);
            message += (" Message= "+ data_vector[i].message);
            item1->setText(message);
            tableWidget->setItem(i, 1, item1);

            if(data_vector[i].type == "FATAL"){
                tableWidget->item(i, 0)->setForeground(Qt::red);
                tableWidget->item(i, 1)->setForeground(Qt::red);
            }

            tableWidget->setHidden(false); // отображаем таблицу
        }
    }

// Функция заполнения панели фильтров
void MainWindow::fill_top_level_items(const QMap<QString, int>& map, const int& top_level_item_index)
{
    for(const auto& type : map.keys()){
       QTreeWidgetItem *type_item = new QTreeWidgetItem(treeWidget->topLevelItem(top_level_item_index-1));
        type_item->setText(0, type + " (" + QString::number(map.value(type)) + ")");
        type_item->setCheckState(0, Qt::Checked);
    }
}

void MainWindow::fill_filters(const Filters_structure& filters_struct){
    disconnect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem* , int)), this, SLOT(ChangeTypeFilters(QTreeWidgetItem*, int)));

    // очищаем старые фильтры
    for(int i = treeWidget->topLevelItemCount() -1; i >= 0 ; i--){
        int type_item_index = treeWidget->topLevelItem(i)->childCount();
        while(type_item_index != -1){
            treeWidget->topLevelItem(i)->removeChild(treeWidget->topLevelItem(i)->takeChild(type_item_index));
            type_item_index--;
        }
        delete treeWidget->takeTopLevelItem(i);
    }

    // заполняем фильтры для Уровня сообщений новыми значениями
    QTreeWidgetItem *itm = new QTreeWidgetItem(treeWidget);
    itm->setText(0, "Уровень сообщения");
    itm->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);
    fill_top_level_items(filters_struct.types_map, treeWidget->topLevelItemCount());

    if(!filters_struct.users_map.empty()){
        QTreeWidgetItem *itm1 = new QTreeWidgetItem(treeWidget);
        itm1->setText(0, "Пользователь");
        itm1->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);
        fill_top_level_items(filters_struct.users_map, treeWidget->topLevelItemCount());
        qDebug() << "пользователь";
    }
    if(!filters_struct.levels_map.empty()){
        QTreeWidgetItem *itm2 = new QTreeWidgetItem(treeWidget);
        itm2->setText(0, "Уровень доступа");
        itm2->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);
        fill_top_level_items(filters_struct.levels_map, treeWidget->topLevelItemCount());
        qDebug() << "уровень доступа";
    }
    if(!filters_struct.servers_map.empty()){
        QTreeWidgetItem *itm3 = new QTreeWidgetItem(treeWidget);
        itm3->setText(0, "Имя сервера");
        itm3->setChildIndicatorPolicy(QTreeWidgetItem::ChildIndicatorPolicy::DontShowIndicatorWhenChildless);
        fill_top_level_items(filters_struct.servers_map, treeWidget->topLevelItemCount());
        qDebug() << "имя сервера";
    }
    treeWidget->expandAll();
    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(ChangeTypeFilters(QTreeWidgetItem*,int)));
}

// ==================== фильтрация таблицы при измененении фильтров ====================

void MainWindow::TypeFilterTable(const QStringList& types_filters_list){
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

//================================================private slots:====================================================================================================================================================

void MainWindow::expand_and_collapse_treeWidget(){
    if(treeWidget->width() > 60){
        //treeWidget->setColumnHidden(0, true);
        //treeWidget->setHeaderHidden(true);
        treeWidget->setMaximumWidth(60);
        clean_filters_button->setHidden(true);
    } else{
        treeWidget->setMaximumWidth(180);
        clean_filters_button->setHidden(false);
    }
}

// =============== посылают сигналы в контроллер ==============================================================

// нажата кнопка "Открыть файл"
void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Документ (разделитель - ;) (*.txt);; JSON (*.json)");
    QString label_file_name = filename.split("/").takeLast();
    ui->label->setText("Файл " + label_file_name);

    emit OpenFileClicked(filename);
}

// кнопка "Очистить"
void MainWindow::on_pushButton_clean_clicked()
{
    emit CleanFiltersClicked();
}

// кнопка "Сбросить"
void MainWindow::on_pushButton_2_clicked()
{
    emit RestoreDataRange();
}

// при изменении фильтров уровня сообщений
void MainWindow::ChangeTypeFilters(QTreeWidgetItem*, int)
{
    QStringList types_filters_list;
    for(int i = 0; i < treeWidget->topLevelItem(0)->childCount(); i++){
        if(treeWidget->topLevelItem(0)->child(i)->checkState(0)){
            types_filters_list.append(treeWidget->topLevelItem(0)->child(i)->text(0).split(" ")[0]);
        }
    }
    TypeFilterTable(types_filters_list);
    emit TypeFiltersChanged(types_filters_list);
}

// при изменении диапазона дат
void MainWindow::ChangeDateTimeRange()
{
    QDateTime temp1 = dateTimeEdit->dateTime();
    QDateTime temp2 = dateTimeEdit_2->dateTime();
    emit DateTimeChanged(temp1, temp2);
}

// =============== вызываются в контроллере ==================================================================

// заполнение окна после нажатия кнопки "Открыть"
void MainWindow::GetDataAndFillWindow(Data_Model &data_model){
    fill_chart(data_model.chart_map, data_model.time_range);
    fill_table(data_model.data_vector);
    fill_filters(data_model.filters_struct);

    // отображение данных о лог-файле
    messages_counter->setText("Всего записей: " + QString::number(data_model.data_vector.size()));
    dateTimeEdit->setDateTime(data_model.data_vector[0].date_time);
    dateTimeEdit_2->setDateTime(data_model.data_vector.last().date_time);
}

// для восстановления изначального временного диапазона
void MainWindow::RestoreDateTimeRange(QDateTime& start, QDateTime& finish){
    dateTimeEdit->setDateTime(start);
    dateTimeEdit_2->setDateTime(finish);
}

void MainWindow::RebuildChart_handler(const QMap<QDateTime, QMap<QString, int>>& chart_map,const int& range,const int& counter){
    fill_chart(chart_map, range);
    messages_counter->setText("Всего записей: " + QString::number(counter));
}

void MainWindow::GetDataAndRebuildWindow(Data_Model &data_model){
    fill_chart(data_model.chart_map, data_model.time_range);
    fill_table(data_model.data_vector);

    fill_filters(data_model.filters_struct);
    messages_counter->setText("Всего записей: " + QString::number(data_model.data_vector.size()));
}
