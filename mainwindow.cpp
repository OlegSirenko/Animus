#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "math.h"
#include "QDebug"
#include <QPixmap>
#include "joypad.h"
#include "QPushButton"
#include "QLabel"
#include "QSettings"
#include "QGridLayout"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pixViewSide("://ViewPicture.png");
    QMatrix rm;
    rm.rotate(90);
    pixViewSide = pixViewSide.transformed(rm);

    settings = new QSettings("gst_settings");
    settings->beginGroup("Connection");
    gst_addr = settings->value("Connection/gst_address").toString();
    gst_port = settings->value("Connection/gst_port").toString();

    qDebug()<<gst_port<<gst_addr;
    if (gst_addr.isEmpty() || gst_addr.isEmpty()){
        gst_addr = "127.0.0.1";
        gst_port = "7010";
    }

    ui->view_2->setPixmap(pixViewSide);
    ui->view_2->setVisible(false);
    ui->view->setVisible(false);
    server = new QTcpServer();
    if(!server->listen(QHostAddress(""), 12345)) {
        qDebug()<<server->errorString();
        ui->textBrowser->append(server->errorString());
        return;
    }
    ui->pushButton->setStyleSheet("background-color: blue");
    ui->pushButton->setEnabled(false);
    ui->textBrowser->append("Ожидание подключения");
    qDebug("Start Listening Socket!");
    connect(server,&QTcpServer::newConnection, this, &MainWindow::server_New_Connect);
    connect(ui->widget, &JoyPad::xChanged, this, [this](float x){
            MainWindow::send_joypad_acc(x, ui->widget->y());
    });
    connect(ui->widget, &JoyPad::yChanged, this, [this](float y){
            MainWindow::send_joypad_acc(ui->widget->x(), y);
    });
    timer = new QTimer;
    timer ->setInterval(20);
    connect(timer, SIGNAL(timeout()), this,  SLOT(send_static()));


    //viewWidget = new PlaneView;
    //connect(this, SIGNAL(joysticTouchedSignal(int,int,int)), viewWidget, SLOT(joystic_touched(int,int,int)));

    myWorker = new worker;
    WorkerThread = new QThread;

    myWorker->moveToThread(WorkerThread);
    qDebug()<<"Gstreamer Thread Started";


    connect(this, SIGNAL(startWorkSignal()), myWorker,SLOT(StartWork()));
    connect(this, SIGNAL(stopWorkSignal()), myWorker, SLOT(StopWork()));
    connect(this, SIGNAL(change_pipeline_addr_port(QString,QString)), myWorker, SLOT(address_port_changed(QString,QString)));
    connect(myWorker, SIGNAL(state_changed(QString)), this, SLOT(state_changed(QString)));
    connect(myWorker, SIGNAL(onNewFrame(QImage)), this, SLOT(drawFrame(QImage)));
    WorkerThread->start();
    emit change_pipeline_addr_port(gst_addr, gst_port);
    emit startWorkSignal();
}


MainWindow::~MainWindow()
{
    emit stopWorkSignal();
    socket->close();
    server->close();
    WorkerThread->exit();
    delete ui;
}


void MainWindow::server_New_Connect(){
    socket = server->nextPendingConnection();
    qDebug()<<"Connected!";
    timer->start();
    ui->textBrowser->append("Плагин подключен!");
    ui->view->setVisible(true);
    ui->view_2->setVisible(true);
    QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);
}


void MainWindow::socket_Disconnected(){
    ui->textBrowser->append("Связь потеряна!");
    qDebug()<<"Disconnected!";
}


void MainWindow::socket_Read_Data(){
    QByteArray data;
    data = socket->readAll();
    if (data.isEmpty()){
        qDebug() << data;
        ui->textBrowser->append(data);
    }
}


void MainWindow::on_verticalSlider_valueChanged(int value) // send zoom
{
    zoom = value;
}


void MainWindow::send_joypad_acc(float x, float y){
    (heading >= 0 && heading <= 360)? heading += 1 * x : heading -= 360 * x/abs(x); // if heading in [0, 360] => grow up, else start from 0
    (pitch >= -90 && pitch <= 0)? pitch += 1*y : (pitch>=0)? pitch = 0 : pitch = -90;
}


