#include "nsga.h"
#include <QDebug>
#include <QTime>

Nsga::Nsga(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<Swarms>("Swarms");
    qRegisterMetaType<Swarms>("Schema");
}

int Nsga::nsgaII(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector,
                 QVector<bool> &max_cover, int g, int &change_cro, int start1, int testObj)
{
    if(g<STR)
    {
        change_cro = g;
        //qDebug() << "NSGAII's corssover is " << change_cro << endl;
    }

    QVector<int> temp_ind(SWARMSIZE * 2);
    QVector<bool> temp_pos(SWARMSIZE * num_testcase);

    switch (change_cro)
    {
        case 0:
            nsgaSelectionRand(sw_pt, temp_ind);
            nsgaCrossoverSerial(sw_pt, temp_ind, num_testcase); break;
        case 1:
            nsgaSelectionRand(sw_pt, temp_ind);
            nsgaCrossoverSinglepoint(sw_pt, temp_ind, num_testcase); break;
        case 2:
            nsgaSelectionRand(sw_pt, temp_ind);
            nsgaCrossoverAntisinglepoint(sw_pt, temp_ind, num_testcase); break;
        case 3:
            nsgaSelectionRandTwodim(sw_pt, temp_ind, temp_pos, num_testcase);
            nsgaCrossoverScan(sw_pt, temp_ind, temp_pos, num_testcase); break;
        case 4:
            nsgaSelectionRandTwodim(sw_pt, temp_ind, temp_pos, num_testcase);
            nsgaCrossoverAntiscan(sw_pt, temp_ind, temp_pos, num_testcase); break;
        case 5:
            nsgaSelectionRandTwodim(sw_pt, temp_ind, temp_pos, num_testcase);
            nsgaCrossoverUniform(sw_pt, temp_ind, temp_pos, num_testcase); break;
        default:
            nsgaCopy(sw_pt, num_testcase);
    }

    QVector<int>().swap(temp_ind);
    QVector<bool>().swap(temp_pos);

    nsgaMutationRandom(sw_pt, num_testcase);
    //calFitness(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover);
    calfit.calFitnessTcp(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, start1, testObj);

    int range = elite.eliterSelectionNsga(sw_pt, num_testcase);
    return range;
}

void Nsga::nsgaSelectionRand(Swarms *sw_pt, QVector<int> &temp_ind)
{
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<2*SWARMSIZE; i++)
    {
        int rand_num1 = qrand() % SWARMSIZE;
        int rand_num2 = qrand() % SWARMSIZE;

        int domain_judge = 0;
        int domain_comp = domainComp(sw_pt, rand_num1, rand_num2);

        if(domain_comp<0)
        {
            domain_judge = sw_pt->distance[rand_num1]>sw_pt->distance[rand_num2] ? rand_num1 : rand_num2;
        }

        temp_ind[i] = domain_judge;

        //qDebug() << "The temp_ind is " << temp_ind[i] << ". ";
    }
    return;
}

void Nsga::nsgaSelectionRandTwodim(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
{
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        int rand_num1 = qrand() % SWARMSIZE;
        int rand_num2 = qrand() % SWARMSIZE;

        int domain_judge = 0;

        int domain_comp = domainComp(sw_pt, rand_num1, rand_num2);

        if(domain_comp<0)
        {
            domain_judge = sw_pt->distance[rand_num1]>sw_pt->distance[rand_num2] ? rand_num1 : rand_num2;
        }

        temp_ind[i] = domain_judge;
    }

    int rand_num = (int)((float)num_testcase * CRORATE);
    QTime time1;
    time1 = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<SWARMSIZE; i++)
    {
        for(int j=0; j<num_testcase; j++)
        {
            temp_pos[i * num_testcase + j] = 0;
        }
        for(int j=0; j<rand_num; j++)
        {
            int pos = qrand() % num_testcase;
            temp_pos[i * num_testcase + pos] = 1;
        }
    }
    return;
}

