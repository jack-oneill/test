#include "mainwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QGridLayout>
#include <QGraphicsPathItem>
#include <QDebug>
#include "gui/logger.h"
#include "simulationkernel.h"
#include "customergenerationprocess.h"
#include "customergenerator.h"
#include  "gui/statistics.h"
#include "customergeneratoroffline.h"
#include "vehiclerouter.h"
#include "agentfactory.h"
#include "regressionrunner.h"
#include "vehicledispatcher.h"
#include <QStatusBar>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Logger::instance().setFilterLevel(200);
    myTimeLabel=new QLabel(this);
    myView=new SimGraphicsView(myTimeLabel,this);
    myTabWidget = new QTabWidget(this);
    myTabWidget->addTab(myView,"Map");
    myLog = new QTextEdit(this);
    myLog->setReadOnly(true);
    myTabWidget->addTab(myLog,"Log");
    Statistics* stat  = new Statistics(this);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(stat);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    stat->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    scrollArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    myTabWidget->addTab(scrollArea,"Statistics");
    mySettings = new Settings(this);
    myTabWidget->addTab(mySettings,"Settings");
    initialize();
    buildMenus();
    buildToolBars();
    setCentralWidget(myTabWidget);
    myWorld=NULL;
    connect(&Logger::instance(),SIGNAL(newMessage(const QString&)),myLog,SLOT(append(const QString&)));
    myStatusBar=new QStatusBar(this);
    setStatusBar(myStatusBar);
    myStatusBar->addWidget(new QLabel("Time: ",this));
    myStatusBar->addWidget(myTimeLabel);
    mySimulationThread=NULL;
    myRegressionRunner=NULL;
    myCustomerGenerator=NULL;
}
void MainWindow::buildMenus()
{
    myFileMenu=menuBar()->addMenu("&File");
    myFileMenu->addAction(myActionOpenNetwork);
    myFileMenu->addAction(myActionOpenDistribution);
    myFileMenu->addAction(myActionOpenRegression);
    myEditMenu=menuBar()->addMenu("&Edit");
    mySimulationMenu=menuBar()->addMenu("&Simulation");
    mySimulationMenu->addAction(myActionSimStart);
    mySimulationMenu->addAction(myActionSimPause);
    mySimulationMenu->addAction(myActionSimStop);
}

void MainWindow::buildToolBars()
{
   QToolBar* bar = new QToolBar("General",this);
   bar->addAction(myActionOpenNetwork);
   bar->addAction(myActionOpenDistribution);
   bar->addAction(myActionOpenRegression);
   addToolBar(bar);
   QToolBar* simulation = new QToolBar("Simulation",this);
   simulation->addAction(myActionSimStart);
   simulation->addAction(myActionSimPause);
   simulation->addAction(myActionSimStop);
   addToolBar(simulation);
}

void MainWindow::initialize()
{
    myActionOpenNetwork=new QAction("Open Map",this);
    myActionOpenDistribution=new QAction("Open Distribtuion", this);
    myActionOpenRegression= new QAction("Open Regression",this);
    myActionSimPause=new QAction("&Pause",this);
    myActionSimStart=new QAction("&Start",this);
    myActionSimStop=new QAction("S&top",this);
    connect(myActionOpenNetwork,SIGNAL(triggered()),this,SLOT(openNetwork()));
    connect(mySettings,SIGNAL(openSelectMap()),this,SLOT(openNetwork()));
    connect(mySettings,SIGNAL(openSelectDistribution()),this,SLOT(openDistribution()));
    connect(mySettings,SIGNAL(openSelectRegression()),this,SLOT(openRegression()));
    connect(myActionOpenDistribution,SIGNAL(triggered()),this,SLOT(openDistribution()));
    connect(myActionOpenRegression,SIGNAL(triggered()),this,SLOT(openRegression()));
    connect(myActionSimStart,SIGNAL(triggered()),this,SLOT(startSimulation()));
    connect(myActionSimPause,SIGNAL(triggered()),this,SLOT(pauseSimulation()));
    connect(myActionSimStop,SIGNAL(triggered()),this,SLOT(stopSimulation()));

}

