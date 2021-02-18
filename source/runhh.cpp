#include "runhh.h"
//#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QtGlobal>
// #include <windows.h>
// #include <wincon.h>

RunHH::RunHH(QObject *parent) : QObject(parent)
{    

    qRegisterMetaType<Swarms>("Swarms");
    qRegisterMetaType<Swarms>("Schema");

//    clock = new QTimer();
//    clock->setInterval(1000);
    /*connect(clock, &QTimer::timeout, [=](){
        static int sec = 0;
        sec++;
        //qDebug() << "runTime: " << sec << endl;
    }); */
//    connect(clock, SIGNAL(timeout()), this, SLOT(runFrame()));
//    clock->start();
}

void RunHH::runFrame(QString subName, QVector<bool> &alg_sel, QVector<bool> &cro_sel, QString evalu, QString selec, int testObj)
{
//    Widget widgetUi;
//    QString subName = widgetUi.ui->lineEdit->text();
//    qDebug() << "The subName is " << subName << endl;

    int num_testcase = 0;
    int code_line = 0;
    checkFile(num_testcase, code_line, subName);
    qDebug() << "num testcase:" << num_testcase << endl << "code line:" << code_line << endl;
    //ReadFile

    QVector<bool> cMatrix;
    QVector<float> tVector;
    QVector<bool> max_cover(code_line);
    QVector<float> learn_val;
    QVector<int> learn_time;    
    readFile(cMatrix, tVector, max_cover, code_line, subName);

    /*QVector<float>::iterator iter;
    for (iter=tVector.begin();iter!=tVector.end();iter++)
    {
        qDebug() << *iter << " ";
    }

    QFile testFile("C:\\Users\\w\\Desktop\\test.txt");
    testFile.open(QIODevice::WriteOnly);
    QTextStream inTest(&testFile);
    QVector<bool>::iterator iter1;
    for(iter1=max_cover.begin();iter1!=max_cover.end();iter1++)
    {
        inTest << *iter1;
    }
    testFile.close();*/

    //被选中的底层算法数
    int alg_num = 0;
    int cro_num = 0;
    for(int i=0; i<3; i++)
    {
        if(alg_sel[i])
            alg_num++;
    }
    for(int i=0; i<6; i++)
    {
        if(cro_sel[i])
            cro_num++;
    }
    int STR_SEL = alg_num * cro_num;

    QVector<int> llh_sel(STR_SEL);    //被选择的LLH序号
    QVector<int> delta(STR_SEL);      //若被选择的LLH为连续序号，则llh_sel和连续序号的差值

    int t1 = 0;
    for(int i=0; i<3; i++)
    {
       if(alg_sel[i])
       {
           for(int j=0; j<6; j++)
           {
               if(cro_sel[j])
               {
                   int sequ = i * CRO_NUM + j;
                   llh_sel[t1] = sequ;
                   t1++;
               }
           }
       }
    }

    for(int i=0; i<STR_SEL; i++)
    {
        int temp = llh_sel[i] - i;
        delta[i] = temp;
    }

    int row = STR;
    int col = 5;
    QVector<QVector<float>> window(row);    //avghdstep用到
    for(int i=0; i<row; i++)
    {
        window[i].resize(col);
    }

    //开始loop,假设一个loop
    //初始化
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            window[i][j] = -1.0;
        }
    }

    Swarms swarm;
    Swarms temp_swarm;
    Swarms copy_swarm;
    Swarms *sw_pt = &swarm;
    Swarms *new_sw_pt = &temp_swarm;
    Swarms *sw_cp = &copy_swarm;
    iniIndividuals(sw_pt, learn_val, learn_time, num_testcase);
    Schema schema;
    Schema *sch_pt = &schema;
    iniSchema(sch_pt, num_testcase);

    //计算适应度
    int start = 0;
    //calFitness(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover);
    calfitn.calFitnessTcp(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, start, testObj);
    //再次初始化
    if(ALG==-1)
    {
        iniIndividualsPure(new_sw_pt, sw_pt, num_testcase);
        iniIndividualsPure(sw_cp, sw_pt, num_testcase);
    }

    //
    int front_0 = 0;
    int change_cro = 0;
    int change_alg = 0;
    int learn = 0;
    //终止条件
    int c_a = 0, c_t = 0;
    int c_sum = 0;
