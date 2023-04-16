#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <qcustomplot.h>
#include <QPainter>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_2_clicked();
    void recv_data(void);
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void customsplot_init();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer *sendTimer;
    QCustomPlot *plot;
    QCPItemText *textItem;
    int sendCount=0;
};
#endif // MAINWINDOW_H
