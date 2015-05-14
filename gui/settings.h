#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>

class Settings : public QWidget
{
    Q_OBJECT
    QLineEdit* myMapFileEdit;
    QLineEdit* myDistributionFileEdit;
    /*
    QSpinBox* myNumberOfVehicles;
    QSpinBox* myTimeLimit;
    QDoubleSpinBox* myGenerationRate;
    */
    QSpinBox* mySeed;
    QSpinBox* myLogLevel;
    QCheckBox* myIgnoreWaitingTime;
    QLineEdit* myRegressionFileEdit;
public:
    explicit Settings(QWidget *parent = 0);
    QString mapFile();
    QString distributionFile();
    QString regressionFile();
    /*int numVehicles();
    uint64_t timeLimit();
    double generationRate();*/
    bool ignoreWaitingTime();
    int seed();
    int logLevel();
    void setMapFile(const QString&);
    void setDistributionFile(const QString&);
    void setRegressionFile(const QString&);
signals:
    void openSelectMap();
    void openSelectDistribution();
    void openSelectRegression();

public slots:
    void selectMap();
    void selectDistribution();
    void selectRegression();

};

#endif // SETTINGS_H
