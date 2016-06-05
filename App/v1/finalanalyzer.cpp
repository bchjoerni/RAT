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


#include "finalanalyzer.h"

finalAnalyzer::finalAnalyzer( std::vector<dataPoint> *data )
    : ratAnalyzer( data ), _currentIndex( 0 ), _compTotal( 0 ),
      _compCorrect( 0 ), _compFalseTotal( 0 ), _compFalsePPoint( 0 ),
      _compFalseDepth( 0 ), _ventTotal( 0 ), _ventCorrect( 0 ),
      _ventFalseTotal( 0 ), _ventFalseVLow( 0 ), _ventFalseVHigh( 0 ),
      _ventFalseDuration( 0 )
{
}

finalAnalyzer::~finalAnalyzer()
{
}

void finalAnalyzer::resetAll()
{
    resetAnalyzer();
    _compTotal         = 0;
    _compCorrect       = 0;
    _compFalseTotal    = 0;
    _compFalsePPoint   = 0;
    _compFalseDepth    = 0;
    _ventTotal         = 0;
    _ventCorrect       = 0;
    _ventFalseTotal    = 0;
    _ventFalseVLow     = 0;
    _ventFalseVHigh    = 0;
    _ventFalseDuration = 0;
}

bool finalAnalyzer::enoughData()
{
    if( _data->size() < 7 )
    {
        return false;
    }

    return true;
}

std::string finalAnalyzer::getHtmlReport()
{
    if( _data->size() < 7 )
    {
        return "Not enough data!";
    }

    analyzeData();
    return getHtmlReportText();
}

void finalAnalyzer::analyzeData()
{
    while( _currentIndex != _data->size()-2 )
    {
        analyzeNextDataPoint();
    }
}

void finalAnalyzer::analyzeNextDataPoint()
{
    nextDataPoint();
    analyzeCompression();
    analyzeVentilation();
}

void finalAnalyzer::nextDataPoint()
{
    _currentIndex++;
    _currentData = &((*_data)[_currentIndex]);
    _previousData = &((*_data)[_currentIndex-1]);
}

void finalAnalyzer::updateFrequency()
{
    _frequency.push_back( _frequencyNow );
}

void finalAnalyzer::updateCompressionCount( bool correct )
{
    if( correct )
    {
        _compCorrect++;
    }
    else
    {
        _compFalseTotal++;
        if( !_ppointOkay )
        {
            _compFalsePPoint++;
        }
        if( !_depthReached )
        {
            _compFalseDepth++;
        }
    }
    _compTotal++;
}

void finalAnalyzer::updateVentilationCount( bool correct )
{
    if( correct )
    {
        _ventCorrect++;
    }
    else
    {
        _ventFalseTotal++;
        if( _ventilationDuration >= _maxVentTimeMs )
        {
            _ventFalseDuration++;
        }
        if( !_enoughVolume )
        {
            _ventFalseVLow++;
        }
        if( _volumeTooHigh )
        {
            _ventFalseVHigh++;
        }
    }
    _ventTotal++;
}

void finalAnalyzer::updateVentilationDuration( int duration )
{
    // nothing to do
}

double finalAnalyzer::getMinFrequency()
{
    return *std::min_element( std::begin( _frequency ), std::end( _frequency ) );
}

double finalAnalyzer::getMaxFrequency()
{
    return *std::max_element( std::begin( _frequency ), std::end( _frequency ) );
}

double finalAnalyzer::getAverageFrequency()
{
    double sum = 0.0;

    for( unsigned int i = 0; i < _frequency.size(); i++ )
    {
        sum += _frequency[i];
    }

    return sum/_frequency.size();
}

double finalAnalyzer::getStandardDeviationFrequency( double averageFrequency )
{
    if( averageFrequency <= 0 )
    {
        averageFrequency = getAverageFrequency();
    }

    double standardDeviation = 0.0;

    for( unsigned int i = 0; i < _frequency.size(); i++ )
    {
        standardDeviation += std::pow( _frequency[i] - averageFrequency, 2.0 );
    }
    standardDeviation = std::sqrt( standardDeviation/(_frequency.size() - 1) );

    return standardDeviation;
}