void Nsga::nsgaCrossoverSerial(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
{
    QVector<int> cross_num;
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        int temp = qrand() % num_testcase;
        cross_num.append(temp);
    }

    QVector<bool> mark;
    int POS = SWARMSIZE * num_testcase;
    for(int loop=0; loop<SWARMSIZE; loop++)
    {
        int cro_p1 = cross_num[loop*2];
        int cro_p2 = cross_num[loop*2+1];
        if(cro_p1>cro_p2)
        {
            int t = cro_p2;
            cro_p2 = cro_p1;
            cro_p1 = t;
        }

        for(int i=0; i<num_testcase; i++)
            mark.append(0);

        for(int i=cro_p1; i<cro_p2; i++)
        {
            int temp = sw_pt->particle[temp_ind[2*loop] * num_testcase + i];
            sw_pt->particle[POS + loop*num_testcase + i] = temp;
            mark[temp] = 1;
        }

        int start_pos = 0;
        for(int i = 0; i<cro_p1; i++)
        {
            for(int j = start_pos; j<num_testcase; j++)
            {
                int temp = sw_pt->particle[temp_ind[2 * loop + 1] * num_testcase + j];
                if(mark[temp]==0)
                {
                    sw_pt->particle[POS + loop * num_testcase + i] = temp;
                    start_pos = j + 1;
                    break;
                }
                else
                    continue;
            }
        }

        for(int i = cro_p2; i < num_testcase; i++)
        {
            for(int j = start_pos; j < num_testcase; j++)
            {
                 int temp = sw_pt->particle[temp_ind[2 * loop + 1] * num_testcase + j];
                 if(mark[temp]==0)
                 {
                    sw_pt->particle[POS + loop * num_testcase + i] = temp;
                    start_pos = j + 1;
                    break;
                 }
                 else
                     continue;
            }
        }
    }

//    mark.clear();
//    cross_num.clear();
    //销毁内存
    QVector<bool>().swap(mark);
    QVector<int>().swap(cross_num);
    return;
}

void Nsga::nsgaCrossoverSinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
{
    QVector<int> cross_num;
    int min_pos = sw_pt->effnum[0];
    for(int i=1; i<SWARMSIZE; i++)
    {
        if(sw_pt->effnum[i]<min_pos)
        {
            min_pos = sw_pt->effnum[i];
        }
    }

    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<SWARMSIZE; i++)
    {
        int temp = qrand() % num_testcase;
        cross_num.append(temp);
    }

    QVector<bool> mark;
    int POS = SWARMSIZE * num_testcase;
    for(int i=0; i<SWARMSIZE; i++)
    {
        for(int j=0; j<num_testcase; j++)
            mark.append(0);

        for(int j=0; j<cross_num[i]; j++)
        {
            int temp = sw_pt->particle[temp_ind[2 * i] * num_testcase + j];
            sw_pt->particle[POS + i * num_testcase + j] = temp;
            mark[temp] += 1;
        }

        int start_point = 0;
        for(int j=cross_num[i]; j<num_testcase; j++)
        {
            for(int k=start_point; k<num_testcase; k++)
            {
                int temp = sw_pt->particle[temp_ind[2 * i + 1] * num_testcase + k];
                if(mark[temp]==0)
                {
                    sw_pt->particle[POS + i * num_testcase + j] = temp;
                    start_point = k + 1;
                    break;
                }
                else
                    continue;
            }
        }
    }

    //销毁内存
    QVector<bool>().swap(mark);
    QVector<int>().swap(cross_num);
    return;
}

void Nsga::nsgaCrossoverAntisinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
{
    QVector<int> cross_num;
    int min_pos = sw_pt->effnum[0];
    for(int i=1; i<SWARMSIZE; i++)
    {
        if(sw_pt->effnum[i]<min_pos)
        {
            min_pos = sw_pt->effnum[i];
        }
    }

    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<SWARMSIZE; i++)
    {
        int temp = qrand() % num_testcase;
        cross_num.append(temp);
    }

    QVector<bool> mark;
    int POS = SWARMSIZE * num_testcase;
    for(int i=0; i<SWARMSIZE; i++)
    {
        for(int j=0; j<num_testcase; j++)
            mark.append(0);

        for(int j=cross_num[i]; j<num_testcase; j++)
        {
            int temp = sw_pt->particle[temp_ind[2 * i] * num_testcase + j];
            sw_pt->particle[POS + i * num_testcase + j] = temp;
            mark[temp] += 1;
        }

        int start_point = 0;
        for(int j=0; j<cross_num[i]; j++)
        {
            for(int k=start_point; k<num_testcase; k++)
            {
               int temp = sw_pt->particle[temp_ind[2 * i + 1] * num_testcase + k];
               if(mark[temp]==0)
               {
                   sw_pt->particle[POS + i * num_testcase + j] = temp;
                   start_point = k + 1;
                   break;
               }
               else
                   continue;
            }
        }
    }
    //销毁内存
    QVector<bool>().swap(mark);
    QVector<int>().swap(cross_num);
    return;
}

