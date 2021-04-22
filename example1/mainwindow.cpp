#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readdata.h"

#include <QTextStream>
#include <QDateTime>
#include <QMap>

#include <QFileDialog>

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
    chart->setTitle("Зависимость количества логов от даты");
    chart->legend()->hide();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(820,300);
}

/*
void MainWindow::create_table(){

}

void MainWindow::fill_table(){

}

*/

//Функция построения графика по считанным вершинам
//values - массив вершин графика
void MainWindow::build_chart(const QMap<QDate, int>& values){
    if(!values.empty()){        // если значение массива вершин графика непустое - строим график
        QChart *chart = chartView->chart();
        chart->removeAllSeries(); // удалится ли при этом указатель на series?
        QLineSeries* series = new QLineSeries(this);
        //заполнение значений графика
        for(auto& key : values.keys()){
            QDateTime k(key);
            series->append(k.toMSecsSinceEpoch(), values.value(key));
        }
        chart->addSeries(series);
        if(chart->axisX() && chart->axisY()){     // удаляем оси, если они уже были установлены
            chart->removeAxis(chart->axisX());
            chart->removeAxis(chart->axisY());
        }

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
        if(chartView->isHidden())
            chartView->setHidden(false);
        ui->gridLayout->addWidget(chartView, 4, 0, -1, -1);

} else{    // если массив вершин графика пустой, то делаем график невидимым и перестраиваем размер окна
        QLayoutItem *child;
        if((child = ui->gridLayout->takeAt(4)) != nullptr){
            chartView->setHidden(true);
            ui->gridLayoutWidget->adjustSize();
            adjustSize();
        }
        delete child;
    }
}

void MainWindow::on_pushButton_2_clicked()  // кнопка "Открыть и построить"
{
    ReadData data_read(ui->lineEdit->text());
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    QMap<QDate, int> values = data_read.make_date_number_map(data_vector);
    build_chart(values);
}

void MainWindow::on_action_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Документ (разделитель - ;) (*.txt);; JSON (*.json)");
    ui->lineEdit->setText(filename);
    ReadData data_read(filename);
    QVector<date_time_type_msg> data_vector = data_read.file_read();
    QMap<QDate, int> values = data_read.make_date_number_map(data_vector);
    build_chart(values);
}
