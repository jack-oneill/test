#ifndef SCOREBOARD_H
#define SCOREBOARD_H
#include <QObject>
#include <QString>
#include <QList>
class ScoreSheet : public QObject
{
    Q_OBJECT
private:
    double myAverageTravelTime;
    double myAverageOptimalTravelTime;
    double myAverageWaitingTime;
    uint64_t myMaxTravelTime;
    uint64_t myMaxOptimalTravelTime;
    uint64_t myMaxWaitingTime;
    uint64_t myMinTravelTime;
    uint64_t myMinOptimalTravelTime;
    uint64_t myMinWaitingTime;
    QList<uint64_t> myTravelTime;
    QList<uint64_t> myOptimalTime;
    QList<uint64_t> myWaitingTime;
    uint64_t myGiveUps;
    QString myName;
public:
    ScoreSheet(const QString&,QObject* parent=NULL);
    ~ScoreSheet();
    QString name();
    void addGiveUp();
    void addDelivered(uint64_t travel, uint64_t optimal, uint64_t wait);
    void print();
    QString toString();
    QList<uint64_t> travelTime();
    QList<uint64_t> optimalTime();
    QList<uint64_t> waitingTime();
    double averageTravelTime();
    double averageOptimalTravelTime();
    double averageWaitingTime();
    uint64_t maxTravelTime();
    uint64_t maxOptimalTravelTime();
    uint64_t maxWaitingTime();
    uint64_t minTravelTime();
    uint64_t minOptimalTravelTime();
    uint64_t minWaitingTime();
    uint64_t giveUps();
    uint64_t serviced();
signals:
    void updated();
};

class ScoreBoard : public QObject
{
    Q_OBJECT
private:
    QList<ScoreSheet*> mySheets;
    ScoreBoard();
    ~ScoreBoard();
    static ScoreBoard* myInstance;
public:
    static ScoreBoard* instance();
    ScoreSheet* addSheet(const QString&);
    void clear();
    ScoreSheet* last();
    QList<ScoreSheet*> sheets();
signals:
    void newSheet(ScoreSheet*);
public slots:

};

#endif // SCOREBOARD_H
