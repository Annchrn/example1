#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void textChanged();

private slots:
    void on_pushButton_2_clicked();
    void on_action_triggered();

private:
    Ui::MainWindow *ui;

    void build_chart(const QMap<QDateTime, int>& values);
};
#endif // MAINWINDOW_H
