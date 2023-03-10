#include "planeview.h"
#include <QDebug>
#include "ui_planeview.h"
#include "math.h"
#include <QPainter>

PlaneView::PlaneView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaneView)
{
    ui->setupUi(this);
    heading = 0;
}

PlaneView::~PlaneView()
{
    delete ui;
}



void PlaneView::joystic_touched(int sHeading, int sPitch, int sZoom){
    //
    heading = sHeading;
    pitch = sPitch;
    zoom = sZoom;
    //qDebug()<<(140 + (65 * -sin(sHeading*3.14/180)))<< (70 + (65 * cos(sHeading*3.14/180)));
    //ui->pushButton->move((140 + (65 * -sin(heading*3.14/180))), (70 + (65 * cos(heading*3.14/180))));
    qDebug()<<sHeading <<heading;

    //qDebug()<<"Emited"<<heading<<pitch<<zoom;
}



void PlaneView::change_UI(){

    //ui->view->move((140 + (65 * -sin(heading*3.14/180))), (70 + (65 * cos(heading*3.14/180))));
    //qDebug()<<(140 + (65 * -sin(heading*3.14/180)))<< (70 + (65 * cos(heading*3.14/180)));

}


void PlaneView::on_pushButton_clicked()
{
    //qDebug()<<(140 + (65 * -sin(heading*3.14/180)))<< (70 + (65 * cos(heading*3.14/180)));

    qDebug()<<"BUTTON PUSHED "<<heading;
    ui->pushButton->move((140 + (65 * -sin(heading*3.14/180))), (70 + (65 * cos(heading*3.14/180))));
}

