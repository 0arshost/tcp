#include "calfitness.h"
#include <QtDebug>

CalFitness::CalFitness(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<Swarms>("Swarms");
    qRegisterMetaType<Swarms>("Schema");
}

void CalFitness::calFitnessTcp(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix,
                               QVector<float> &tVector, QVector<bool> &max_cover, int start, int testObj)
{
    /*calApscSerial(cMatrix, sw_pt->particle, sw_pt->apsc, sw_pt->effnum, code_line, max_cover, num_testcase);
    calEfftimeSerial(sw_pt->effnum, sw_pt->particle, tVector, sw_pt->exetime, num_testcase);
    calParticleDistance(sw_pt->apsc, sw_pt->exetime, sw_pt->disapsc, sw_pt->disexet, sw_pt->distance,
                        sw_pt->avgapsc, sw_pt->avgexetime, sw_pt->avgdistance); */

    if(testObj==0)
    {
        //计算APSC
        for(int i=0+start; i<SWARMSIZE+start; i++)  //第i个个体
        {
            int sum_testnum = 0;
            int max_testnum = 0;

            for(int j=0; j<code_line; j++)  //第j行
            {
                if(max_cover[j]==0) continue;

                for(int k=0; k<num_testcase; k++)  //第k个测试用例
                {
                    bool temp = cMatrix[sw_pt->particle[i*num_testcase+k] * code_line + j];
                    if(temp==1)
                    {
                        k += 1;
                        sum_testnum += k;
                        if(k>max_testnum)
                            max_testnum = k;
                        break;
                    }
                }
            }

            float eff_code_line = 0.0;
            for(int j=0; j<code_line; j++)
            {
                if(max_cover[j]==1)
                    eff_code_line += 1;
                else
                    continue;
            }

            //qDebug() << "max_testnum: " << max_testnum << " ";
            sw_pt->effnum[i] = max_testnum;
            sw_pt->apsc[i] = 1 - ((float)sum_testnum / (float)(eff_code_line*num_testcase)) + 1.0 / (float)(2*num_testcase);
            //qDebug() << sw_pt->apsc[i] << " ";
        }

        //EET
        for(int i=0+start; i<SWARMSIZE+start; i++)
        {
            sw_pt->exetime[i] = 0.0;
        }
    }

    /*if(!testGoal[0] && testGoal[1])
    {
        //APSC
        for(int i=0+start; i<SWARMSIZE+start; i++)
        {
            int max_testnum = 0;

            for(int j=0; j<code_line; j++)  //第j行
            {
                if(max_cover[j]==0) continue;

                for(int k=0; k<num_testcase; k++)  //第k个测试用例
                {
                    bool temp = cMatrix[sw_pt->particle[i*num_testcase+k] * code_line + j];
                    if(temp==1)
                    {
                        k += 1;
                        if(k>max_testnum)
                            max_testnum = k;
                        break;
                    }
                }
            }


            sw_pt->effnum[i] = max_testnum;
            //sw_pt->apsc[i] = 1 - ((float)sum_testnum / (float)(eff_code_line*num_testcase)) + 1.0 / (float)(2*num_testcase);
            sw_pt->effnum[i] = 0.0;
        }

        //计算EET
        for(int i=0+start; i<SWARMSIZE+start; i++)
        {
            sw_pt->exetime[i] = 0;
            for(int j=0; j<sw_pt->effnum[i]; j++)
            {
                sw_pt->exetime[i] += tVector[sw_pt->particle[i*num_testcase+j]];
                //qDebug() << "exetime: " << sw_pt->exetime[i];
            }
        }
    } */

    if(testObj==1)
    {
        //计算APSC
        for(int i=0+start; i<SWARMSIZE+start; i++)  //第i个个体
        {
            int sum_testnum = 0;
            int max_testnum = 0;

            for(int j=0; j<code_line; j++)  //第j行
            {
                if(max_cover[j]==0) continue;

                for(int k=0; k<num_testcase; k++)  //第k个测试用例
                {
                    bool temp = cMatrix[sw_pt->particle[i*num_testcase+k] * code_line + j];
                    if(temp==1)
                    {
                        k += 1;
                        sum_testnum += k;
                        if(k>max_testnum)
                            max_testnum = k;
                        break;
                    }
                }
            }

            float eff_code_line = 0.0;
            for(int j=0; j<code_line; j++)
            {
                if(max_cover[j]==1)
                    eff_code_line += 1;
                else
                    continue;
            }

            //qDebug() << "max_testnum: " << max_testnum << " ";
            sw_pt->effnum[i] = max_testnum;
            sw_pt->apsc[i] = 1 - ((float)sum_testnum / (float)(eff_code_line*num_testcase)) + 1.0 / (float)(2*num_testcase);
            //qDebug() << sw_pt->apsc[i] << " ";
        }

        //计算EET
        for(int i=0+start; i<SWARMSIZE+start; i++)
        {
            sw_pt->exetime[i] = 0;
            for(int j=0; j<sw_pt->effnum[i]; j++)
            {
                sw_pt->exetime[i] += tVector[sw_pt->particle[i*num_testcase+j]];
                //qDebug() << "exetime: " << sw_pt->exetime[i];
            }
        }
    }

    //计算个体距离particle_distance
    sw_pt->avgapsc = 0.0;
    sw_pt->avgexetime = 0.0;
    sw_pt->avgdistance = 0.0;
    float amax = (float)MIN;
    float amin = (float)MAX;
    float tmax = (float)MIN;
    float tmin = (float)MAX;

    for(int i=0+start; i<SWARMSIZE+start; i++)
    {
        sw_pt->avgapsc += sw_pt->apsc[i];
        sw_pt->avgexetime += sw_pt->exetime[i];
        if(amax<sw_pt->apsc[i]) amax = sw_pt->apsc[i];
        if(amin>sw_pt->apsc[i]) amin = sw_pt->apsc[i];
        if(tmax<sw_pt->exetime[i]) tmax = sw_pt->exetime[i];
        if(tmin>sw_pt->exetime[i]) tmin = sw_pt->exetime[i];
    }

    sw_pt->avgapsc = sw_pt->avgapsc / SWARMSIZE;
    sw_pt->avgexetime = sw_pt->avgexetime / SWARMSIZE;
    //qDebug() << "sw_pt->avgexetime: " << sw_pt->avgexetime << endl;

    float ap = amax - amin;
    float ti = tmax - tmin;

    for(int i=0+start; i<SWARMSIZE+start; i++)
    {
        sw_pt->disapsc[i] = (sw_pt->apsc[i]-amin) / ap;
        sw_pt->disexet[i] = (sw_pt->exetime[i]-tmin) / ti;
        sw_pt->distance[i] = sw_pt->disapsc[i] + sw_pt->disexet[i];
        sw_pt->avgdistance += sw_pt->distance[i];
    }

    sw_pt->avgdistance = sw_pt->avgdistance / SWARMSIZE;
    //qDebug() << "sw_pt->avgdistance: " << sw_pt->avgdistance << endl;

    return;
}

