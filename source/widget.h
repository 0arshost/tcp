#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "runhh.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();    
    Ui::Widget *ui;

private slots:
    void on_btnRun_clicked();
    void onGotRun();

public slots:
    void recieveMessage(QString msg);//槽函数

private:

    RunHH runner;
};
#endif // WIDGET_H
