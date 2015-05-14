#include "settings.h"
#include <QSizePolicy>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>

Settings::Settings(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout* layout = new  QGridLayout(this);
    QLabel* label;
    QPushButton* button;
    setLayout(layout);
    QGroupBox* box0 = new QGroupBox("Files",this);
    layout->addWidget(box0,0,0,5,5);
    QGridLayout* layout0 = new  QGridLayout(box0);
    box0->setLayout(layout0);
    box0->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    label = new QLabel("Map",this);
    layout0->addWidget(label,0,0,1,1);
    myMapFileEdit = new QLineEdit("Select Map File",box0);
    myMapFileEdit->setReadOnly(true);
    layout0->addWidget(myMapFileEdit,0,1,1,3);
    button = new QPushButton("Select",this);
    layout0->addWidget(button,0,4,1,1);
    connect(button,SIGNAL(clicked()),this,SLOT(selectMap()));
    label = new QLabel("Distribution",this);
    layout0->addWidget(label,1,0,1,1);
    myDistributionFileEdit= new QLineEdit("Select Distribution File",box0);
    myDistributionFileEdit->setReadOnly(true);
    layout0->addWidget(myDistributionFileEdit,1,1,1,3);
    button = new QPushButton("Select",this);
    layout0->addWidget(button,1,4,1,1);
    connect(button,SIGNAL(clicked()),this,SLOT(selectDistribution()));

    label = new QLabel("Regression",this);
    layout0->addWidget(label,2,0,1,1);
    myRegressionFileEdit= new QLineEdit("Select Regression File",box0);
    myRegressionFileEdit->setReadOnly(true);
    layout0->addWidget(myRegressionFileEdit,2,1,1,3);
    button = new QPushButton("Select",this);
    layout0->addWidget(button,2,4,1,1);
    connect(button,SIGNAL(clicked()),this,SLOT(selectRegression()));

    QGroupBox* box1 = new QGroupBox("Simulation",this);
    layout->addWidget(box1,5,0,5,5);
    QGridLayout* layout1 = new  QGridLayout(box1);

    box1->setLayout(layout1);
    box1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    /*
    label = new QLabel("Time limit [h]",this);
    layout1->addWidget(label,0,0,1,1);
    myTimeLimit = new QSpinBox(this);
    myTimeLimit->setMinimum(1);
    layout1->addWidget(myTimeLimit,0,1,1,1);
    label = new QLabel("Number of Vehicles",this);
    layout1->addWidget(label,0,2,1,1);
    myNumberOfVehicles = new QSpinBox(this);
    myNumberOfVehicles->setMinimum(1);
    layout1->addWidget(myNumberOfVehicles,0,3,1,1);
    label = new QLabel("Customer Generation Rate [nCust/h]",this);
    layout1->addWidget(label,1,0,1,1);
    myGenerationRate = new QDoubleSpinBox(this);
    myGenerationRate->setMinimum(0.01);
    layout1->addWidget(myGenerationRate,1,1,1,1);*/
    label = new QLabel("Seed",this);
    layout1->addWidget(label,0,2,1,1);
    mySeed = new QSpinBox(this);
    layout1->addWidget(mySeed,0,3,1,1);
    label = new QLabel("Log Level",this);
    layout1->addWidget(label,1,0,1,1);
    myLogLevel = new QSpinBox(this);
    layout1->addWidget(myLogLevel,1,1,1,1);
    myIgnoreWaitingTime = new QCheckBox("Ignore Waiting Time",this);
    layout1->addWidget(myIgnoreWaitingTime,2,2,1,2);
}
QString Settings::mapFile()
{
    return myMapFileEdit->text();
}

QString Settings::distributionFile()
{
    return myDistributionFileEdit->text();
}

QString Settings::regressionFile()
{
    return myRegressionFileEdit->text();
}
void Settings::setMapFile(const QString& file)
{
    myMapFileEdit->setText(file);
}

void Settings::setDistributionFile(const QString& file)
{
    myDistributionFileEdit->setText(file);
}

void Settings::setRegressionFile(const QString& file)
{
    myRegressionFileEdit->setText(file);
}

/*int Settings::numVehicles()
{
    return myNumberOfVehicles->value();
}

uint64_t Settings::timeLimit()
{
    return (uint64_t)myTimeLimit->value();
}
double Settings::generationRate()
{
    return myGenerationRate->value();
}*/
bool Settings::ignoreWaitingTime()
{
   return myIgnoreWaitingTime->isChecked();
}

int Settings::seed()
{
    return mySeed->value();
}

int Settings::logLevel()
{
    return myLogLevel->value();
}

void Settings::selectMap()
{
    emit openSelectMap();
}

void Settings::selectDistribution()
{
    emit openSelectDistribution();
}

void Settings::selectRegression()
{
   emit openSelectRegression();
}