void MainWindow::send_static(){
    sending = QString::number(heading+180)+QString::number(abs(pitch)+180)+QString::number(zoom)+'\0';
    socket->write(sending.toLocal8Bit());

    if (socket->flush()){
        QPixmap pixView("://ViewPicture.png");
        QPixmap pixViewSide(pixView);
        QMatrix rm;
        rm.rotate(180+heading);
        pixView = pixView.transformed(rm);
        ui->view->setPixmap(pixView);


        QMatrix rm_side;
        rm_side.rotate(-270-pitch);
        pixViewSide = pixViewSide.transformed(rm_side);
        ui->view_2->setPixmap(pixViewSide);
        int center_rotation_x_top = ui->topview->x() + ui->topview->width()/2 - 15;
        int center_rotation_y_top = ui->topview->y() + ui->topview->height()/2 - 15;

        int center_rotation_x_side = ui->sideview->x() + ui->sideview->width()/2;
        int center_rotation_y_side = ui->sideview->y() + ui->sideview->height()/2 - 15;

        ui->view->move((center_rotation_x_top + (70 * -sin(heading*3.14/180))), (center_rotation_y_top + (70 * cos(heading*3.14/180))));
        ui->view_2->move((center_rotation_x_side - (40 * sin((270-pitch)*3.14/180))), (center_rotation_y_side + (40 * cos((270-pitch)*3.14/180))));
    }
}



void MainWindow::state_changed(QString str){
    ui->textBrowser_2->append("GStreamer: "+str);
}

void MainWindow::drawFrame(QImage img){
    //ui->VideoLabel->setGeometry(ui->centralwidget->size());
    ui->VideoLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->VideoLabel->size()));
    //update();
}

void MainWindow::on_pushButton_clicked()
{
    ui->widget->setEnabled(true);
    ui->pushButton->setStyleSheet("background-color: blue");
    ui->pushButton_2->setStyleSheet("background-color: none");
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);

}


void MainWindow::on_pushButton_2_clicked()
{
    ui->widget->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->pushButton->setStyleSheet("background-color: none");
    ui->pushButton_2->setStyleSheet("background-color: blue");
    ui->pushButton_2->setEnabled(false);

    heading = 0;
    pitch = 0;
}


void MainWindow::on_pushButton_3_clicked()
{

    ui->textBrowser_2->append("Перезапуск Gstreamer...");
    emit stopWorkSignal();
    myWorker->running = false;
    myWorker->stopped = true;
    qDebug() << "stopwork signal emmitted";
    WorkerThread->start();
    qDebug() << "startwork signal emmitted";
    emit startWorkSignal();
    ui->textBrowser_2->append("Перезапущен!");
}


void MainWindow::on_settings_clicked()
{
    settings_dialog = new QDialog;
    QGridLayout *layout = new QGridLayout;
    QPushButton *submit = new QPushButton;
    QPushButton *cancel = new QPushButton;
    address = new QLineEdit;
    port = new QLineEdit;
    QLabel *label_addr = new QLabel;
    QLabel *label_port = new QLabel;
    settings_dialog->setModal(true);
    settings_dialog->setWindowTitle("Gstreamer settings");
    settings_dialog->setWindowIcon(QIcon(QPixmap("://settings_icon_2.png")));

    connect(submit, SIGNAL(clicked()), this, SLOT(on_submit_dialog_button_clicked()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(on_cancel_dialog_button_clicked()));

    settings_dialog->setLayout(layout);

    submit->setText("Подтвердить") ;
    cancel->setText("Отмена");

    label_addr->setText("Адрес трансляции:");
    label_port->setText("Порт трансляции:");

    address->setText(gst_addr);
    port->setText(gst_port);

    layout->addWidget(label_addr, 1, 1);
    layout->addWidget(label_port, 2, 1);
    layout->addWidget(address, 1, 2);
    layout->addWidget(port, 2, 2);
    layout->addWidget(submit, 3, 1);
    layout->addWidget(cancel, 3, 2);
    settings_dialog->show();
}


void MainWindow::on_submit_dialog_button_clicked(){

    if(address->text().isEmpty() || port->text().isEmpty()){
        if (address->text().isEmpty()){
            address->setStyleSheet("QLineEdit { background-color: yellow }");
        }
        if (port->text().isEmpty()){
            port->setStyleSheet("QLineEdit { background-color: yellow }");
        }
        return;
    }

    ui->textBrowser_2->append("Перезапуск Gstreamer...");
    emit stopWorkSignal();
    myWorker->running = false;
    myWorker->stopped = true;
    qDebug() << "stopwork signal emmitted";
    emit change_pipeline_addr_port(address->text(), port->text());
    gst_addr = address->text();
    gst_port = port->text();
    WorkerThread->start();
    qDebug() << "startwork signal emmitted";
    emit startWorkSignal();
    ui->textBrowser_2->append("Перезапущен!");

    qDebug()<<gst_addr<<gst_port;

    //settings = new QSettings("gst_settings");
    settings->beginGroup("Connection");
    settings->setValue("gst_address", gst_addr);
    settings->setValue("gst_port", gst_port);
    settings->endGroup();
    qDebug() << settings->fileName();
    qDebug()<<"Saved";


    //qDebug()<<settings->value()

    settings_dialog->close();

    //QString strKey("Connection/");

}

void MainWindow::on_cancel_dialog_button_clicked(){
    settings_dialog->close();
}