/*
void CalFitness::calApscSerial(QVector<bool> &cMatrix, QVector<int> &particle, QVector<float> &apsc, QVector<int> &eff_num, int code_line, QVector<bool> max_cover, int num_testcase)
{
    for(int i=0; i<SWARMSIZE; i++)
    {
        int sum_testnum = 0;
        int max_testnum = 0;
        for(int j=0; j<code_line; j++)
        {
            if(max_cover[j]==0) continue;
            for(int k=0; k<num_testcase; k++)
            {
                bool temp = cMatrix[particle[i * num_testcase + k] * code_line + j];
                if(temp==1)
                {
                    k += 1;
                    sum_testnum += k;
                    if (k > max_testnum) max_testnum = k;
                    break;
                }
            }
        }

        float eff_code_line = 0.0;
        for(int j=0; j<code_line; j++)
        {
            if(max_cover[j]==1)
                eff_code_line += 1;
            else
                continue;
        }
        eff_num[i] = max_testnum;
        apsc[i] = 1 - ((float)sum_testnum / (float)(eff_code_line * num_testcase))
                + 1.0f / (float)(2 * num_testcase);

        //qDebug() << "apsc: " << apsc[i] << " ";
    }
}

void CalFitness::calEfftimeSerial(QVector<int> &eff_num, QVector<int> &particle, QVector<float> &tVector, QVector<float> &exetime, int num_testcase)
{
    for (int i=0; i<SWARMSIZE; i++)
    {
        exetime[i] = 0;
        for (int j=0; j<eff_num[i]; j++)
        {
            exetime[i] += tVector[particle[i * num_testcase + j]];
            //qDebug() << "exetime: " << exetime[i] << " ";
        }
    }
}

void CalFitness::calParticleDistance(QVector<float> &apsc, QVector<float> &exetime, QVector<float> &disapsc, QVector<float> &disexet, QVector<float> &distance, float &avgapsc, float &avgexetime, float &avgdistance)
{
    avgapsc = 0.0;
    avgexetime = 0.0;
    avgdistance = 0.0;

    float amax = (float)MIN;
    float amin = (float)MAX;
    float tmax = (float)MIN;
    float tmin = (float)MAX;

    for(int i=0; i<SWARMSIZE; i++)
    {
        avgapsc += apsc[i];
        avgexetime += exetime[i];
        if (amax < apsc[i]) amax = apsc[i];
        if (amin > apsc[i]) amin = apsc[i];
        if (tmax < exetime[i]) tmax = exetime[i];
        if (tmin > exetime[i]) tmin = exetime[i];
    }
    avgapsc = avgapsc / SWARMSIZE;
    avgexetime = avgexetime / SWARMSIZE;

    //qDebug() << "avgexetime: " << avgexetime << endl;

    float ap = amax - amin;
    float ti = tmax - tmin;
    for(int i=0; i<SWARMSIZE; i++)
    {
        disapsc[i] = (apsc[i] - amin) / ap;
        disexet[i] = (exetime[i] - tmin) / ti;
        distance[i] = disapsc[i] + disexet[i];
        avgdistance += distance[i];
    }

    avgdistance = avgdistance / SWARMSIZE;

    //qDebug() << "avgdistance: " << avgdistance << endl;
} */
