#include "scoreboard.h"
#include <cmath>
#include <QDebug>

ScoreSheet::ScoreSheet(const QString& nam,QObject* parent) : QObject(parent)
{
    myName=nam;
    myAverageTravelTime=0;
    myAverageOptimalTravelTime=0;
    myAverageWaitingTime=0;
    myMaxTravelTime=0;
    myMaxOptimalTravelTime=0;
    myMaxWaitingTime=0;
    myMinTravelTime=0;
    myMinOptimalTravelTime=0;
    myMinWaitingTime=0;
    myGiveUps=0;

}

ScoreSheet::~ScoreSheet()
{

}
QString ScoreSheet::name()
{
    return myName;
}

void ScoreSheet::addGiveUp()
{
    myGiveUps++;
    emit updated();
}

void ScoreSheet::addDelivered(uint64_t travel, uint64_t optimal, uint64_t wait)
{
    myAverageTravelTime=myAverageTravelTime*myTravelTime.size()/((double)myTravelTime.size()+1)+travel/((double)myTravelTime.size()+1);
    myAverageOptimalTravelTime=myAverageOptimalTravelTime*myOptimalTime.size()/((double)myOptimalTime.size()+1)+optimal/((double)myOptimalTime.size()+1);
    myAverageWaitingTime=myAverageWaitingTime*myWaitingTime.size()/((double)myWaitingTime.size()+1)+wait/((double)myWaitingTime.size()+1);
    myMaxOptimalTravelTime=std::max(optimal,myMaxOptimalTravelTime);
    myMaxTravelTime=std::max(travel,myMaxTravelTime);
    myMaxWaitingTime=std::max(wait,myMaxWaitingTime);
    if(myWaitingTime.size()==0)
        myMinWaitingTime=wait;
    else
        myMinWaitingTime=std::min(wait,myMinWaitingTime);
    if(myTravelTime.size()==0)
        myMinTravelTime=travel;
    else
        myMinTravelTime=std::min(travel,myMinTravelTime);
    if(myOptimalTime.size()==0)
        myMinOptimalTravelTime=optimal;
    else
        myMinOptimalTravelTime=std::min(optimal,myMinOptimalTravelTime);
    myWaitingTime.append(wait);
    myTravelTime.append(travel);
    myOptimalTime.append(optimal);
    emit updated();
}

QList<uint64_t> ScoreSheet::travelTime()
{
    return myTravelTime;
}

QList<uint64_t> ScoreSheet::optimalTime()
{
    return myOptimalTime;
}

QList<uint64_t> ScoreSheet::waitingTime()
{
   return myWaitingTime;
}

void ScoreSheet::print()
{
    qDebug()<<"Average Travel Time: "+QString::number(myAverageTravelTime/60.0);
    qDebug()<<"Average Optimal Travel Time: "+QString::number(myAverageOptimalTravelTime/60.0);
    qDebug()<<"Average Waitng Time: "+QString::number(myAverageWaitingTime/60.0);
    qDebug()<<"Max Travel Time: "+QString::number(myMaxTravelTime/60.0);
    qDebug()<<"Max Optimal Travel Time: "+QString::number(myMaxOptimalTravelTime/60.0);
    qDebug()<<"Max Waitng Time: "+QString::number(myMaxWaitingTime/60.0);
    qDebug()<<"Min Travel Time: "+QString::number(myMinTravelTime/60.0);
    qDebug()<<"Min Optimal Travel Time: "+QString::number(myMinOptimalTravelTime/60.0);
    qDebug()<<"Min Waitng Time: "+QString::number(myMinWaitingTime/60.0);
    qDebug()<<"Delivered : "+QString::number(myTravelTime.size());
    qDebug()<<"Given up: "+QString::number(myGiveUps);
}

QString ScoreSheet::toString()
{
    QString result;
    result+="Average Travel Time: "+QString::number(myAverageTravelTime/60.0)+"\n";
    result+="Average Optimal Travel Time: "+QString::number(myAverageOptimalTravelTime/60.0)+"\n";
    result+="Average Waitng Time: "+QString::number(myAverageWaitingTime/60.0)+"\n";
    result+="Max Travel Time: "+QString::number(myMaxTravelTime/60.0)+"\n";
    result+="Max Optimal Travel Time: "+QString::number(myMaxOptimalTravelTime/60.0)+"\n";
    result+="Max Waitng Time: "+QString::number(myMaxWaitingTime/60.0)+"\n";
    result+="Min Travel Time: "+QString::number(myMinTravelTime/60.0)+"\n";
    result+="Min Optimal Travel Time: "+QString::number(myMinOptimalTravelTime/60.0)+"\n";
    result+="Min Waitng Time: "+QString::number(myMinWaitingTime/60.0)+"\n";
    result+="Delivered : "+QString::number(myTravelTime.size())+"\n";
    result+="Given up: "+QString::number(myGiveUps)+"\n";
    return result;
}

double ScoreSheet::averageTravelTime()
{
    return myAverageTravelTime;
}

double ScoreSheet::averageOptimalTravelTime()
{
    return myAverageOptimalTravelTime;
}

double ScoreSheet::averageWaitingTime()
{
    return myAverageWaitingTime;
}

uint64_t ScoreSheet::maxTravelTime()
{
   return myMaxTravelTime;
}

uint64_t ScoreSheet::maxOptimalTravelTime()
{
   return myMaxOptimalTravelTime;
}

uint64_t ScoreSheet::maxWaitingTime()
{
   return myMaxWaitingTime;
}

uint64_t ScoreSheet::minTravelTime()
{
   return myMinTravelTime;
}

uint64_t ScoreSheet::minOptimalTravelTime()
{
    return myMinOptimalTravelTime;
}

uint64_t ScoreSheet::minWaitingTime()
{
    return myMinWaitingTime;
}
uint64_t ScoreSheet::giveUps()
{
   return myGiveUps;
}
uint64_t ScoreSheet::serviced()
{
   return myTravelTime.size();
}

ScoreBoard* ScoreBoard::myInstance=NULL;

ScoreBoard::ScoreBoard() : QObject(NULL)
{

}


ScoreBoard::~ScoreBoard()
{
    clear();
}

ScoreBoard* ScoreBoard::instance()
{
    if(myInstance==NULL)
        myInstance=new ScoreBoard();
    return myInstance;
}

ScoreSheet* ScoreBoard::addSheet(const QString& nam)
{
    mySheets.append(new ScoreSheet(nam,this));
   emit newSheet(mySheets.back());
    return mySheets.back();
}

ScoreSheet* ScoreBoard::last()
{
   if(mySheets.size()==0)
       return NULL;
   return mySheets.back();
}

QList<ScoreSheet*> ScoreBoard::sheets()
{
   return mySheets;
}

void ScoreBoard::clear()
{
    for(int i=0;i<mySheets.size();++i)
        delete mySheets[i];
    mySheets.clear();
}