//    int c_judge = 0;

    /*if(clock->isActive()==false)  //开始计时
        clock->start(); */
//    static int sec = 0;
//    sec++;

    int gener = 0;
    for(int g=0; g<20; g++)
    {
        qDebug() << "gener: " << g << " loops: " << "1" << endl;

        int start1 = SWARMSIZE;

        int dic_num = 0;
        if(g<STR_SEL) dic_num = llh_sel[g];
        else dic_num = learn;
        change_alg = dic_num / CRO_NUM;
        change_cro = dic_num % CRO_NUM;
        qDebug() << "The algorithm is " << change_alg << ","  << "the crossover is " << change_cro << endl;

        if(change_alg == 0)  //NSGAII
        {
            front_0 = nsga.nsgaII(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, g, change_cro, start1, testObj);
        //qDebug() << "front_nsga" << front_0 << endl;
        }
        else if(change_alg == 1)  //TAEA
        {
            front_0 = taea.taea(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, front_0, change_cro, start1, testObj);
        //qDebug() << "front_taea" << front_0 << endl;
        }
        else if(change_alg == 2)  //SPEA2
        {
            front_0 = spea.spea(sw_pt, num_testcase, code_line, cMatrix, tVector, max_cover, change_cro, start1, testObj);
        //qDebug() << "front_spea" << front_0 << endl;
        }
        else
            qDebug() << "Error: no algorithm " << ALG << endl;

        if(ALG==-1)
        {
            learn = selecter.learnStra(sw_pt, sw_cp, change_alg, change_cro, learn_val, learn_time, num_testcase,
                                       llh_sel, delta, evalu, selec, window);
        }

        //Stop condition judge
        float temp_a = 0, temp_t = 0;
        for(int i=0; i<front_0; i++)
        {
            temp_a += sw_pt->apsc[i];
            temp_t += sw_pt->exetime[i];
        }

        int change_a = qAbs(c_a - (int)(temp_a / (float)front_0 * 1000));
        int change_t = qAbs(c_t - (int)(temp_t / (float)front_0 * 1000));

        c_a = abs((int)(temp_a / (float)front_0 * 1000));
        c_t = abs((int)(temp_t / (float)front_0 * 1000));

        gener = g;
        int temp_sum = change_a + change_t;
        if(temp_sum==0) c_sum += 1;
        else c_sum = 0;
        if(c_sum==10)
        {
            if(gener<50)
                continue;
            else
                break;
        }

        if (sw_pt->apsc[0] == 1) break;

        qDebug() << "********" << c_sum << "********" << endl;

    }  //end of generation

    qDebug() << sw_pt->range << endl;

    for(int i=0; i<sw_pt->range; i++)
    {
        qDebug() << sw_pt->apsc[i] << " " << sw_pt->exetime[i] << endl;
//        for(int j=0; j<sw_pt->effnum[i]; j++)
//        {
//            if(sw_pt->particle[i*num_testcase+j]==1)
//            {

//            }
//        }
    }
    QString res = "";
    for(int i=0; i<sw_pt->range; i++)
    {
        qDebug() << "第" << i+1 << "个解：";
        res = res + "第" + QString::number(i+1) + "个解:\n";
        for(int j=i*num_testcase; j<(i+1)*num_testcase; j++)
        {
            qDebug() << sw_pt->particle[j] << "eee" <<endl;
            res = res + QString::number(sw_pt->particle[j]) + "\n";
        }
    }
    emit sendMessage(res);

    //end of loop

    if(ALG==-1)
    {
        //free new_sw_pt
        QVector<int>().swap(new_sw_pt->particle);
        QVector<float>().swap(new_sw_pt->apsc);
        QVector<float>().swap(new_sw_pt->exetime);
        QVector<int>().swap(new_sw_pt->effnum);
        QVector<float>().swap(new_sw_pt->distance);
        QVector<float>().swap(new_sw_pt->disapsc);
        QVector<float>().swap(new_sw_pt->disexet);
        QVector<bool>().swap(new_sw_pt->efficient);
        QVector<bool>().swap(new_sw_pt->newgen);
        QVector<int>().swap(new_sw_pt->batch);

        //free sw_cp
        QVector<int>().swap(sw_cp->particle);
        QVector<float>().swap(sw_cp->apsc);
        QVector<float>().swap(sw_cp->exetime);
        QVector<int>().swap(sw_cp->effnum);
        QVector<float>().swap(sw_cp->distance);
        QVector<float>().swap(sw_cp->disapsc);
        QVector<float>().swap(sw_cp->disexet);
        QVector<bool>().swap(sw_cp->efficient);
        QVector<bool>().swap(sw_cp->newgen);
        QVector<int>().swap(sw_cp->batch);
    }

    QVector<bool>().swap(cMatrix);
    QVector<float>().swap(tVector);
    QVector<bool>().swap(max_cover);
    QVector<float>().swap(learn_val);
    QVector<int>().swap(learn_time);
    for(int i=0; i<row; i++)
    {
        QVector<float>().swap(window[i]);
    }
    QVector<QVector<float>>().swap(window);

