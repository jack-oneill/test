#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QStringList>
#include <QDateTime>
class Logger : public QObject
{
    Q_OBJECT
    Logger();
    static Logger* myInstance;
    QStringList myLog;
    bool myPrintToStdOut;
    bool myLiveLog;
    QTime myCurrentTime;
    unsigned myFilterLevel;
    unsigned myCurrentFilterLevel;
public:
    static Logger& instance();
    void clear();
    QStringList log() const;
    void setPrintToStdOut(bool=true);
    void setLiveLog(bool=true);
    void setFilterLevel(const unsigned&);
    Logger& operator <<(const unsigned&);
    Logger& operator <<(const int&);
    Logger& operator <<(const QDateTime &);
    Logger& operator <<(const uint64_t&);
    Logger& operator <<(const QString&);
signals:
    void newMessage(const QString&);

public slots:

};

#endif // LOGGER_H
