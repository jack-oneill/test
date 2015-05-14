#ifndef CUSTOMERGENERATIONPROCESS_H
#define CUSTOMERGENERATIONPROCESS_H
#include <stdlib.h>
#include <boost/random.hpp>

class CustomerGenerationProcess
{
protected:
    boost::random::mt19937* myGenerator;
    boost::random::uniform_01<boost::random::mt19937>* myUniform;
    CustomerGenerationProcess(double,double);
    virtual ~CustomerGenerationProcess();
    double myRate;//In customers/hour
    static CustomerGenerationProcess* myInstance;
public:
    static CustomerGenerationProcess* instance(double=0,double=0);
    void setRate(double);
    void setSeed(double);
    uint64_t next() const;
};

#endif // CUSTOMERGENERATIONPROCESS_H
