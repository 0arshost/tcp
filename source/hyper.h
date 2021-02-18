#ifndef HYPER_H
#define HYPER_H

#include <QObject>
#include "confighh.h"
#include "eliterselec.h"

class Hyper : public QObject
{
    Q_OBJECT
public:
    explicit Hyper(QObject *parent = nullptr);
    int learnStra(Swarms *sw_pt, Swarms *sw_cp, int change_alg, int change_cro,
                 QVector<float> &learn_val, QVector<int> &learn_time, int num_testcase,
                  QVector<int> &llh_sel, QVector<int> &delta, QString evalu, QString selec,
                  QVector<QVector<float>> &window);

signals:

private:
    EliterSelec eliteLearn;

};

#endif // HYPER_H
