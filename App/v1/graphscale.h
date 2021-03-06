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

#ifndef GRAPHSCALE_H
#define GRAPHSCALE_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class graphScale : public QWidget
{
    Q_OBJECT

public:
    explicit graphScale(QWidget *parent = 0);
    ~graphScale();
    void setDrawSpeed( unsigned int drawSpeed );

protected:
    void paintEvent( QPaintEvent* event );

private:
    unsigned int _drawSpeed;
};

#endif // GRAPHSCALE_H