MainWindow::~MainWindow()
{
    if(myRegressionRunner)
        delete myRegressionRunner;
    if(myCustomerGenerator)
        delete myCustomerGenerator;
}

void MainWindow::openNetwork()
{
   QFileDialog dialog;
   QString file  = dialog.getOpenFileName(this,"Open Map",QString(),"*.map");
   if(file=="")
       return;
   if(myWorld)
       delete myWorld;
   myWorld = new World(file,this);
   if(!myWorld->valid())
   {
       qDebug()<<"World Map Invalid - Check Map & Neighborhood Files";
       delete myWorld;
       myWorld=NULL;

   }
   else
   {
       mySettings->setMapFile(file);
       myView->setWorld(myWorld);
   }
}

void MainWindow::openDistribution()
{
   QFileDialog dialog;
   QString file  = dialog.getOpenFileName(this,"Open Distribution",QString(),"*.dist");
   mySettings->setDistributionFile(file);
}

void MainWindow::openRegression()
{
   QFileDialog dialog;
   QString file  = dialog.getOpenFileName(this,"Open Regression",QString(),"*.reg");
   mySettings->setRegressionFile(file);
}

void MainWindow::saveOutput()
{

}

void MainWindow::pauseSimulation()
{

}

void MainWindow::startSimulation()
{
    if(myWorld==NULL)
        return;
    if(mySimulationThread)
       delete mySimulationThread;
    if(myRegressionRunner)
       delete myRegressionRunner;
    if(myCustomerGenerator)
       delete myCustomerGenerator;
    myView->clear();
    mySimulationThread = new QThread(this);
    SimulationKernel* kernel = new SimulationKernel();
    kernel->moveToThread(mySimulationThread);
    kernel->setWorld(myWorld);
    myWorld->setKernel(kernel);
    myWorld->clear();
    //lemon::SmartDigraph::Node node;// = myWorld->network()->graph()->nodeFromId(0);
    AgentFactory::instance()->reset();
    Logger::instance().setFilterLevel(mySettings->logLevel());
    bool isLoaded=true;
    myCustomerGenerator = new CustomerGeneratorOffline(myWorld,"",15);
    isLoaded = myCustomerGenerator ->load(mySettings->distributionFile());
    myRegressionRunner= new RegressionRunner(kernel,myCustomerGenerator);
    myRegressionRunner->moveToThread(mySimulationThread);
    isLoaded = isLoaded && myRegressionRunner->load(mySettings->regressionFile());
    if(isLoaded==false)
    {
        qDebug()<<"Could not start simulation because of missing or corrupt input files";
        return;
    }
    /*
    if(mySettings->distributionFile()!="")
    {
        CustomerGenerator::instance(myWorld,mySettings->seed())->load(mySettings->distributionFile());
        CustomerGenerator::instance(myWorld,mySettings->seed())->setSeed(mySettings->seed());
    }
    else
        return;

    Vehicle* veh;
    for(int i=0;i<mySettings->numVehicles();++i)
    {
        node = CustomerGenerator::instance(myWorld)->generate().first;
        veh = AgentFactory::instance()->newVehicle("Vehicle",3,node,myWorld);
        myWorld->addVehicle(veh);
    }

    CustomerGenerationProcess::instance()->setSeed(mySettings->seed());
    CustomerGenerationProcess::instance()->setRate(mySettings->generationRate());
    */
    VehicleRouter::instance(myWorld)->setIgnoreWaitingTime(mySettings->ignoreWaitingTime());
    VehicleDispatcher::instance()->setup(myWorld,myCustomerGenerator);
    //kernel->setTimeLimit(mySettings->timeLimit());
    connect(mySimulationThread,SIGNAL(started()),myRegressionRunner,SLOT(start()));
    connect(mySimulationThread,SIGNAL(started()),myView,SLOT(startRefreshCycle()));

    //connect(kernel,SIGNAL(simulationEnded()),mySimulationThread,SLOT(quit()));
    connect(myRegressionRunner,SIGNAL(regressionEnded()),mySimulationThread,SLOT(quit()));
    connect(myRegressionRunner,SIGNAL(regressionEnded()),myView,SLOT(stopRefreshCycle()));
    mySimulationThread->start();
    myView->startRefreshCycle();

}

void MainWindow::stopSimulation()
{

}
