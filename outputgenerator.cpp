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

void OutputGenerator::open(const QString& file)
{
    close();
    myFile.setFileName(file);
    if(!myFile.open(QIODevice::WriteOnly))
    {
        qDebug()<<"OutputGenerator: Could not open file "+file+" for writing.";
        return;
    }
    myStream.setDevice(&myFile);
}

void OutputGenerator::close()
{
   if(myFile.isOpen())
       myFile.close();
}
