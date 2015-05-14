#include "logger.h"
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <iostream>
#include <cmath>

Logger* Logger::myInstance=NULL;

Logger &Logger::instance()
{
   if(myInstance==NULL)
       myInstance=new Logger();
   return *myInstance;
}



Logger::Logger() : QObject(NULL)
{
    myPrintToStdOut=false;
    myLiveLog=true;
}

void Logger::clear()
{
    myCurrentTime.setHMS(0,0,0);
    myLog.clear();
}

QStringList Logger::log() const
{
    return myLog;
}
void Logger::setPrintToStdOut(bool print)
{
    myPrintToStdOut=print;
}
void Logger::setLiveLog(bool live)
{
    myLiveLog=live;
}

void Logger::setFilterLevel(const unsigned& lvl)
{
   myFilterLevel=lvl;
}

Logger& Logger::operator <<(const int& lvl)
{
    myCurrentFilterLevel=(unsigned) lvl;
    return *this;
}
Logger& Logger::operator <<(const unsigned& lvl)
{
    myCurrentFilterLevel=lvl;
    return *this;
}

//Logger& Logger::operator <<(const QDateTime& time)
//{
//    myCurrentTime=time;
//    return *this;
//}

Logger& Logger::operator <<(const uint64_t& time)
{
    int hours = (int) floor(time/3600.0);
    int mins = (int) floor( (time-hours*3600)/60.0);
    int secs = (int)(time - hours*3600 - mins*60);
    myCurrentTime.setHMS(hours,mins,secs);
    return *this;
}

Logger& Logger::operator <<(const QString& message)
{
    if(myCurrentFilterLevel>myFilterLevel)
        return *this;
    QString text = myCurrentTime.toString()+" : "+message;
    qDebug()<<text;
    if(!myLiveLog)
        myLog.append(text);
    else
        emit newMessage(text);
    if(myPrintToStdOut)
        std::cout<<text.toStdString()<<std::endl;
    return *this;
}
