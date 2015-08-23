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


#include "graphventilation.h"


graphVentilation::graphVentilation( QWidget *parent ) : graph( parent )
{
}

void graphVentilation::setDrawVariables()
{
    _border = this->width()/125;
    _bgColorWidth = this->width() - 2*_border;
    int effectiveHeight = this->height() - 2*_border;

    _drawHeight[0] = effectiveHeight/20;
    _drawHeight[1] = effectiveHeight*4/10;
    _drawHeight[2] = effectiveHeight*7/10;
    _drawHeight[3] = effectiveHeight*9/10;

    _bgColorPosition[0] = this->height() - _border -
            (_drawHeight[1] + _drawHeight[2])/2;
    _bgColorHeight[0] = this->height() - _border - _drawHeight[0] -
            _bgColorPosition[0];
    _bgColorPosition[1] = this->height() - _border -
            (_drawHeight[3] + _drawHeight[2])/2;
    _bgColorHeight[1] = _bgColorPosition[0] - _bgColorPosition[1];
    _bgColorPosition[2] = _border;
    _bgColorHeight[2] = _bgColorPosition[1] - _border;
}

void graphVentilation::drawEvent( QPainter* painter, unsigned int dataPointNum )
{
    painter->fillRect( _drawPosition, this->height() - _border -
            _drawHeight[_dataPoints[dataPointNum].height], _drawWidth,
            _drawHeight[_dataPoints[dataPointNum].height],
            _dataPoints[dataPointNum].color );
}

void graphVentilation::drawBackground( QPainter* painter )
{
    painter->fillRect( 0, 0, this->width(), this->height(), COLOR_BACKGROUND );
    painter->setPen( COLOR_LINE );
    painter->drawLine( _border, this->height() - _border,
                       this->width() - 2*_border, this->height() - _border );

    painter->fillRect( _border, _bgColorPosition[0], _bgColorWidth,
            _bgColorHeight[0], COLOR_BGWRONG );

    painter->fillRect( _border, _bgColorPosition[1], _bgColorWidth,
            _bgColorHeight[1], COLOR_BGRIGHT );

    painter->fillRect( _border, _bgColorPosition[2], _bgColorWidth,
            _bgColorHeight[2], COLOR_BGWRONG );
}