//    if(clock->isActive()==true)   //结束计时
//        clock->stop();
//    qDebug() << "Time: " << sec << endl;

    emit gotRun();
}



void RunHH::checkFile(int &num_testcase, int &code_line, QString subName)
{
    //获取文件
    QString inputPath = ":/new_total_cover/";
    QString covFile = subName + ".cov";
    QString eetFile = subName + ".time";
    fileCover = inputPath + covFile;
    fileEet = inputPath + eetFile;
    //fileCover = ":/new_total_cover/commons-cli.cov";
    //fileEet = ":/new_total_cover/commons-cli.time";
    QFile file_cov(fileCover);
    QFile file_eet(fileEet);
    file_cov.open(QIODevice::ReadOnly);
    file_eet.open(QIODevice::ReadOnly);

    //获取测试用例数
    QTextStream inEet(&file_eet);
    int testNum = 0;
    while(!inEet.atEnd())
    {
        inEet.readLine();
        testNum++;
    }
    file_eet.close();
    num_testcase = testNum;

    //获取代码行数
    QTextStream inCov(&file_cov);
    int lineNum = 0;
    QString content = "";
    while(!inCov.atEnd())
    {
        content += inCov.readLine();
        break;
    }
    file_cov.flush();
    file_cov.close();
    lineNum = content.count();
    //lineNum = content.length();
    code_line = lineNum;
}


void RunHH::readFile(QVector<bool> &cMatrix, QVector<float> &tVector, QVector<bool> &max_cover, int code_line, QString subName)
{
    //获取文件
    QString inputPath = ":/new_total_cover/";
    QString covFile = subName + ".cov";
    QString eetFile = subName + ".time";
    fileCover = inputPath + covFile;
    fileEet = inputPath + eetFile;
    //fileCover = ":/new_total_cover/commons-cli.cov";
    //fileEet = ":/new_total_cover/commons-cli.time";
    QFile file_cov(fileCover);
    QFile file_eet(fileEet);
    file_cov.open(QIODevice::ReadOnly);
    file_eet.open(QIODevice::ReadOnly);

    //把测试用例执行时间写到tVector向量中
    //int testNum = 0;
    QString test;
    float eet;
    QTextStream inEet(&file_eet);
    while(!inEet.atEnd())
    {
        test = inEet.readLine();
        eet = test.toFloat();
       // qDebug() << eet << " ";
        tVector.append(eet);
    }

    for(int i=0; i<code_line; i++)
    {
        max_cover[i] = 0;
    }

    //把覆盖矩阵写到cMatrix向量中
    int pos = 0;
    //int line = 0;
    QTextStream inCov(&file_cov);
    QString cont = "";
    while(!inCov.atEnd())
    {
        cont = inCov.readLine(); //读取每一行内容
        pos = 0;
        for(int i=0; i<code_line; i++)
        {
            if(cont.at(i)=="1")
            {
                cMatrix.append(1);
                //cMatrix[line * code_line + pos] = 1;
                max_cover[pos] = 1;
                pos++;
            }
            else
            {
                cMatrix.append(0);
                //cMatrix[line * code_line + pos] = 0;
                pos++;
            }
        }
        //line++;
    }

    file_cov.flush();
    file_cov.close();
    file_eet.close();
}


