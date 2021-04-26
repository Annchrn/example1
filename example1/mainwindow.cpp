#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readdata.h"

#include <QTextStream>
#include <QDateTime>
#include <QMap>

#include <QFileDialog>
#include <QTableWidget>
#include <QTableWidgetItem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->gridLayoutWidget);
    // внешний вид окна
    ui->gridLayout->setMargin(15);
    ui->lineEdit->setMinimumWidth(500);
    adjustSize();
    // поле ввода
    ui->lineEdit->setPlaceholderText("Файл не выбран");
    ui->pushButton_2->setDisabled(true);
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    // график
    create_chart();
    //таблица
    create_table();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Делает кнопку неактивной, если lineEdit пуст
void MainWindow::textChanged(){
    if(ui->lineEdit->text() != ""){
        ui->pushButton_2->setEnabled(true);
    }else{
        ui->pushButton_2->setDisabled(true);
    }
}

// Инициализация графика
void MainWindow::create_chart()
{
    QChart *chart = new QChart();
    // отображение
    chartView = new QChartView(chart);
    ui->gridLayout->addWidget(chartView, 3, 0, 1, 0);
    chart->setTitle("Зависимость количества логов от даты");
    chart->legend()->hide();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(820,200);
   // chartView->setHidden(true);
}

// Инициализация таблицы
void MainWindow::create_table(){
    //таблица
    tableWidget = new QTableWidget(this);
    ui->gridLayout->addWidget(tableWidget, 4, 0, 1, 0);
    tableWidget->setMinimumSize(820,500);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // столбцы
    tableWidget->setColumnCount(3);
    QStringList name_table;
    name_table << "Дата" << "Тип" << "Сообщение";
    tableWidget->setHorizontalHeaderLabels(name_table);
    tableWidget->setColumnWidth(0, 200);
    tableWidget->setColumnWidth(1, 100);
    tableWidget->horizontalHeader()->setStretchLastSection(true);

    tableWidget->setFont(QFont("Times", 9));
    tableWidget->horizontalHeader()->setFont(QFont("Times", 9));//, QFont::Bold));
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}" );
    // padding-left:4px; border:1px solid blue; }" "QHeaderView::section:checked{background-color:grey; }"
    // , QFont::Cursive
    // прячем таблицу
  //  tableWidget->setHidden(true);
}

//Функция построения графика (зависимость даты от количества лог-записей)
//values - массив вершин графика
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

void MainWindow::build_chart(const QMap<QDate, int>& values){
    if(!values.empty()){        // если массив вершин графика непустой - строим график
        clear_chart();
        QChart *chart = chartView->chart();
    //    chart->removeAllSeries();
        //заполнение значений графика
        QLineSeries* series = new QLineSeries(this);
        for(const auto& key : values.keys()){
            QDateTime k(key);
            series->append(k.toMSecsSinceEpoch(), values.value(key));
        }
        chart->addSeries(series);

        // оси х, у
        QDateTimeAxis *axisX = new QDateTimeAxis(this);
        axisX->setTickCount(10);
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
   //    ui->gridLayout->addWidget(chartView, 3, 0, 1, 0);

} else{    // если массив вершин графика пустой, то делаем график невидимым и перестраиваем размер окна
        QLayoutItem *child;
        if((child = ui->gridLayout->takeAt(4)) != nullptr){
       //     chartView->setHidden(true);
        //    tableWidget->setHidden(true);
            clear_chart();
            tableWidget->clearContents();
            ui->gridLayoutWidget->adjustSize();
            adjustSize();
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
            // 2-й столбец - "Тип"
            QTableWidgetItem* item1 = new QTableWidgetItem();
            item1->setTextAlignment(84);
            item1->setText(data_vector[i].type);
            tableWidget->setItem(i, 1, item1);
            // 3-й столбец - "Сообщение"
            QTableWidgetItem* item2 = new QTableWidgetItem();
            item2->setTextAlignment(84);
            item2->setText(data_vector[i].message);
            tableWidget->setItem(i, 2, item2);

            if(data_vector[i].type == "FTL"){
                tableWidget->item(i, 0)->setForeground(Qt::red);
                tableWidget->item(i, 1)->setForeground(Qt::red);
                tableWidget->item(i, 2)->setForeground(Qt::red);
            }

            tableWidget->setHidden(false); // отображаем таблицу
      //      ui->gridLayout->addWidget(tableWidget, 4, 0, 1, 0);
        }
    }
}

//void MainWindow::update_Window(){

//}

void MainWindow::on_pushButton_2_clicked()  // кнопка "Открыть и построить"
{
    ReadData data_read(ui->lineEdit->text());
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    QMap<QDate, int> values = data_read.make_date_number_map(data_vector);
    build_chart(values);
}

void MainWindow::on_action_triggered()  // открытие через меню
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Документ (разделитель - ;) (*.txt);; JSON (*.json)");
    ui->lineEdit->setText(filename);
    ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    QMap<QDate, int> values = data_read.make_date_number_map(data_vector);
    build_chart(values);
    build_table(data_vector);
}
