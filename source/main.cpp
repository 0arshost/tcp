#include "widget.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("面向多目标测试用例优先排序问题的超启发框架软件  v1.0");
    w.setFixedHeight(606);
    w.setFixedWidth(1090);
    w.show();
    return a.exec();
}
