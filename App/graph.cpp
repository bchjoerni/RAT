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


#include "graph.h"


graph::graph( QWidget *parent ) : QWidget( parent )
{
    setDrawVariables();
    _drawPosition = _border;
}

graph::~graph()
{
    _dataPoints.clear();
}

void graph::setDrawVariables()
{
    _border = this->width()/125;
    int effectiveHeight = this->height() - 2*_border;

    _drawHeight[3] = effectiveHeight*1/10;
    _drawHeight[2] = effectiveHeight*5/10;
    _drawHeight[1] = effectiveHeight*9/10;
    _drawHeight[0] = effectiveHeight - 2;
}

void graph::resizeEvent( QResizeEvent *event )
{
    setDrawVariables();
    _dataPoints.clear();
    _drawPosition = _border;
}

void graph::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    drawBackground( &painter );

    if( _dataPoints.size() > 1 )
    {
        _drawPosition = _border;

        for( unsigned int i = 0; i < _dataPoints.size() - 1; i++ )
        {
            _drawWidth = _dataPoints[i].time.msecsTo( _dataPoints[i+1].time )/
                    _drawSpeed + 1;
            drawEvent( &painter, i );
            _drawPosition += _drawWidth;
        }
    }
}

void graph::drawBackground( QPainter *painter )
{
     painter->fillRect( 0, 0, this->geometry().width(),
                       this->geometry().height(), COLOR_BACKGROUND );
}

void graph::drawEvent( QPainter* painter, unsigned int dataPointNum )
{
    painter->fillRect( _drawPosition, _border, _drawWidth,
         _drawHeight[_dataPoints[dataPointNum].height]
            - _border, _dataPoints[dataPointNum].color );
}

void graph::setDrawSpeed( unsigned int speed )
{
    _drawSpeed = speed;
}

unsigned int graph::getDrawSpeed() const
{
    return _drawSpeed;
}

void graph::newPoint( QTime time, int value, int correct )
{
    if( _dataPoints.size() > 0 )
    {
        _drawWidth = _dataPoints[_dataPoints.size()-1].time.msecsTo( time )/
                _drawSpeed + 1;
        if( (_drawPosition + _drawWidth) > (this->width() - _border) )
        {
            _dataPoints.clear();
            _drawPosition = _border;
        }
    }

    addPoint( time, value, correct );

    update();
}

void graph::addPoint( QTime time, int value, int correct )
{
    QColor compColor = COLOR_DEFDRAW;
    if( correct != 0 )
    {
        compColor = COLOR_SETWRONG;
    }

    _dataPoints.push_back( graphPoint( time, value, compColor ) );
}

void graph::clearGraph()
{
    _dataPoints.clear();
    update();
}

void graph::colorLastEvent( bool correct )
{
    bool nonZero = false;
    int numPoints = _dataPoints.size();
    for( int i = numPoints-1; i >= 0; i-- )
    {
        if( _dataPoints[i].height == 0 )
        {
            if( nonZero )
            {
                break;
            }
        }
        else
        {
            nonZero = true;
        }
        if( _dataPoints[i].color != COLOR_SETWRONG )
        {
            if( correct )
            {
                _dataPoints[i].color = COLOR_RIGHT;
            }
            else
            {
                _dataPoints[i].color = COLOR_WRONG;
            }
        }
    }
}
