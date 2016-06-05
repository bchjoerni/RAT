/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Copyright 2015 bchjoerni
*/

#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>

#include <utility>

#include "datapoint.h"

struct graphPoint
{
    QTime time;
    int height;
    QColor color;

    graphPoint( QTime eventTime, int depthOrVolume, QColor drawColor )
        : time( eventTime ), height( depthOrVolume ), color( drawColor )
    {
    }
};

class graph : public QWidget
{
    Q_OBJECT

public:
    explicit graph( QWidget *parent = 0 );
    ~graph();
    virtual void newPoint( QTime time, int value, int correct );
    void setDrawSpeed( unsigned int speed );
    unsigned int getDrawSpeed() const;
    void clearGraph();
    virtual void colorLastEvent( bool correct );

protected:
    void paintEvent( QPaintEvent* event );
    void resizeEvent( QResizeEvent* event );
    virtual void addPoint( QTime time, int value, int correct );
    virtual void setDrawVariables();
    virtual void drawBackground( QPainter *painter );
    virtual void drawEvent( QPainter* painter, unsigned int dataPointNum );    

    std::vector<graphPoint> _dataPoints;
    unsigned int _border;
    unsigned int _drawPosition;
    unsigned int _drawHeight[4];
    unsigned int _drawSpeed;
    unsigned int _drawWidth;
    unsigned int _bgColorPosition[3];
    unsigned int _bgColorHeight[3];
    unsigned int _bgColorWidth;

    const QColor COLOR_RIGHT      = Qt::green;
    const QColor COLOR_WRONG      = Qt::red;
    const QColor COLOR_SETWRONG   = Qt::darkRed;
    const QColor COLOR_DEFDRAW    = Qt::black;
    const QColor COLOR_LINE       = Qt::lightGray;
    const QColor COLOR_COMPHALF   = Qt::yellow;
    const QColor COLOR_BACKGROUND = Qt::white;
    const QColor COLOR_BGRIGHT    = QColor( 200, 255, 200 );
    const QColor COLOR_BGHALF     = QColor( 255, 255, 200 );
    const QColor COLOR_BGWRONG    = QColor( 255, 200, 200 );
};

#endif // GRAPH_H
