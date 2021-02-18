#include "spea.h"
#include <QTime>
#include <QVector>

Spea::Spea(QObject *parent) : QObject(parent)
{

}

int Spea::spea(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector,
               QVector<bool> &max_cover, int &change_cro, int start1, int testObj)
{
    QVector<int> temp_ind(SWARMSIZE * 2);
    QVector<bool> temp_pos(SWARMSIZE * num_testcase);

    switch (change_cro)
    {
        case 0:
            speaSelection(temp_ind, temp_pos, num_testcase);
            nsgaCrossoverSerial(sw_pt, temp_ind, num_testcase); break;
        case 1:
            speaSelection(temp_ind, temp_pos, num_testcase);
            nsgaCrossoverSinglepoint(sw_pt, temp_ind, num_testcase); break;
        case 2:
            speaSelection(temp_ind, temp_pos, num_testcase);
            nsgaCrossoverAntisinglepoint(sw_pt, temp_ind, num_testcase); break;
        case 3:
            speaSelection(temp_ind, temp_pos, num_testcase);
            nsgaCrossoverScan(sw_pt, temp_ind, temp_pos, num_testcase); break;
        case 4:
            speaSelection(temp_ind, temp_pos, num_testcase);
            nsgaCrossoverAntiscan(sw_pt, temp_ind, temp_pos, num_testcase); break;
        case 5:
            speaSelection(temp_ind, temp_pos, num_testcase);
            nsgaCrossoverUniform(sw_pt, temp_ind, temp_pos, num_testcase); break;
        default:
            nsgaCopy(sw_pt, num_testcase);
    }

    QVector<int>().swap(temp_ind);
    QVector<bool>().swap(temp_pos);

    nsgaMutationRandom(sw_pt, num_testcase);
    calfitSpea.calFitnessTcp(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, start1, testObj);

    int eliter = eliteSpea.eliterSelectionSpea(sw_pt, num_testcase);
    return eliter;
}

void Spea::speaSelection(QVector<int> &temp_ind, QVector<bool> temp_pos, int num_testcase)
{
    temp_ind[0] = 0;
    temp_ind[SWARMSIZE*2-1] = 0;

    for (int i = 1; i < SWARMSIZE; i++)
    {
        temp_ind[i * 2] = i;
        temp_ind[i * 2 - 1] = i;
    }

    int rand_num = (int)((float)num_testcase * CRORATE);
    QTime time;
    time = QTime::currentTime();
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

void Spea::nsgaCrossoverSerial(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
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

    //销毁内存
    QVector<bool>().swap(mark);
    QVector<int>().swap(cross_num);
    return;
}

void Spea::nsgaCrossoverSinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
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

void Spea::nsgaCrossoverAntisinglepoint(Swarms *sw_pt, QVector<int> &temp_ind, int num_testcase)
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

void Spea::nsgaCrossoverScan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
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

void Spea::nsgaCrossoverAntiscan(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
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

void Spea::nsgaCrossoverUniform(Swarms *sw_pt, QVector<int> &temp_ind, QVector<bool> &temp_pos, int num_testcase)
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

void Spea::nsgaCopy(Swarms *sw_pt, int num_testcase)
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

void Spea::nsgaMutationRandom(Swarms *sw_pt, int num_testcase)
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
