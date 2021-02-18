#include "eliterselec.h"
#include <QDebug>

EliterSelec::EliterSelec(QObject *parent) : QObject(parent)
{

}

int EliterSelec::eliterSelectionNsga(Swarms *sw_pt, int num_testcase)
{
    int front_num = 0;
    int set_count = 0;

    Swarms sw_new;
    initTempSwarm(&(sw_new), num_testcase);

    for(int i=0; i<2*SWARMSIZE; i++)
    {
//        sw_pt->efficient.append(0);
//        sw_pt->batch.append(0);
        sw_pt->efficient[i] = 0;
        sw_pt->batch[i] = 0;
    }
    for(int i=0; i<SWARMSIZE; i++)
    {
//        sw_pt->newgen.append(0);
        sw_pt->newgen[i] = 0;
    }
    for(int i=0; i<SWARMSIZE; i++)
    {
        sw_pt->newgen[SWARMSIZE+i] = 1;
    }

    QVector<bool> mark_dom(SWARMSIZE * 2);
    QVector<int> mark_record(SWARMSIZE * 2);
    int batch_num = 0;
    while (set_count<SWARMSIZE && batch_num<SWARMSIZE)
    {
        batch_num++;

        for(int i=0; i<SWARMSIZE * 2; i++)
        {
            mark_dom[i] = 0;
            mark_record[i] = -1;
        }

        for(int i=0; i<SWARMSIZE * 2; i++)    //说明后面SWARMSIZE的个体全部被前面的个体支配
        {
//qDebug() << i << " The efficient is" << sw_pt->efficient[i] << endl;
            if(sw_pt->efficient[i]==1)
            {
                mark_dom[i] = 1;
            }

            if(mark_dom[i]==1) continue;

            for(int j=i+1; j<SWARMSIZE * 2; j++)
            {
                if(mark_dom[j] == 1 || sw_pt->efficient[j] == 1)
                    continue;

                int judge = domainComp(sw_pt, i, j);
//qDebug() << i << " The judge is " << judge << ". ";

                if(judge == -1) continue;    //互不支配
                else if (judge == -2)      //完全等同
                {
                     sw_pt->efficient[j] = 1;
                 }
                 else if (judge == i)      //i个体支配j个体
                 {
                     mark_dom[j] = 1;
                  }
                  else                    //j支配i
                  {
                     mark_dom[i] = 1;
                     break;
                  }
            }
        }

//qDebug() << "The batch_num is " << batch_num << endl;

        int f_count = 0;
        for(int i = 0; i < SWARMSIZE * 2; i++)
        {
//qDebug() << i << " The mark_dom is " << mark_dom[i] << ". "; //第二个SWARMSIZE的Mark_dom都是1
            if(0 == mark_dom[i])
            {
                mark_record[f_count] = i;
                sw_pt->efficient[i] = 1;
                sw_pt->batch[i] = batch_num;
                f_count++;
            }
         }

//qDebug() << "The f_count is " << f_count << endl;

        if(set_count==0 || set_count+f_count>=SWARMSIZE)
        {
            //MaoPao
            for(int i=0; i<f_count-1; i++)
            {
                for(int j=f_count-2; j>=i; j--)
                {
                    int dis1 = (int)(sw_pt->distance[mark_record[j]] * 1000000);
                    int dis2 = (int)(sw_pt->distance[mark_record[j+1]] * 1000000);
                    if(dis1<dis2)
                    {
                        int temp = dis1;
                        dis2 = dis1;
                        dis1 = temp;
                    }
                }
            }
        }

        for(int i=0; i<f_count; i++)
        {
//qDebug() << "The i+set_count is " << i+set_count << endl;
//qDebug() << "The mark_record[i] is " << mark_record[i] << endl;
            copySwarm(sw_pt, &(sw_new), i+set_count, mark_record[i], num_testcase);
        }

        if(0 == set_count)
        {
            front_num = f_count;
        }

        set_count += f_count;
    }

    for(int i=0; i<set_count; i++)
    {
        copySwarm(&(sw_new), sw_pt, i, i, num_testcase);
    }

    //release struct sw_new
    QVector<int>().swap(sw_new.particle);
    QVector<float>().swap(sw_new.apsc);
    QVector<float>().swap(sw_new.exetime);
    QVector<int>().swap(sw_new.effnum);
    QVector<float>().swap(sw_new.distance);
    QVector<float>().swap(sw_new.disapsc);
    QVector<float>().swap(sw_new.disexet);
    QVector<bool>().swap(sw_new.efficient);
    QVector<bool>().swap(sw_new.newgen);
    QVector<int>().swap(sw_new.batch);

    QVector<bool>().swap(mark_dom);
    QVector<int>().swap(mark_record);

    sw_pt->range = front_num;

    return front_num;
}

