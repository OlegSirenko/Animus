#include "worker.h"
#include "QDebug"
#include <QImage>
#include <QTimer>
#include <QCoreApplication>

worker::worker(QObject *parent)
    : QObject{parent},
      stopped(false),
      running(false)
{

}

worker::~worker(){
    gst_message_unref (msg);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}

void worker::StopWork()
{
    qDebug() << "inside StopWork";
    this->stopped = true;
    this->running = false;
    //gst_message_unref (msg);
    //gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}

void worker::do_Work()
{

    /* Initialize GStreamer */
    if(!init_gst()){
        emit state_changed("Не удалось инициализировать  Gstreamer");
        return;
    }
    emit state_changed("GStreamer инициализирован");

    /* Build the pipeline */
    if (address.isEmpty()){
        address = default_address;
        port = default_port;
        emit state_changed("Попытка подключения к " + address +":"+ port);
        emit state_changed("Для изменения адресса подключения измените настройки.");
    }
    if (!create_pipeline(address, port)){
        emit state_changed("Не удалось создать видеопоток");
    }
    emit state_changed("Видеопоток создан");


    /* Start playing */
    emit state_changed("Ожидайте воспроизведения");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    gst_object_ref (pipeline);
    mAppSink = gst_bin_get_by_name(GST_BIN(pipeline), QString("animus").toStdString().c_str());


    GstClockTime lTimeOutNanoSecond = 3000000000;

    while(running){
        //mAppSink = gst_bin_get_by_name(GST_BIN(pipeline), QString("animus").toStdString().c_str());
        GstSample *lSample = gst_app_sink_try_pull_sample(GST_APP_SINK(mAppSink), lTimeOutNanoSecond);
        if(lSample){

            GstCaps *caps = gst_sample_get_caps(lSample);
            GstStructure *structure = gst_caps_get_structure(caps, 0);
            width = g_value_get_int(gst_structure_get_value(structure, "width"));
            height = g_value_get_int(gst_structure_get_value(structure, "height"));

            GstBuffer *lBuf = gst_sample_get_buffer(lSample);
            if(lBuf){
                GstMapInfo lInfo = {};
                if (gst_buffer_map(lBuf, &lInfo, GST_MAP_READ)){
                    emit onNewFrame(QImage(lInfo.data, width, height, QImage::Format::Format_RGB888));
                }
                gst_buffer_unmap(lBuf, &lInfo);
            }
            gst_sample_unref(lSample);
        }
        else {
            QImage lImage(640, 480, QImage::Format::Format_RGB888);
            lImage.fill(Qt::TransparentMode);
            emit onNewFrame(lImage);
        }
    }
    /* Free resources */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    //QMetaObject::invokeMethod(this, "do_Work", Qt::QueuedConnection);
}

void worker::StartWork()
{
    qDebug() << "inside StartWork";
    stopped = false;
    running = true;

    //emit running();
    do_Work();
}

bool worker::init_gst(){
    gst_init (nullptr, nullptr);
    GError *err = nullptr;
    if(!gst_init_check(nullptr, nullptr, &err)){
        return false;
    }
    return true;
}

void worker::address_port_changed(QString new_address, QString new_port){
    address = new_address;
    port = new_port;
    qDebug()<<"Address and port Changed";
}

bool worker::create_pipeline(QString ip_address, QString port){
    GError *err = nullptr;
    QString pipeline_str = "tcpclientsrc host="+ip_address+" port="+port+" ! multipartdemux ! jpegdec ! appsink name=animus";
    pipeline =gst_parse_launch(pipeline_str.toStdString().c_str(), &err);
    if (err != nullptr){
        return false;
    }
    return true;

    /*
     *
     *
                "tcpclientsrc host=192.168.1.168 port=7010 ! multipartdemux ! jpegdec ! "
                "  videoscale ! glimagesink"
     */
}
