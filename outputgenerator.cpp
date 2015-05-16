#include "outputgenerator.h"
#include <QDebug>

OutputGenerator* OutputGenerator::myInstance = NULL;

OutputGenerator::OutputGenerator()
{

}
OutputGenerator::~OutputGenerator()
{

}

OutputGenerator* OutputGenerator::instance()
{
    if(myInstance==NULL)
        myInstance=new OutputGenerator();
    return myInstance;
}

void OutputGenerator::write(const QString& message)
{
    if(myFile.isOpen())
        myStream<<message+"\n";
}

void OutputGenerator::writeCust(const QString& message)
{
    if(myFileCust.isOpen())
        myStreamCust<<message+"\n";
}

void OutputGenerator::open(const QString& file)
{
    close();
    myFile.setFileName(file+".vehicles");
    myFileCust.setFileName(file+".customers");
    if(!myFile.open(QIODevice::WriteOnly) || !myFileCust.open(QIODevice::WriteOnly))
    {
        qDebug()<<"OutputGenerator: Could not open file "+file+".{vehicles,customers} for writing.";
        return;
    }
    myStream.setDevice(&myFile);
    myStreamCust.setDevice(&myFileCust);
}

void OutputGenerator::close()
{
   if(myFile.isOpen())
       myFile.close();
   if(myFileCust.isOpen())
       myFileCust.close();
}