int EliterSelec::eliterSelectionTaea(Swarms *sw_pt, int num_testcase)
{
    int front_num = 0;
    int set_count = 0;

    Swarms sw_new;
    initTempSwarm(&(sw_new), num_testcase);

    for(int i=0; i<2*SWARMSIZE; i++)
    {
//        sw_pt->efficient.append(0);
//        sw_pt->batch.append(0);
        sw_pt->efficient[i] = 0;
        sw_pt->batch[i] = 0;
    }
    for(int i=0; i<SWARMSIZE; i++)
    {
//        sw_pt->newgen.append(0);
        sw_pt->newgen[i] = 0;
    }
    for(int i=0; i<SWARMSIZE; i++)
    {
        sw_pt->newgen[SWARMSIZE+i] = 1;
    }

    QVector<bool> mark_dom(SWARMSIZE * 2);
    QVector<int> mark_record(SWARMSIZE * 2);
    QVector<int> dive_record(SWARMSIZE * 2);

    for(int i=0; i<SWARMSIZE*2; i++)
    {
        mark_dom[i] = 0;
        mark_record[i] = -1;
    }
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        if(sw_pt->efficient[i]==1)
        {
            mark_dom[i] = 1;
        }
        if(mark_dom[i]==1) continue;

        for(int j=i+1; j<SWARMSIZE*2; j++)
        {
            if (mark_dom[j] == 1 || sw_pt->efficient[j] == 1) continue;

            int judge = domainComp(sw_pt, i, j);
            if(judge==-1) continue;
            else if(judge==-2)
                sw_pt->efficient[j] = 1;
            else if(judge==i)
                mark_dom[j] = 1;
            else
            {
                mark_dom[i] = 1;
                break;
            }
        }
    }

    int f_count = 0, d_count = 0;
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        if(0==mark_dom[i])
        {
            mark_record[f_count] = i;
            sw_pt->efficient[i] = 1;
            sw_pt->batch[i] = 1;
            f_count++;
        }
        else if(0!=mark_dom[i] && i<SWARMSIZE)
        {
            dive_record[d_count] = i;
            d_count++;
        }
    }

    //MaoPao
    for(int i=0; i<f_count-1; i++)
    {
        for(int j=f_count-2; j>=i; j--)
        {
            int dis1 = (int)(sw_pt->distance[mark_record[j]] * 1000000);
            int dis2 = (int)(sw_pt->distance[mark_record[j+1]] * 1000000);
            if(dis1<dis2)
            {
                int temp = dis1;
                dis2 = dis1;
                dis1 = temp;
            }
        }
    }

    for(int i=0; i<f_count; i++)
    {
        copySwarm(sw_pt, &(sw_new), i, mark_record[i], num_testcase);
    }

    front_num = f_count;
    set_count = f_count;

    //update_distance_taea
    for(int d=0; d<d_count; d++)
    {
        sw_pt->distance[dive_record[d]] = 0;
        for(int f=0; f<f_count; f++)
        {
            int tempa = qAbs((int)(sw_new.apsc[f]*1000) - (int)(sw_pt->apsc[dive_record[d]]*1000));
            int tempt = qAbs((int)(sw_pt->exetime[dive_record[d]]*1000) - (int)(sw_new.exetime[f]*1000));
            sw_pt->distance[dive_record[d]] += (float)tempa / sw_pt->avgapsc + (float)tempt / sw_pt->avgexetime;
        }
    }

    //Maopao
    for(int i=0; i<d_count-1; i++)
    {
        for(int j=d_count-2; j>=i; j--)
        {
            int dis1 = (int)(sw_pt->distance[dive_record[j]] * 1000000);
            int dis2 = (int)(sw_pt->distance[dive_record[j+1]] * 1000000);
            if(dis1<dis2)
            {
                int temp = dis1;
                dis2 = dis1;
                dis1 = temp;
            }
        }
    }

    for(int i=0; i<d_count; i++)
    {
        copySwarm(sw_pt, &(sw_new), i+set_count, dive_record[i], num_testcase);
    }

    QVector<bool>().swap(mark_dom);
    QVector<int>().swap(mark_record);
    QVector<int>().swap(dive_record);

    for(int i=0; i<SWARMSIZE; i++)
    {
        copySwarm(&(sw_new), sw_pt, i, i, num_testcase);
    }

    //release struct sw_new
    QVector<int>().swap(sw_new.particle);
    QVector<float>().swap(sw_new.apsc);
    QVector<float>().swap(sw_new.exetime);
    QVector<int>().swap(sw_new.effnum);
    QVector<float>().swap(sw_new.distance);
    QVector<float>().swap(sw_new.disapsc);
    QVector<float>().swap(sw_new.disexet);
    QVector<bool>().swap(sw_new.efficient);
    QVector<bool>().swap(sw_new.newgen);
    QVector<int>().swap(sw_new.batch);

    sw_pt->range = front_num;
    return front_num;
}