std::string finalAnalyzer::getCprDuration()
{
    int secs = (*_data)[0].time.secsTo( (*_data)[_data->size()-1].time );

    std::string duration = "";
    if( secs/3600 > 0 )
    {
        duration = duration + toStdString( secs/3600 ) + "h ";
    }
    if( secs/60 > 0 )
    {
        duration = duration + toStdString( (secs/60)%60 ) + "min ";
    }
    duration = duration + toStdString( secs%60 ) + "s";

    return duration;
}

std::string finalAnalyzer::getHtmlReportText()
{
    std::string report = "<html>";
    report += getHtmlLine( "Ereignisbericht", 3 );
    report += getHtmlNewLine();
    report += getHtmlLine( "Wiederbelebungsdauer: " + getCprDuration(), 0 );
    report += getHtmlNewLine();
    report += getHtmlLine( "Herzdruckmassage:", 1 );
    report += getHtmlLine( "korrekt: " + toStdString( _compCorrect ) + " (" +
                           getPercentage( _compCorrect, _compTotal ) +
                           "%)", 0 );
    report += getHtmlLine( "falsch: " + toStdString( _compFalseTotal ) + " (" +
                           getPercentage( _compFalseTotal, _compTotal ) +
                           "%)", 0 );
    report += getHtmlLine( "falsch aufgrund des Druckpunktes: " +
                           toStdString( _compFalsePPoint ), 2 );
    report += getHtmlLine( "falsch aufgrund der Drucktiefe: " +
                           toStdString( _compFalseDepth ), 2 );
    report += getHtmlLine( "gesamt: " + toStdString( _compTotal ), 0 );
    report += getHtmlNewLine();
    report += getHtmlLine( "Frequenz:", 1 );
    report += getHtmlLine( "Durchschnitt: " + toStdString( static_cast<int>(
                               getAverageFrequency() ) ) + " / min", 0 );
    report += getHtmlLine( "niedrigste: " + toStdString( static_cast<int>(
                               getMinFrequency() ) ) + " / min", 0 );
    report += getHtmlLine( "hoechste: " + toStdString( static_cast<int>(
                               getMaxFrequency() ) ) + " / min", 0 );
    report += getHtmlLine( "Standardabweichung: " + toStdString(
                               static_cast<int>( getStandardDeviationFrequency()
                                                 *10 )/10.0 ) + " / min", 0 );
    report += getHtmlNewLine();
    report += getHtmlLine( "Beatmung:", 1 );
    report += getHtmlLine( "korrekt: " + toStdString( _ventCorrect) + " (" +
                           getPercentage( _ventCorrect, _ventTotal ) +
                           "%)", 0 );
    report += getHtmlLine( "falsch: " + toStdString( _ventFalseTotal) + " (" +
                           getPercentage( _ventFalseTotal, _ventTotal ) +
                           "%)", 0 );
    report += getHtmlLine( "falsch, Beatmungsvolumen zu gering: " +
                           toStdString( _ventFalseVLow ), 0 );
    report += getHtmlLine( "falsch, Beatmungsvolumen zu hoch: " +
                           toStdString( _ventFalseVHigh ), 0 );
    report += getHtmlLine( "falsch, Beatmungsdauer zu lang: " +
                           toStdString( _ventFalseDuration ), 0 );
    report += getHtmlLine( "gesamt: " + toStdString( _ventTotal ), 0 );
    report += "</html>";

    return report;
}

std::string finalAnalyzer::getHtmlLine( std::string text, int type )
{
    if( type == 1 )
    {
        return "<p align=\"center\"><u>" + text + "</u></p>";
    }
    else if( type == 2 )
    {
        return "<p align=\"center\"><i>" + text + "</i></p>";
    }
    else if( type == 3 )
    {
        return "<p align=\"center\"><b>" + text + "</b></p>";
    }
    return "<p align=\"center\">" + text + "</p>";
}

std::string finalAnalyzer::getHtmlNewLine()
{
    return "<br>";
}

std::string finalAnalyzer::getPercentage( int fraction, int base )
{
    if( base <= 1 )
    {
        return "-";
    }

    if( (fraction*100000)/base - (fraction*10000/base)*10 >= 5 )
    {
        return toStdString( (fraction*10000/base + 1)/100.0 );
    }

    return toStdString( (fraction*10000/base)/100.0 );
}

std::string finalAnalyzer::toStdString( int number )
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string finalAnalyzer::toStdString( double number )
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}
