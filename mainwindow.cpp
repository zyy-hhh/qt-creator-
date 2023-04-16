#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <qcustomplot.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Display")); //标题
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//查找串口
    {
        ui->combox_COMx->addItem(info.portName());//加入搜索到的串口并填入下拉菜单
    }
    serial = new QSerialPort(this);
    sendTimer = new QTimer(this);//定时器
    //    sendTimer->setInterval(1000);//时间间隔1000ms=1s
    //    connect(sendTimer, SIGNAL(timeout()), this, SLOT(on_pushButton_3_clicked()));
    ui->customPlot->yAxis->setRange(15,60);
    //  connect(sendTimer, SIGNAL(timeout),this, SLOT(upData()));
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(on_pushButton_3_clicked()));
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(customsplot_init()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//打开串口按钮
void MainWindow::on_pushButton_2_clicked()
{
    if(ui->pushButton_2->text()==tr("打开串口"))
    {
        ui->pushButton_2->setText(tr("关闭串口"));

        serial->setPortName(ui->combox_COMx->currentText());

        if(serial->open(QIODevice::ReadWrite))
        {
            qDebug()<<"OK!"<<endl;
            serial->setBaudRate(QSerialPort::Baud9600);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setStopBits(QSerialPort::OneStop);
            connect(serial, SIGNAL(readyRead()), this, SLOT(recv_data()));
        }
        else
        {
            qDebug()<<"FAILED"<<endl;
        }
    }
    else
    {
        ui->pushButton_2->setText(tr("打开串口"));
        serial->close();
    }
}

//发送按钮
void MainWindow::on_pushButton_3_clicked()

{
    QString sendData = ui->textEdit->toPlainText();
    QByteArray sendDataHex = QByteArray::fromHex(sendData.toUtf8());
    serial->write(sendDataHex);
    //      sendTimer->start(1000);
    sendTimer->start(1000);
    sendCount++;
}
static double num_1,num_2;

//数据接收和转换与显示
void MainWindow::recv_data(void)
{
    qDebug()<<"REcv Data"<<endl;
    QByteArray data = serial->readAll();
    QString readData = QString(data.toHex());
    ui->textEdit_ReceiveMsg->append(readData);
    //16提取转10
    int humidity = readData.mid(6,4).toInt(nullptr,16);
    int temperature = readData.mid(10,4).toInt(nullptr,16);
    int value_1 = humidity;
    int value_2 = temperature;
    static QString formatted_1;
    static QString formatted_2;
    formatted_1 = QString("%1").arg(value_1*0.1,0,'f',2);
    formatted_2 = QString("%1").arg(value_2*0.1,0,'f',2);
    qDebug()<<"Humidity"<<formatted_1<<"temperature"<<formatted_2<<endl;
    ui->textEdit_ReceiveMsg_shidu->append(formatted_1);
    ui->textEdit_ReceiveMsg_wendu->append(formatted_2);
    //绘图初始化
    // 添加曲线 1
    ui->customPlot->addGraph();         // 增加图层
    ui->customPlot->graph(0)->setPen(QPen(Qt::red));//画线
    ui->customPlot->graph(0)->setBrush(QBrush(QColor(0,210,0,30)));     // 填充颜色
    // ui->customPlot->graph(0)->setAntialiasedFill(false);        // 设置图层反锯齿：关闭

    // 添加曲线 2
    ui->customPlot->addGraph();
    //    ui->customPlot->graph(1)->setPen(QPen(Qt::blue,2));
    ui->customPlot->graph(1)->setPen(QPen(Qt::blue));       // 设置笔的颜色
    //    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);   // 不画线条
    //    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);     // 设置点的形状
    ui->customPlot->graph(1)->setBrush(QBrush(QColor(210,0,0,30)));     // 填充颜色
    //坐标轴1设置
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");             //时间格式：  秒
    ui->customPlot->xAxis->setTicker(timeTicker);       // 横坐标显示时间
    //   ui->customPlot->axisRect()->setupFullAxesBox();     //坐标为矩
    // x轴设置
    ui->customPlot->xAxis->setLabel("时间(单位s)");         // 设置横轴标签
    ui->customPlot->yAxis->setRange(13,80);       //设置y轴数值范围
    ui->customPlot->xAxis->setLabel("时间");
    ui->customPlot->yAxis->setLabel("数值");
    num_1 = formatted_1.toDouble();
    num_2 = formatted_2.toDouble();
}

// 绘图
void MainWindow::customsplot_init()
{
    qDebug()<<"1"<<sendCount<<endl;
    ui->customPlot->graph(0)->addData(sendCount, num_1);
    ui->customPlot->graph(1)->addData(sendCount, num_2);
    ui->customPlot->xAxis->setRange(sendCount, 25, Qt::AlignRight);        //设置x轴范围
    ui->customPlot->replot();       //画图
}

//清除按钮
void MainWindow::on_pushButton_clicked()
{
    ui->textEdit_ReceiveMsg->clear();
}