void Nsga::nsgaCrossoverScan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
{
    QVector<int> cross_num(num_testcase+1, 0);
    QVector<int> t_num;
    for(int loop=0; loop<SWARMSIZE; loop++)
    {
        cross_num[0] = 0;
        cross_num[num_testcase] = 0;
        for(int i=1; i<num_testcase+1; i++)
        {
            cross_num[i] = cross_num[i - 1] + (int)temp_pos[loop * num_testcase + i - 1];
        }
        for(int i=0; i<num_testcase; i++)
        {
            int temp = cross_num[num_testcase] + i - cross_num[i];
            t_num.append(temp);
        }
        for(int i=0; i<num_testcase; i++)
        {
            if(temp_pos[loop * num_testcase + i] == 1)
            {
                t_num[i] = cross_num[i];
            }
        }
        int POS = SWARMSIZE * num_testcase;
        for(int i=0; i<num_testcase; i++)
        {
            sw_pt->particle[POS + loop * num_testcase + t_num[i]] =
                            sw_pt->particle[temp_ind[loop] * num_testcase + i];
        }
    }

    QVector<int>().swap(cross_num);
    QVector<int>().swap(t_num);
    return;
}

void Nsga::nsgaCrossoverAntiscan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
{
    QVector<int> cross_num(num_testcase+1, 0);
    QVector<int> t_num;
    for(int loop=0; loop<SWARMSIZE; loop++)
    {
        cross_num[0] = 0;
        cross_num[num_testcase] = 0;
        for(int i=1; i<num_testcase+1; i++)
        {
            cross_num[i] = cross_num[i - 1] + (int)temp_pos[loop * num_testcase + i - 1];
        }
        for(int i=0; i<num_testcase; i++)
        {
            int temp = i - cross_num[i];
            t_num.append(temp);
        }
        for(int i=0; i<num_testcase; i++)
        {
            if(temp_pos[loop * num_testcase + i] == 1)
            {
                t_num[i] = num_testcase - 1 - cross_num[i];
            }
        }

        int POS = SWARMSIZE * num_testcase;
        for(int i=0; i<num_testcase; i++)
        {
            sw_pt->particle[POS + loop * num_testcase + t_num[i]] =
            sw_pt->particle[temp_ind[loop] * num_testcase + i];
        }
    }

    QVector<int>().swap(cross_num);
    QVector<int>().swap(t_num);
    return;
}