void RunHH::iniIndividuals(Swarms *sw_pt, QVector<float> &learn_val, QVector<int> &learn_time, int num_testcase)
{
    //给种群每个个体进行序列编码,种群particle空间为2*SWARMSIZE*num_testcase
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        for(int j=0; j<num_testcase; j++)
        {
            sw_pt->particle.append(j);
        }
    }

    //初始化种群个体，随机打乱顺序编码
    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    for(int i=0; i<SWARMSIZE; i++)  //i表示种群中第i个个体
    {
        int j = num_testcase-1;
        for(int k=(i+1)*num_testcase-1; k>i*num_testcase; k--) //k表示种群中第i个个体中的第k个测试用例
        {
            int rand = qrand()%j + i*num_testcase;
            int temp = sw_pt->particle[k];
            sw_pt->particle[k] = sw_pt->particle[rand];
            sw_pt->particle[rand] = temp;
            j--;
        }
    }
    //int n = qrand()%100;

    //对种群这个结构体组成进行初始化
    for(int i=0; i<2*SWARMSIZE; i++)
    {
        sw_pt->apsc.append((float)MIN);
        sw_pt->exetime.append((float)MAX);
        sw_pt->distance.append((float)MIN);
        sw_pt->disapsc.append((float)MIN);
        sw_pt->disexet.append((float)MAX);
        sw_pt->effnum.append(0);
        sw_pt->efficient.append(0);
        sw_pt->newgen.append(0);
        sw_pt->batch.append(0);
    }
    sw_pt->avgapsc = 0.0;
    sw_pt->avgexetime = 0.0;
    sw_pt->avgdistance = 0.0;
    sw_pt->range = SWARMSIZE;

    for(int i=0; i<STR; i++)
    {
        learn_val.append(0.0);
        learn_time.append(0);
    }
}

void RunHH::iniSchema(Schema *sch_pt, int num_testcase)
{
    //初始化
    for(int i=0; i<num_testcase; i++)
    {
        sch_pt->matrix_map.append(-1);
        sch_pt->schema.append(-1);
    }
    for(int i=0; i<SWARMSIZE; i++)
    {
        sch_pt->sch_value.append(0.0);
    }
    for(int i=0; i<SWARMSIZE*num_testcase; i++)
    {
        sch_pt->sch_table.append(-1);
        sch_pt->sch_chose.append(0);
    }

    int count = 0;
    sch_pt->map_size = count;
}

/*
void RunHH::calFitness(Swarms *sw_pt, int num_testcase, int code_line, QVector<bool> &cMatrix, QVector<float> &tVector, QVector<bool> &max_cover)
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
    //qDebug() << "sw_pt->avgexetime: " << sw_pt->avgexetime << endl;

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
    //qDebug() << "sw_pt->avgdistance: " << sw_pt->avgdistance << endl;
} */

void RunHH::iniIndividualsPure(Swarms *sw, Swarms *sw_old, int num_testcase)
{
    for(int i=0; i<SWARMSIZE*2; i++)
    {
        for(int j=0; j<num_testcase; j++)
        {
            sw->particle.append(sw_old->particle[i*num_testcase+j]);
        }
    }

    for(int i=0; i<SWARMSIZE*2; i++)
    {
        sw->apsc.append(sw_old->apsc[i]);
        sw->exetime.append(sw_old->exetime[i]);
        sw->distance.append(sw_old->distance[i]);
        sw->disapsc.append(sw_old->disapsc[i]);
        sw->disexet.append(sw_old->disexet[i]);
        sw->effnum.append(sw_old->effnum[i]);
        sw->efficient.append(sw_old->efficient[i]);
        sw->newgen.append(sw_old->newgen[i]);
        sw->batch.append(sw_old->batch[i]);
    }

    sw->avgapsc = 0.0;
    sw->avgexetime = 0.0;
    sw->avgdistance = 0.0;
    sw->range = SWARMSIZE;
}





