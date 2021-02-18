#ifndef CALFITNESS_H
#define CALFITNESS_H

#include <QObject>
#include "confighh.h"

class CalFitness : public QObject
{
    Q_OBJECT
public:
    explicit CalFitness(QObject *parent = nullptr);
    void calFitnessTcp(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix,
                       QVector<float> &tVector, QVector<bool> &max_cover, int start, int testObj);
signals:

/*
private:
    void calApscSerial(QVector<bool> &cMatrix, QVector<int> &particle, QVector<float> &apsc, QVector<int> &eff_num,
                       int code_line, QVector<bool> max_cover, int num_testcase);
    void calEfftimeSerial(QVector<int> &eff_num, QVector<int> &particle, QVector<float> &tVector,  QVector<float> &exetime, int num_testcase);
    void calParticleDistance(QVector<float> &apsc, QVector<float> &exetime, QVector<float> &disapsc, QVector<float> &disexet,
                             QVector<float> &distance, float &avgapsc, float &avgexetime, float &avgdistance);
*/
};

#endif // CALFITNESS_H