void Nsga::nsgaCrossoverUniform(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
{
    QVector<bool> mark;
    int POS = SWARMSIZE * num_testcase;
    for(int loop=0; loop<SWARMSIZE; loop++)
    {
        for(int i=0; i<num_testcase; i++)
            mark.append(0);

        for(int i=0; i<num_testcase; i++)
        {
            if(temp_pos[loop * num_testcase + i] == 0)
                continue;
            else
            {
                int temp = sw_pt->particle[temp_ind[2 * loop] * num_testcase + i];
                sw_pt->particle[POS + loop * num_testcase + i] = temp;
                mark[temp] = 1;
            }
        }

        int start_pos = 0;
        for(int i=0; i<num_testcase; i++)
        {
            for(int j=start_pos; j<num_testcase; j++)
            {
                if(temp_pos[loop * num_testcase + i] == 1)
                    continue;
                else
                {
                    int temp = sw_pt->particle[temp_ind[2 * loop + 1] * num_testcase + j];
                    if(mark[temp]==0)
                    {
                        sw_pt->particle[POS + loop * num_testcase + i] = temp;
                        start_pos = j + 1;
                        break;
                    }
                }
            }
        }
    }

    QVector<bool>().swap(mark);
    return;
}

void Nsga::nsgaCopy(Swarms *sw_pt, int num_testcase)
{
    int start = SWARMSIZE * num_testcase;
    for(int i = 0; i < SWARMSIZE; i++)
    {
        for (int j = 0; j < num_testcase; j++)
        {
            int temp = sw_pt->particle[i * num_testcase + j];
            sw_pt->particle[start + i * num_testcase + j] = temp;
        }
    }
}

void Nsga::nsgaMutationRandom(Swarms *sw_pt, int num_testcase)
{
    float MUT = MUTRATE;
    int num_rand = (int)(num_testcase * MUT * 2) + 2;
    QVector<int> array_rand(num_testcase);
    int old_part = SWARMSIZE * num_testcase;

    for(int i=0; i<SWARMSIZE; i++)
    {
        QTime time;
        time = QTime::currentTime();
        qsrand(time.msec()+time.second()*1000);
        for(int j=0; j<num_testcase; j++)
        {
            int temp = qrand() % num_testcase;
            array_rand[j] = temp;
        }
        for(int j=0; j<num_rand; j+=2)
        {
            int pos1 = old_part + i * num_testcase + array_rand[j];
            int pos2 = old_part + i * num_testcase + array_rand[j + 1];
            int temp_record = sw_pt->particle[pos1];
            sw_pt->particle[pos1] = sw_pt->particle[pos2];
            sw_pt->particle[pos2] = temp_record;
        }
    }

    QVector<int>().swap(array_rand);
}

int Nsga::domainComp(Swarms *sw_pt, int a, int b)
{
    int a_a = (int)(sw_pt->apsc[a] * 1000000);
    int b_a = (int)(sw_pt->apsc[b] * 1000000);
    int a_c = 1;
    int b_c = 1;
    int a_t = (int)(sw_pt->exetime[a] * 1000);
    int b_t = (int)(sw_pt->exetime[b] * 1000);

    if (a_a == b_a && a_t == b_t && a_c == b_c) return -2;

    QVector<int> equ(3, 0);
    if(a_a == b_a)
        equ[0] = 0;
    else if(a_a>b_a)
        equ[0] = 1;
    else
        equ[0] = -1;

    if(a_t==b_t)
        equ[1] = 0;
    else if(a_t<b_t)
        equ[1] = 1;
    else
        equ[1] = -1;

    if(a_c==b_c)
        equ[2] = 0;
    else if(a_c>b_c)
        equ[2] = 1;
    else
        equ[2] = -1;

    bool mark_zero = 0;
    int sum = 0;
    for(int i=0; i<3; i++)
    {
        if(equ[i]==0)
            mark_zero = 1;
        sum += equ[i];
    }

    if(mark_zero==1)
    {
        if(sum==0)
            return -1;
        else if(sum>0)
            return a;
        else
            return b;
    }
    else
    {
        if(sum==3)
            return a;
        else if(sum==-3)
            return b;
        else
            return -1;
    }
}

/*
void Nsga::calFitness(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector, QVector<bool> &max_cover)
{
    //计算APSC
    for(int i=0; i<SWARMSIZE; i++)  //第i个个体
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
    for(int i=0; i<SWARMSIZE; i++)
    {
        sw_pt->exetime[i] = 0;
        for(int j=0; j<sw_pt->effnum[i]; j++)
        {
            sw_pt->exetime[i] += tVector[sw_pt->particle[i*num_testcase+j]];
            //qDebug() << "exetime: " << sw_pt->exetime[i];
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

    for(int i=0; i<SWARMSIZE; i++)
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

    float ap = amax - amin;
    float ti = tmax - tmin;

    for(int i=0; i<SWARMSIZE; i++)
    {
        sw_pt->disapsc[i] = (sw_pt->apsc[i]-amin) / ap;
        sw_pt->disexet[i] = (sw_pt->exetime[i]-tmin) / ti;
        sw_pt->distance[i] = sw_pt->disapsc[i] + sw_pt->disexet[i];
        sw_pt->avgdistance += sw_pt->distance[i];
    }

    sw_pt->avgdistance = sw_pt->avgdistance / SWARMSIZE;
    qDebug() << "avgdistance: " << sw_pt->avgdistance << endl;

    return;
} */
