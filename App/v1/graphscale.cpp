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


#include "graphscale.h"

graphScale::graphScale(QWidget *parent) : QWidget(parent), _drawSpeed( 9 )
{

}

graphScale::~graphScale()
{

}

void graphScale::setDrawSpeed( unsigned int drawSpeed )
{
    _drawSpeed = drawSpeed;
}

void graphScale::paintEvent( QPaintEvent* event )
{
    if( _drawSpeed > 0 )
    {
        int border = this->height()/4;
        int x2 = border + static_cast<int>( 1000.0/_drawSpeed );

        QPainter painter( this );
        painter.fillRect( 0, 0, x2 + border, this->height(), Qt::white );
        painter.setPen( Qt::black );
        painter.drawLine( border, border, border, this->height() - border );
        painter.drawLine( border, this->height()/2, x2, this->height()/2 );
        painter.drawLine( x2, border, x2, this->height() - border );
    }
}
