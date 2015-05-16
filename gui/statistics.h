#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
class ScoreSheet;

class StatWidget : public QGroupBox
{
    Q_OBJECT
    QLabel* myDropped;
    QLabel* myServiced;
    QLabel* myResult;
    ScoreSheet* mySheet;
    unsigned myIndex;
public:
    explicit StatWidget(QWidget* parent=0);
    void setIndex(const unsigned&);
    void setSheet(ScoreSheet*);
public slots:
    void updateMe();
};

class Statistics : public QWidget
{
    Q_OBJECT
    QList<StatWidget*> myWidgets;
public:
    explicit Statistics(QWidget *parent = 0);
signals:
public slots:
    void updateMe(ScoreSheet*);

};

#endif // STATISTICS_H
