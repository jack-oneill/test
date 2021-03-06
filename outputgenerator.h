#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H
#include <QFile>
#include <QTextStream>
class OutputGenerator
{
private:
    static OutputGenerator* myInstance;
    OutputGenerator();
    ~OutputGenerator();
    QFile myFile;
    QFile myFileCust;
    QTextStream myStream;
    QTextStream myStreamCust;
public:
    static OutputGenerator* instance();
    void write(const QString&);
    void writeCust(const QString&);
    void open(const QString&);
    void close();
};

#endif // OUTPUTGENERATOR_H
