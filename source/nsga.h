#ifndef NSGA_H
#define NSGA_H

#include <QObject>
#include "confighh.h"
#include "calfitness.h"
#include "eliterselec.h"

class Nsga : public QObject
{
    Q_OBJECT
public:
    explicit Nsga(QObject *parent = nullptr);
    int nsgaII(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix,
               QVector<float> &tVector, QVector<bool> &max_cover, int g, int &change_cro, int start1, int testObj);

signals:

private:
    void nsgaSelectionRand(Swarms *sw_pt, QVector<int> &temp_ind);
    void nsgaSelectionRandTwodim(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverSerial(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverSinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverAntisinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverScan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverAntiscan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverUniform(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCopy(Swarms *sw_pt, int num_testcase);
    void nsgaMutationRandom(Swarms *sw_pt, int num_testcase);
    int domainComp(Swarms *sw_pt, int a, int b);
    //void calFitness(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector, QVector<bool> &max_cover);
    CalFitness calfit;
    EliterSelec elite;
};

#endif // NSGA_H
