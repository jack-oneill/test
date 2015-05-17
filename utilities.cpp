#include "utilities.h"
#include <cmath>
using namespace std;
Utilities::Utilities()
{
}
Utilities::~Utilities()
{
}

double Utilities::dist(const double& x0,const double& y0, const double& x1,const double& y1)
{
    double R = 6371000; // metres
    double phi1 = x0*3.14/180;
    double phi2 = x1*3.14/180;
    double dPhi= phi2-phi1;
    double dL= (y0-y1)*3.14/180;
    double a = sin(dPhi/2)*sin(dPhi/2)+cos(phi1)*cos(phi2)*sin(dL/2)*sin(dL/2);
    double c = 2 * std::atan2(sqrt(a), sqrt(1-a));
    double d = R * c;
    return d;
}

double Utilities::dist(const QPointF& a, const QPointF& b)
{
   return dist(a.x(),a.y(),b.x(),b.y());
}
