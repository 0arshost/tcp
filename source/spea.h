#ifndef SPEA_H
#define SPEA_H

#include <QObject>
#include "confighh.h"
#include "calfitness.h"
#include "eliterselec.h"

class Spea : public QObject
{
    Q_OBJECT
public:
    explicit Spea(QObject *parent = nullptr);
    int spea(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix,
             QVector<float> &tVector, QVector<bool> &max_cover, int &change_cro, int start, int testObj);

signals:

private:
    void speaSelection(QVector<int> &temp_ind, QVector<bool> temp_pos, int num_testcase);
    void nsgaCrossoverSerial(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverSinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverAntisinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase);
    void nsgaCrossoverScan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverAntiscan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCrossoverUniform(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase);
    void nsgaCopy(Swarms *sw_pt, int num_testcase);
    void nsgaMutationRandom(Swarms *sw_pt, int num_testcase);
    CalFitness calfitSpea;
    EliterSelec eliteSpea;
};

#endif // SPEA_H