int EliterSelec::eliterSelectionSpea(Swarms *sw_pt, int num_testcase)
{
    int front_num = 0;
    int set_count = 0;

    Swarms sw_new;
    initTempSwarm(&(sw_new), num_testcase);

    for (int i = 0; i < sw_pt->range ; i++)
        sw_pt->efficient[i] = 0;
    for (int i = sw_pt->range; i < SWARMSIZE ; i++)
        sw_pt->efficient[i] = 1;
    for (int i = SWARMSIZE; i < SWARMSIZE * 2; i++)
        sw_pt->efficient[i] = 0;
    for (int i = 0; i < SWARMSIZE * 2; i++)
        sw_pt->batch[i] = 0;
    for (int i = 0; i < SWARMSIZE; i++)
        sw_pt->newgen[i] = 0;
    for (int i = 0; i < SWARMSIZE; i++)
        sw_pt->newgen[SWARMSIZE + i] = 1;

    QVector<bool> mark_dom(SWARMSIZE*2);
    QVector<int> mark_record(SWARMSIZE*2);

    int batch_num = 0;
    while(set_count < SWARMSIZE && batch_num < SWARMSIZE)
    {
        batch_num++;

        for(int i=0; i<SWARMSIZE*2; i++)
        {
            mark_dom[i] = 0;
            mark_record[i] = -1;
        }

        for(int i=0; i<SWARMSIZE*2; i++)
        {
            if(sw_pt->efficient[i]==1)
                mark_dom[i] = 1;
            if(mark_dom[i]==1) continue;

            for(int j=i+1; j<SWARMSIZE*2; j++)
            {
                if (mark_dom[j] == 1 || sw_pt->efficient[j] == 1) continue;

                int judge = domainComp(sw_pt, i, j);
                if(judge==-1) continue;
                else if(judge==-2)
                    sw_pt->efficient[j] = 1;
                else if(judge==i)
                    mark_dom[j] = 1;
                else
                {
                    mark_dom[i]=1;
                    break;
                }
            }
        }

        int f_count = 0;
        for (int i = 0; i < SWARMSIZE * 2; i++)
        {
            if (0 == mark_dom[i])
            {
                mark_record[f_count] = i;
                sw_pt->efficient[i] = 1;
                sw_pt->batch[i] = batch_num;
                f_count++;
             }
        }

        if(set_count == 0 || set_count + f_count >= SWARMSIZE)
        {
            //MaoPao
            for(int i=0; i<f_count-1; i++)
            {
                for(int j=f_count-2; j>=i; j--)
                {
                    int dis1 = (int)(sw_pt->distance[mark_record[j]] * 1000000);
                    int dis2 = (int)(sw_pt->distance[mark_record[j+1]] * 1000000);
                    if(dis1<dis2)
                    {
                        int temp = dis1;
                        dis2 = dis1;
                        dis1 = temp;
                    }
                }
            }
        }

        for(int i=0; i<f_count; i++)
        {
            copySwarm(sw_pt, &(sw_new), i + set_count, mark_record[i], num_testcase);
        }

        if(0==set_count)
            front_num = f_count;

        set_count += f_count;
    }

    for(int i=0; i<set_count; i++)
    {
        copySwarm(&(sw_new), sw_pt, i, i, num_testcase);
    }

    //release struct sw_new
    QVector<int>().swap(sw_new.particle);
    QVector<float>().swap(sw_new.apsc);
    QVector<float>().swap(sw_new.exetime);
    QVector<int>().swap(sw_new.effnum);
    QVector<float>().swap(sw_new.distance);
    QVector<float>().swap(sw_new.disapsc);
    QVector<float>().swap(sw_new.disexet);
    QVector<bool>().swap(sw_new.efficient);
    QVector<bool>().swap(sw_new.newgen);
    QVector<int>().swap(sw_new.batch);

    QVector<bool>().swap(mark_dom);
    QVector<int>().swap(mark_record);

    sw_pt->range = front_num;
    return front_num;
}

