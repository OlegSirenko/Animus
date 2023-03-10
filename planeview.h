#ifndef PLANEVIEW_H
#define PLANEVIEW_H

#include <QWidget>

namespace Ui {
class PlaneView;
}

class PlaneView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaneView(QWidget *parent = nullptr);
    ~PlaneView();
    int heading;

public slots:
    void joystic_touched(int x, int y, int zoom);
    void change_UI();

private slots:
    void on_pushButton_clicked();

private:
    Ui::PlaneView *ui;
    //int heading;
    int pitch = 0;
    int zoom= 0;
};

#endif // PLANEVIEW_H
