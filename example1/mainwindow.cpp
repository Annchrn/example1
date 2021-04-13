#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "readdata.h"

#include <QTextStream>
#include <QDateTime>
#include <QMap>
#include <QtCharts>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setCentralWidget(ui->gridLayoutWidget);
    ui->gridLayout->setMargin(15);
    ui->lineEdit->setMinimumWidth(500);
    adjustSize();

    ui->lineEdit->setPlaceholderText("Файл не выбран");
    ui->pushButton_2->setDisabled(true);
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
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

//Функция построения графика по считанным вершинам
//values - массив вершин графика
void MainWindow::build_chart(const QMap<QDateTime, int>& values){
    if(!values.empty()){
        //заполнение
        QLineSeries *series = new QLineSeries();
        for(auto& key : values.keys()){
            series->append(key.toMSecsSinceEpoch(), values.value(key));
        }

        //график
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->legend()->hide();
        chart->setTitle("Зависимость значений от даты");

        //ось х
        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setTickCount(10);
        axisX->setFormat("dd.MM.yyyy");
        axisX->setTitleText("Дата");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        //ось у
        QValueAxis *axisY = new QValueAxis;
        axisY->setLabelFormat("%i"); // ("%d")
        axisY->setTitleText("Значения");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        //отображение
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(820,400);
        ui->gridLayout->addWidget(chartView, 4, 0, -1, -1);
    }
}

void MainWindow::on_pushButton_2_clicked()  // кнопка "Открыть и построить"
{
    ReadData data_read(ui->lineEdit->text(), ui->comboBox->currentText());
    QMap<QDateTime, int> values = data_read.file_read();
    build_chart(values);
}

void MainWindow::on_action_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "");
    ui->lineEdit->setText(filename);
}
