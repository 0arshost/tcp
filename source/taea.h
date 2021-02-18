#ifndef TAEA_H
#define TAEA_H

#include <QObject>
#include "confighh.h"
#include "calfitness.h"
#include "eliterselec.h"

class Taea : public QObject
{
    Q_OBJECT
public:
    explicit Taea(QObject *parent = nullptr);
    int taea(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix,
             QVector<float> &tVector, QVector<bool> &max_cover, int front_0, int &change_cro, int start1, int testObj);

signals:

private:
    void psoSelection(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> temp_pos, int front_0, int num_testcase);
    void nsgaCrossoverSerial(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverSinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverAntisinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverScan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverAntiscan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverUniform(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCopy(Swarms *sw_pt, int num_testcase);
    void nsgaMutationRandom(Swarms *sw_pt, int num_testcase);
    int domainComp(Swarms *sw_pt, int a, int b);
    CalFitness calfitTaea;
    EliterSelec eliteTaea;
};

#endif // TAEA_H
