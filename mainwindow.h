#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <worker.h>
#include <QDialog>
#include <QLineEdit>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void startWorkSignal();
    void stopWorkSignal();
    void joysticTouchedSignal(int x,int y, int z);
    void change_pipeline_addr_port(QString, QString);

private slots:
    // Joypad SLOTS
    void send_joypad_acc(float x, float y);
    void on_verticalSlider_valueChanged(int value);
    void send_static();
    // Socket (Plugin connection) SLOTS
    void server_New_Connect();
    void socket_Read_Data();
    void socket_Disconnected();
    // Gstreamer's Thread SLOTS
    void state_changed(QString);
    void drawFrame(QImage);
    // Main GUI SLOTS
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_settings_clicked();
    // Settings DIALOG SLOTS
    void on_submit_dialog_button_clicked();
    void on_cancel_dialog_button_clicked();




private:
    Ui::MainWindow *ui;
    QTcpServer* server;
    QTcpSocket* socket{};
    QTimer* timer;
    int heading = 180;
    int pitch = 180; // чтобы задать константное кол-во символов.
    int zoom = 1;
    QString sending;
    bool playing = false;
    worker *myWorker;
    QThread *WorkerThread;
    QPixmap viewImage();
    QLineEdit *address;
    QLineEdit *port;
    QDialog *settings_dialog;
    QSettings *settings;
    QString gst_addr = "127.0.0.1";
    QString gst_port = "7010";
};
#endif // MAINWINDOW_H
