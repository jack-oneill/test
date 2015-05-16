#include "statistics.h"
#include "../scoreboard.h"
#include <QGridLayout>


StatWidget::StatWidget(QWidget* parent) : QGroupBox(parent)
{
    QGridLayout*  layout = new QGridLayout(this);
    setLayout(layout);
    setTitle("Default title");
    myServiced=new QLabel(parent);
    myServiced->setText("Serviced 0");
    myServiced->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    myDropped=new QLabel(parent);
    myDropped->setText("Dropped 0");
    myDropped->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    myResult=new QLabel(parent);
    myResult->setText("Empty");
    myResult->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(myServiced,0,0,1,3);
    layout->addWidget(myDropped,0,3,1,3);
    layout->addWidget(myResult,1,0,12,6);
    myIndex=0;
}

void StatWidget::setIndex(const unsigned& ind)
{
   myIndex=ind;
   if(mySheet)
   {
       this->setTitle(mySheet->name()+" "+QString::number(myIndex));
   }
}

void StatWidget::setSheet(ScoreSheet* sheet)
{
    mySheet=sheet;
    if(mySheet)
    {
        connect(mySheet,SIGNAL(updated()),this,SLOT(updateMe()));
       this->setTitle(mySheet->name()+" "+QString::number(myIndex));

    }

}

void StatWidget::updateMe()
{
    if(mySheet)
    {
       myResult->setText(mySheet->toString());
       myDropped->setText("Dropped "+QString::number(mySheet->giveUps()));
       myServiced->setText("Serviced "+QString::number(mySheet->serviced()));
    }
}

Statistics::Statistics(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout*  layout = new QGridLayout(this);
    setLayout(layout);
    connect(ScoreBoard::instance(),SIGNAL(newSheet(ScoreSheet*)),this,SLOT(updateMe(ScoreSheet*)));
}
void Statistics::updateMe(ScoreSheet* sheet)
{
    myWidgets.append(new StatWidget(this));
    myWidgets.back()->moveToThread(this->thread());
    ((QGridLayout*) layout())->addWidget(myWidgets.back(),myWidgets.size()-1,0,1,1);
    myWidgets.back()->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    myWidgets.back()->setSheet(sheet);
    myWidgets.back()->setIndex(myWidgets.size()-1);
    myWidgets.back()->updateMe();
}
