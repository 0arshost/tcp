#ifndef ELITERSELEC_H
#define ELITERSELEC_H

#include <QObject>
#include "confighh.h"

class EliterSelec : public QObject
{
    Q_OBJECT
public:
    explicit EliterSelec(QObject *parent = nullptr);
    int eliterSelectionNsga(Swarms *sw_pt, int num_testcase);
    int eliterSelectionTaea(Swarms *sw_pt, int num_testcase);
    int eliterSelectionSpea(Swarms *sw_pt, int num_testcase);
signals:

private:
    void initTempSwarm(Swarms *sw_pt, int num_testcase);
    int domainComp(Swarms *sw_pt, int a, int b);
    void copySwarm(Swarms *sw_pt, Swarms * sw_new, int a, int b, int num_testcase);
};

#endif // ELITERSELEC_H
