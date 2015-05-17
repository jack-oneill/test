#ifndef UTILITIES_H
#define UTILITIES_H
#include <QPointF>
class Utilities
{
private:
    Utilities();
    ~Utilities();
public:
    static double dist(const double&,const double&, const double&,const double&);
    static double dist(const QPointF&, const QPointF&);
};

#endif // UTILITIES_H
