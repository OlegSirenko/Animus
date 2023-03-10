#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <QThread>
#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include <gst/app/gstappsink.h>

#include <QVideoFrame>


class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = nullptr);
    ~worker();
    volatile bool running, stopped;

signals:
    void SignalToObj_mainThreadGUI();
    void state_changed(QString str);
    void onNewFrame(QImage);
    //QString signal_state_changed();

public slots:
    void StopWork();
    void StartWork();
    void address_port_changed(QString, QString);

private slots:
    void do_Work();

    bool init_gst();
    bool create_pipeline(QString ip, QString port);

private:
    int height;
    int width;
    QString address = "";
    QString port = "";

    QString default_address = "127.0.0.1";
    QString default_port = "7010";

    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;
    void *mAppSink = nullptr;
};

#endif // WORKER_H
