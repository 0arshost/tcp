#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTime>
#include <QtGlobal>
#include<runhh.h>
#include<QStandardPaths>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //获取文件
//    QFile file("D:\\WangRui\\Subject\\new_total_cover\\commons-cli.time");
//    if(file.open(QIODevice::ReadOnly))
//    {
//        /*QString content;
//        while(!file.atEnd())
//        {
//               content += file.readLine();
//        }*/
//        QString content = file.readAll();
//        QMessageBox::information(this, "", content);
//        qDebug() << content << endl;

//        file.close();
//    }

    //获取输入框的内容
//    QString subName = ui->lineEdit->text();
//    qDebug() << "Widget UI is " << subName << endl;

    ui->proce->setVisible(false);
    ui->textEdit->clear();
    ui->nsga->setChecked(1);
    ui->order->setChecked(1);
    ui->hd->setChecked(1);
    ui->greedy->setChecked(1);
    //连接信号和槽
    connect(&runner, SIGNAL(gotRun()), this, SLOT(onGotRun()));
    connect(&runner,SIGNAL(sendMessage(QString)),this,SLOT(recieveMessage(QString)));

}

Widget::~Widget()
{
    delete ui;
}
void Widget::recieveMessage(QString msg) {
    ui->textEdit->append(msg);
    ui->textEdit->append(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    //example 2
    QFile file(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/res.txt");

    //已读写方式打开文件，
    //如果文件不存在会自动创建文件
    if(!file.open(QIODevice::ReadWrite)){
        qDebug()<<"打开失败";
    }else{
        qDebug()<<"打开成功";
    }

    //写入文件
    qint64 length = -1;
    length = file.write(msg.toUtf8(),msg.length());

    if(length == -1){
        qDebug()<<"写入文件失败";
    }else{
        qDebug()<<"写入文件成功";
    }

    //关闭文件
    file.close();
}

void Widget::on_btnRun_clicked()
{
    //ui->proce->setVisible(true);
    //ui->proce->setText("The framework is running...");
    //ui->proce->adjustSize();
    ui->proce->setVisible(true);
    ui->proce->setText("正在计算，请稍后");
    QString subName = ui->lineEdit->text();
    if (subName.isEmpty()) {
        ui->proce->setText("请输入被测对象");
        return;
    }
    //qDebug() << "Widget UI is " << subName.toUtf8().data() << endl;

    //测试目标
//    QVector<bool> testGoal(2);
//    bool apsc = ui->apsc->isChecked();
//    bool eet = ui->eet->isChecked();
//    testGoal[0] = apsc;
//    testGoal[1] = eet;

    int testObj= ui->testObj->currentIndex();

    //底层
    QVector<bool> alg_sel(3);
    QVector<bool> cro_sel(6);

    bool nsga_sel = ui->nsga->isChecked();
    bool taea_sel = ui->taea->isChecked();
    bool spea_sel = ui->spea->isChecked();
    alg_sel[0] = nsga_sel;
    alg_sel[1] = taea_sel;
    alg_sel[2] = spea_sel;

    bool order_sel = ui->order->isChecked();
    bool sing_sel = ui->sing->isChecked();
    bool antising_sel = ui->antising->isChecked();
    bool scan_sel = ui->scan->isChecked();
    bool antiscan_sel = ui->antiscan->isChecked();
    bool uniform_sel = ui->uniform->isChecked();
    cro_sel[0] = order_sel;
    cro_sel[1] = sing_sel;
    cro_sel[2] = antising_sel;
    cro_sel[3] = scan_sel;
    cro_sel[4] = antiscan_sel;
    cro_sel[5] = uniform_sel;

//    for(int i=0; i<3; i++)
//        qDebug() << "Alg is " << alg_sel[i] << endl;
//    for(int j=0; j<6; j++)
//        qDebug() << "Cro is " << cro_sel[j] << " ";

    //上层
    QButtonGroup *evalu = new QButtonGroup(this);
    evalu->addButton(ui->hd);
    evalu->addButton(ui->avghd);
    evalu->addButton(ui->hdmin);
    evalu->addButton(ui->avghdstep);
    //ui->hd->setChecked(true);

    QButtonGroup *selec = new QButtonGroup(this);
    selec->addButton(ui->greedy);
    selec->addButton(ui->roulette);
    selec->addButton(ui->eG);
    selec->addButton(ui->eRG);
    //ui->greedy->setChecked(true);

    QRadioButton *evalu_checked = qobject_cast<QRadioButton*>(evalu->checkedButton());
    QString evaluCheck = evalu_checked->objectName();
    QRadioButton *selec_checked = qobject_cast<QRadioButton*>(selec->checkedButton());
    QString selecCheck = selec_checked->objectName();

    //qDebug() << "The selection strategy is " << evaluCheck << selecCheck << endl;

    runner.runFrame(subName, alg_sel, cro_sel, evaluCheck, selecCheck, testObj);

}

void Widget::onGotRun()
{
    qDebug() << "The running has been confirmed." << endl;
    ui->proce->setText("计算完成,请到" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "下查看");
    ui->proce->adjustSize();
}
