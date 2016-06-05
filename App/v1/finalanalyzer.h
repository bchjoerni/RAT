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

#ifndef FINALANALYZER_H
#define FINALANALYZER_H

#include "ratanalyzer.h"

#include <string>
#include <sstream>

class finalAnalyzer : public ratAnalyzer
{
    Q_OBJECT

public:
    finalAnalyzer( std::vector<dataPoint> *data );
    ~finalAnalyzer();    
    std::string getHtmlReport();
    void resetAll();

private:
    void updateFrequency() Q_DECL_OVERRIDE;
    void updateCompressionCount( bool correct ) Q_DECL_OVERRIDE;
    void updateVentilationCount( bool correct ) Q_DECL_OVERRIDE;
    void updateVentilationDuration( int duration ) Q_DECL_OVERRIDE;

    bool enoughData();
    void resetEvalVars();
    void resetHelperVars();
    void analyzeData();
    void analyzeNextDataPoint();
    void nextDataPoint();
    std::string getCprDuration();
    double getMinFrequency();
    double getMaxFrequency();
    double getAverageFrequency();
    double getStandardDeviationFrequency( double averageFrequency = -1 );

    std::string getHtmlReportText();
    std::string getHtmlLine( std::string text, int type = 0 );
    std::string getHtmlNewLine();
    std::string getPercentage( int fraction, int base );
    std::string toStdString( int number );
    std::string toStdString( double number );

    std::vector<double> _frequency;
    unsigned int _currentIndex;
    int _compTotal;
    int _compCorrect;
    int _compFalseTotal;
    int _compFalsePPoint;
    int _compFalseDepth;
    int _ventTotal;
    int _ventCorrect;
    int _ventFalseTotal;
    int _ventFalseVLow;
    int _ventFalseVHigh;
    int _ventFalseDuration;
};

#endif // FINALANALYZER_H
