#include "histogram.h"
#include <QChartView>

Histogram::Histogram(QWidget *parent) :
    QGraphicsView(parent)
{
    QChartView *view = new QChartView(this);
    QChart *chart = new QChart;

}

void Histogram::viewHistogram()
{

}

Histogram::~Histogram()
{
}
