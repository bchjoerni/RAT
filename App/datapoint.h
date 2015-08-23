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

#ifndef DATAPOINT
#define DATAPOINT

#include <QTime>

#include <utility>

struct dataPoint
{
    QTime time;       // time when dataPoint was recorded
    int compPosition; // compression position, 0 = correct
    int compDepth;    // compression Depth 0-3
    int ventVolume;   // ventilation volume 0-3

    dataPoint() : time( QTime::currentTime() ), compPosition( -1 ),
        compDepth( -1 ), ventVolume( -1 )
    {
    }

    dataPoint( const dataPoint& dp ): time( dp.time ),
        compPosition( dp.compPosition ), compDepth( dp.compDepth ),
        ventVolume( dp.ventVolume )
    {}

    dataPoint( dataPoint&& dp ) noexcept : time( std::move( dp.time ) ),
        compPosition( std::move( dp.compPosition ) ),
        compDepth( std::move( dp.compDepth) ),
        ventVolume( std::move( dp.ventVolume) )
    {}
};

#endif // DATAPOINT

