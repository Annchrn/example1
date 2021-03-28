#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QMap>
#include <QtCharts>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setCentralWidget(ui->gridLayoutWidget);
    ui->gridLayout->setMargin(15);
    adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool file_open(const QString& filename){
    try {
        if (!filename.isEmpty()){
            QFile file(filename);
            if (file.exists()){
                if (file.open(QIODevice::ReadOnly)){ // если файл существует и открылся для чтения
                    return true;
                } else {throw std::runtime_error("Не удалось открыть файл");}
             } else {throw std::runtime_error("Файл не существует");}
            file.close();
        } else {throw std::runtime_error("Введите путь к файлу");}
    }  catch (std::exception& ex) {
        QMessageBox::critical(0, "Ошибка", ex.what());
        return false;
    }
}

QMap<QDateTime, int> file_read(const QString& filename){
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QMap<QDateTime, int> values;
    QTextStream stream(&file);

    while (!stream.atEnd()){
      QString line = stream.readLine();
      QStringList line_values = line.split(";");
      QStringList date_values = line_values[0].split(".");
      QDateTime date;
      date.setDate(QDate(date_values[0].toInt(), date_values[1].toInt(), date_values[2].toInt()));
      values[date] = line_values[1].toInt();
    }
    return values;
}

void MainWindow::build_chart(const QMap<QDateTime, int>& values){
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
    ui->gridLayout->addWidget(chartView, 3, 0, -1, -1);
}

void MainWindow::on_pushButton_2_clicked()  // кнопка "Открыть"
{
    QString filename = ui->lineEdit->text();
    if (file_open(filename)){
         QMap<QDateTime, int> values = file_read(filename);
         build_chart(values);
    }
}

void MainWindow::on_open_action_triggered()    // открытие ч/з меню
{
        QString filename = QFileDialog::getOpenFileName(this, "Выберите файл", "", "*.txt");
        if (file_open(filename)){
            ui->lineEdit->setText(filename);
            QMap<QDateTime, int> values = file_read(filename);
            build_chart(values);
        }
}
