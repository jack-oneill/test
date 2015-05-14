#ifndef REGRESSIONRUNNER_H
#define REGRESSIONRUNNER_H
#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
class SimulationKernel;
class CustomerGeneratorOffline;
class RegressionRunner : public QObject
{
    Q_OBJECT
private:
    QList<QList<int> > mySimulations;
    SimulationKernel* myKernel;
    CustomerGeneratorOffline* myGenerator;
public:
    RegressionRunner(SimulationKernel*, CustomerGeneratorOffline*,QObject* parent = NULL);
    ~RegressionRunner();
    bool load(const QString&);
    unsigned numRegressions();
signals:
    void regressionEnded();
public slots:
    void pause();
    void start();
    void stop();

};

#endif // REGRESSIONRUNNER_H
