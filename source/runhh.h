#ifndef RUNHH_H
#define RUNHH_H

#include <QObject>
#include <QMetaType>
#include <QTimer>
#include "confighh.h"
#include "nsga.h"
#include "taea.h"
#include "spea.h"
#include "calfitness.h"
#include "hyper.h"
/*
#define ALG -1
#define TYPE -1
#define CRO_NUM 6    // "order: 0" "sing: 1" "antsing: 2" "scan: 3" "antscan: 4" "uniform: 5"
#define ALG_NUM 3    // nsga:0  taea:1  spea:2
#define STR (CRO_NUM * ALG_NUM)
#define SWARMSIZE 200
#define GENERATION 10000
#define MIN 1e-6
#define MAX 1000000
#define MUTRATE 0.001
#define CRORATE 0.1


struct Swarms{
    QVector<int> particle;
    QVector<float> apsc;
    QVector<float> exetime;
    QVector<int> effnum;
    QVector<float> disapsc;
    QVector<float> disexet;
    QVector<float> distance;
    QVector<bool> efficient;
    QVector<bool> newgen;
    QVector<int> batch;
    float avgapsc;
    float avgexetime;
    float avgdistance;
    int range;
};

struct Schema{
    QVector<int> matrix_map;
    QVector<int> schema;
    QVector<float> sch_value;
    QVector<int> sch_table;
    QVector<bool> sch_chose;
    int map_size;
    int sch_size;
};

Q_DECLARE_METATYPE(Swarms);
Q_DECLARE_METATYPE(Schema); */

class RunHH : public QObject
{
    Q_OBJECT
public:
    explicit RunHH(QObject *parent = nullptr);

signals:
    void gotRun();
    void sendMessage(QString msg);

public slots:
    void runFrame(QString subName, QVector<bool> &alg_sel, QVector<bool> &cro_sel, QString evalu, QString selec, int testObj);

private:
    Nsga nsga;
    Taea taea;
    Spea spea;
    CalFitness calfitn;
    Hyper selecter;
    QTimer *clock;
    QString fileCover;
    QString fileEet;
    void checkFile(int &num_testcase, int &code_line, QString subName);
    void readFile(QVector<bool> &cMatrix, QVector<float> &tVector, QVector<bool> &max_cover, int code_line, QString subName);
    void iniIndividuals(Swarms *sw_pt, QVector<float> &learn_val, QVector<int> &learn_time, int num_testcase);
    void iniSchema(Schema *sch_pt, int num_testcase);
    //void calFitness(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector, QVector<bool> &max_cover);
    void iniIndividualsPure(Swarms *sw, Swarms *sw_old, int num_testcase);
};

#endif // RUNHH_H
