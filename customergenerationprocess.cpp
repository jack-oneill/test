#include "customergenerationprocess.h"
#include <cmath>


CustomerGenerationProcess* CustomerGenerationProcess::myInstance=NULL;

CustomerGenerationProcess::CustomerGenerationProcess(double rate, double seed)
{
    myRate=rate;
    myGenerator=new boost::random::mt19937(seed);
    myUniform=new boost::random::uniform_01<boost::random::mt19937>(*myGenerator);
}
CustomerGenerationProcess::~CustomerGenerationProcess()
{
    delete myGenerator;
    delete myUniform;
}
CustomerGenerationProcess* CustomerGenerationProcess::instance(double rate,double seed)
{
    if(myInstance==NULL)
        myInstance=new CustomerGenerationProcess(rate,seed);
    return myInstance;
}

void CustomerGenerationProcess::setSeed(double seed)
{
    delete myGenerator;
    delete myUniform;
    myGenerator=new boost::random::mt19937(seed);
    myUniform=new boost::random::uniform_01<boost::random::mt19937>(*myGenerator);
}
void CustomerGenerationProcess::setRate(double rate)
{
    myRate=rate;
}

uint64_t CustomerGenerationProcess::next() const
{
    uint64_t result=0;
    double fresult= -log(1-(*myUniform)())/myRate;
    result=(uint64_t) round(3600*fresult);
    return result;
}
