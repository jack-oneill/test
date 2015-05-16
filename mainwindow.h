#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QToolBar>
#include <QThread>
#include "gui/simgraphicsview.h"
#include <QTextEdit>
#include "gui/settings.h"
class RegressionRunner;
class CustomerGeneratorOffline;
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QStatusBar* myStatusBar;
    QAction* myActionSimStart;
    QAction* myActionSimStop;
    QAction* myActionSimPause;
    QAction* myActionOpenNetwork;
    QAction* myActionOpenDistribution;
    QAction* myActionOpenRegression;
    QThread* mySimulationThread;
    RegressionRunner* myRegressionRunner;
    CustomerGeneratorOffline* myCustomerGenerator;

    SimGraphicsView* myView;
    Settings* mySettings;
    QTabWidget* myTabWidget;
    QTextEdit* myLog;
    World* myWorld;
    QMenu* myFileMenu;
    QMenu* myEditMenu;
    QMenu* mySimulationMenu;
    QToolBar* mySimulationToolBar;
    void buildMenus();
    void buildToolBars();
    void initialize();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void openNetwork();
    void openDistribution();
    void openRegression();
    void saveOutput();
    void pauseSimulation();
    void startSimulation();
    void stopSimulation();
};

#endif // MAINWINDOW_H
