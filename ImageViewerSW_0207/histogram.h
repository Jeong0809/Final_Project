#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QGraphicsView>

namespace Ui {
class Histogram;
}

class Histogram : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Histogram(QWidget *parent = nullptr);
    ~Histogram();

private:
    void viewHistogram();
};

#endif // HISTOGRAM_H
