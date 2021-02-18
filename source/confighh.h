#ifndef CONFIGHH_H
#define CONFIGHH_H

#include <QObject>
#include <QVector>
#define ALG -1
#define TYPE -1
#define ALG_NUM 3    // nsga:0  taea:1  spea:2
#define CRO_NUM 6    // "order: 0" "sing: 1" "antsing: 2" "scan: 3" "antscan: 4" "uniform: 5"
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
Q_DECLARE_METATYPE(Schema);

class Confighh : public QObject
{
    Q_OBJECT
public:
    explicit Confighh(QObject *parent = nullptr);

signals:

};

#endif // CONFIGHH_H
