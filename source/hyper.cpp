#include "hyper.h"
#include <QDebug>
#include <QTime>

Hyper::Hyper(QObject *parent) : QObject(parent)
{

}

int Hyper::learnStra(Swarms *sw_pt, Swarms *sw_cp, int change_alg, int change_cro, QVector<float> &learn_val, QVector<int> &learn_time, int num_testcase,
                     QVector<int> &llh_sel, QVector<int> &delta, QString evalu, QString selec, QVector<QVector<float>> &window)
{
    int str_sel = llh_sel.length();
    //qDebug() << "The sel_str is " << str_sel << endl;

    for(int i=0; i<SWARMSIZE; i++)
    {
        sw_cp->apsc[i + SWARMSIZE] = sw_pt->apsc[i];
        sw_cp->exetime[i + SWARMSIZE] = sw_pt->exetime[i];
        sw_cp->distance[i + SWARMSIZE] = sw_pt->distance[i];

        sw_cp->newgen[i] = 0;
        sw_cp->newgen[i + SWARMSIZE] = 1;
    }

    int newrange = eliteLearn.eliterSelectionNsga(sw_cp, num_testcase);
    qDebug() << "The newrange is " << newrange << endl;

//    for(int i=0; i<SWARMSIZE; i++)       //debug
//        qDebug() << "The newgen is " << sw_cp->newgen[i] << " ";

    float g_count = 0;                                    //g_count是分层后种群中每个新个体所在的层数之和（其实是HD计算公式中的权值i）
    int m_value = sw_cp->batch[SWARMSIZE - 1];          //分层后最后一个个体所在的层肯定是最大层，m_value其实就是总共分了多少层

    QVector<float> ss(SWARMSIZE*2, 0.0);  //权重和
    QVector<int> tt(SWARMSIZE*2, 0);    //每层总个体数
    /* for(int i = 0; i < SWARMSIZE; i++)
    {
       ss[i] = 0.0;
       tt[i] = 0;
    } */

    for(int i=0; i<SWARMSIZE; i++)
    {
        if(sw_cp->newgen[i]==1)
        {
            ss[sw_cp->batch[i]] += m_value - sw_cp->batch[i] + 1;
        }
        tt[sw_cp->batch[i]]++;
    }

    for(int i=1; i<=m_value; i++)
    {
        g_count += ss[i] / tt[i];         //g_count就是HD值
    }

    int strategy = change_alg * CRO_NUM + change_cro;   //知道这代用了哪个算法
    learn_time[strategy]++;

    //评价指标
    if(evalu=="hd")
    {
        learn_val[strategy] = g_count;
    }
    else if(evalu=="avghd")
    {
        learn_val[strategy] = (learn_val[strategy] + g_count) / 2;
    }
    else if(evalu=="hdmin")
    {
        learn_val[strategy] = g_count;
        if(learn_val[strategy]==0)
        {
            float sum_weight = 0.0;
            for(int i=0; i<str_sel; i++)
            {
                sum_weight += learn_val[llh_sel[i]];
            }
            learn_val[strategy] = sum_weight * 0.05;
        }
    }
    else if(evalu=="avghdstep")
    {
        if(window[strategy][0]!=-1.0 && window[strategy][1]!=-1.0 && window[strategy][2]!=-1.0
               && window[strategy][3]!=-1.0 && window[strategy][4]!=-1.0)
        {
            for(int i=0; i<4; i++)
            {
                window[strategy][i] = window[strategy][i+1];
            }
            window[strategy][4] = g_count;
            learn_val[strategy] = window[strategy][0]/32 + window[strategy][1]/16 + window[strategy][2]/8
                    + window[strategy][3]/4 + window[strategy][4]/2;
        }
        else
        {
            for(int i=0; i<5; i++)
            {
                if(window[strategy][i]==-1.0)
                {
                    window[strategy][i] = g_count;
                    if(i==0)
                        learn_val[strategy] = window[strategy][0]/2;
                    else if(i==1)
                        learn_val[strategy] = window[strategy][0]/4 + window[strategy][1]/2;
                    else if(i==2)
                        learn_val[strategy] = window[strategy][0]/8 + window[strategy][1]/4 + window[strategy][2]/2;
                    else if(i==3)
                        learn_val[strategy] = window[strategy][0]/16 + window[strategy][1]/8 + window[strategy][2]/4 + window[strategy][3]/2;
                    else
                        learn_val[strategy] = window[strategy][0]/32 + window[strategy][1]/16 + window[strategy][2]/8
                                + window[strategy][3]/4 + window[strategy][4]/2;
                    break;
                }
            }
        }
    }
    else
    {
        qDebug() << "请选择要使用的评价指标" << endl;
    }

    //选择算法
    int num = llh_sel[0];
    if(selec=="greedy")
    {
        float max = learn_val[llh_sel[0]];
        for(int i=1; i<str_sel; i++)
        {
            float temp = learn_val[llh_sel[i]];

            if (temp >= max)
            {
                max = temp;
                num = llh_sel[i];
            }
        }
    }
    else if(selec=="roulette")
    {
        float sum_wei = 0.0;
        QVector<float> pro(str_sel);
        float sum_pro = 0.0;

        for(int i=0; i<str_sel; i++)
        {
            sum_wei += learn_val[llh_sel[i]];
        }
        for(int j=0; j<str_sel; j++)
        {
            pro[j] = (int)((learn_val[llh_sel[j]] / sum_wei) * 10000 + 0.5) / (float)10000;
        }

        QTime timer;
        timer = QTime::currentTime();
        qsrand(timer.msec()+timer.second()*1000);
        float rand_num = qrand() % 10000 / (float)10000;

        for(int k=0; k<str_sel; k++)
        {
            sum_pro += pro[k];
            if(sum_pro>rand_num)
            {
                num = llh_sel[k];
                break;
            }
        }
        QVector<float>().swap(pro);
    }
    else if(selec=="eG")
    {
        QTime time1;
        time1 = QTime::currentTime();
        qsrand(time1.msec()+time1.second()*1000);
        int learn_rate = qrand() % 100;

        if(learn_rate<30)
        {
            int rand1 = qrand() % str_sel;
            num = llh_sel[rand1];
            qDebug() << "random selection: " << llh_sel[rand1] << endl;
        }
        else
        {
            float max = learn_val[llh_sel[0]];
            for(int i=1; i<str_sel; i++)
            {
                float temp = learn_val[llh_sel[i]];

                if (temp >= max)
                {
                    max = temp;
                    num = llh_sel[i];
                }
            }
        }
    }
    else if(selec=="eRG")
    {
        QTime time2;
        time2 = QTime::currentTime();
        qsrand(time2.msec()+time2.second()*1000);
        int rate = qrand() % 100;

        if(rate<60)
        {
            float sum_wei = 0.0;
            QVector<float> pro(str_sel);
            float sum_pro = 0.0;

            for(int i=0; i<str_sel; i++)
            {
                sum_wei += learn_val[llh_sel[i]];
            }
            for(int j=0; j<str_sel; j++)
            {
                pro[j] = (int)((learn_val[llh_sel[j]] / sum_wei) * 10000 + 0.5) / (float)10000;
            }

            QTime time3;
            time3 = QTime::currentTime();
            qsrand(time3.msec()+time3.second()*1000);
            float rand_num = qrand() % 10000 / (float)10000;

            for(int k=0; k<str_sel; k++)
            {
                sum_pro += pro[k];
                if(sum_pro>rand_num)
                {
                    num = llh_sel[k];
                    qDebug() << "roulette selection: " << llh_sel[k] << endl;
                    break;
                }
            }
            QVector<float>().swap(pro);
        }
        else
        {
            float max = learn_val[llh_sel[0]];
            for(int i=1; i<str_sel; i++)
            {
                float temp = learn_val[llh_sel[i]];

                if (temp >= max)
                {
                    max = temp;
                    num = llh_sel[i];
                }
            }
        }
    }
    else
    {
        qDebug() << "请选择要使用的选择算法" << endl;
    }

    qDebug() << "--------------------" << endl;
    qDebug() << "first * " << num << " *" << endl;

    if(learn_val[num]==0)      //所选LLH的learn_val都变为0后,随机选一个进入下一代
    {
        //num = (strategy + 1) % STR;
        QTime time;
        time = QTime::currentTime();
        qsrand(time.msec()+time.second()*1000);
        int rand = qrand() % str_sel;
        num = rand + delta[rand];
    }

    qDebug() << "check * " << num << " *" << endl;
    qDebug() << "--------------------" << endl;

    for (int i = 0; i < str_sel; i++)
    {
        qDebug() << llh_sel[i] << ": " << learn_val[llh_sel[i]];
        if (llh_sel[i] == num)
            qDebug() << " <<<";
        qDebug() << "hello world" <<endl;
    }

    return num;

}