void EliterSelec::initTempSwarm(Swarms *sw_pt, int num_testcase)
{
    //源代码只分配了particle的空间，未初始化, 初始化与否不受影响
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        for(int j=0; j<num_testcase; j++)
        {
            sw_pt->particle.append(j);
        }
    }

    for(int i=0; i<2*SWARMSIZE; i++)
    {
        sw_pt->apsc.append((float)MIN);
        sw_pt->exetime.append((float)MAX);
        sw_pt->distance.append((float)MAX);
        sw_pt->disapsc.append((float)MAX);
        sw_pt->disexet.append((float)MAX);
        sw_pt->effnum.append(0);
        sw_pt->efficient.append(0);
        sw_pt->newgen.append(0);
        sw_pt->batch.append(0);
    }
    sw_pt->avgapsc = 0.0;
    sw_pt->avgexetime = 0.0;
    sw_pt->avgdistance = 0.0;
}

int EliterSelec::domainComp(Swarms *sw_pt, int a, int b)
{
    int a_a = (int)(sw_pt->apsc[a] * 1000000);
    int b_a = (int)(sw_pt->apsc[b] * 1000000);
    int a_c = 1;
    int b_c = 1;
    int a_t = (int)(sw_pt->exetime[a] * 1000);
    int b_t = (int)(sw_pt->exetime[b] * 1000);

    if (a_a == b_a && a_t == b_t && a_c == b_c) return -2;  //两个个体完全相等

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
        if(equ[i]==0) mark_zero = 1;
        sum += equ[i];
    }

    if(mark_zero==1)
    {
        if(sum==0) return -1;             //return -1表示两个个体互不支配
        else if(sum>0) return a;
        else return b;
    }
    else
    {
        if(sum==3) return a;               //a支配b
        else if(sum==-3) return b;
        else return -1;
    }
}

void EliterSelec::copySwarm(Swarms *sw_pt, Swarms *sw_new, int a, int b, int num_testcase)
{
    for(int i=0; i<num_testcase; i++)
    {
        int pos1 = a * num_testcase + i;
        int pos2 = b * num_testcase + i;
        sw_new->particle[pos1] = sw_pt->particle[pos2];
    }

    sw_new->apsc[a] = sw_pt->apsc[b];
    sw_new->exetime[a] = sw_pt->exetime[b];
    sw_new->distance[a] = sw_pt->distance[b];
    sw_new->disapsc[a] = sw_pt->disapsc[b];
    sw_new->disexet[a] = sw_pt->disexet[b];
    sw_new->effnum[a] = sw_pt->effnum[b];
    sw_new->efficient[a] = sw_pt->efficient[b];
    sw_new->newgen[a] = sw_pt->newgen[b];
    sw_new->batch[a] = sw_pt->batch[b];
    return;
}
