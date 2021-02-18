#include "taea.h"
#include <QTime>
#include <QDebug>

Taea::Taea(QObject *parent) : QObject(parent)
{

}

int Taea::taea(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector,
               QVector<bool> &max_cover, int front_0, int &change_cro, int start1, int testObj)
{
    QVector<int> temp_ind(SWARMSIZE * 2);
    QVector<bool> temp_pos(SWARMSIZE * num_testcase);
    //qDebug() << "Taea's crossover is " << change_cro << endl;

    switch (change_cro)
    {
        case 0:
            psoSelection(sw_pt, temp_ind, temp_pos, front_0, num_testcase);
            nsgaCrossoverSerial(sw_pt, temp_ind, num_testcase); break;
        case 1:
            psoSelection(sw_pt, temp_ind, temp_pos, front_0, num_testcase);
            nsgaCrossoverSinglepoint(sw_pt, temp_ind, num_testcase); break;
        case 2:
            psoSelection(sw_pt, temp_ind, temp_pos, front_0, num_testcase);
            nsgaCrossoverAntisinglepoint(sw_pt, temp_ind, num_testcase); break;
        case 3:
            psoSelection(sw_pt, temp_ind, temp_pos, front_0, num_testcase);
            nsgaCrossoverScan(sw_pt, temp_ind, temp_pos, num_testcase); break;
        case 4:
            psoSelection(sw_pt, temp_ind, temp_pos, front_0, num_testcase);
            nsgaCrossoverAntiscan(sw_pt, temp_ind, temp_pos, num_testcase); break;
        case 5:
            psoSelection(sw_pt, temp_ind, temp_pos, front_0, num_testcase);
            nsgaCrossoverUniform(sw_pt, temp_ind, temp_pos, num_testcase); break;
        default:
            nsgaCopy(sw_pt, num_testcase);
    }

    QVector<int>().swap(temp_ind);
    QVector<bool>().swap(temp_pos);

    nsgaMutationRandom(sw_pt, num_testcase);
    calfitTaea.calFitnessTcp(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, start1, testObj);

    int eliter = eliteTaea.eliterSelectionTaea(sw_pt, num_testcase);
    return eliter;
}

void Taea::psoSelection(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> temp_pos, int front_0, int num_testcase)
{
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);

    if(front_0==0)
    {
        for(int i=0; i<SWARMSIZE * 2; i++)
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
    }
    else
    {
        for(int i=0; i<SWARMSIZE; i++)
        {
            int rand_num = rand() % front_0;
            temp_ind[i*2] = rand_num;
            temp_ind[i*2+1] = i;
        }
    }

    int rand_num = (int)((float)num_testcase * CRORATE);
    QTime time1;
    time1 = QTime::currentTime();
    qsrand(time1.msec()+time1.second()*1000);
    for(int i=0; i<SWARMSIZE; i++)
    {
        for(int j=0; j<num_testcase; j++)
        {
            temp_pos[i*num_testcase+j] = 0;
        }
        for(int j=0; j<rand_num; j++)
        {
            int pos = qrand() % num_testcase;
            temp_pos[i * num_testcase + pos] = 1;
        }
    }
    return;
}

void Taea::nsgaCrossoverSerial(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
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

void Taea::nsgaCrossoverSinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
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

void Taea::nsgaCrossoverAntisinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
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

void Taea::nsgaCrossoverScan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
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

void Taea::nsgaCrossoverAntiscan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
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

void Taea::nsgaCrossoverUniform(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
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

void Taea::nsgaCopy(Swarms *sw_pt, int num_testcase)
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

void Taea::nsgaMutationRandom(Swarms *sw_pt, int num_testcase)
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

int Taea::domainComp(Swarms *sw_pt, int a, int b)
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

