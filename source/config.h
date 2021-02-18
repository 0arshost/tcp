#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QMetaType>

#define ALG -1
#define TYPE -1
#define CRO_NUM 6
#define ALG_NUM 3
#define STR (CRO_NUM * ALG_NUM)
#define SWARMSIZE 200
#define GENERATION 10000
#define MIN 1e-6
#define MAX 1000000
#define MUTRATE 0.001
#define CRORATE 0.1

struct Swarms
{

};

Q_DECLARE_METATYPE(Swarms)

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);

signals:

};

#endif // CONFIG_H
