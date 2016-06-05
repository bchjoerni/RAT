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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QString>
#include <QFile>
#include <QScroller>
#include <QScrollerProperties>
#include <QInputMethod>

struct settingsVar
{
    bool selectDevice;
    QString styleSheet;
    int drawSpeed;
    QString prefix;
    QString delimiter;
};

namespace Ui
{
    class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings( settingsVar* variables, QWidget *parent = 0 );
    ~settings();
    void setAppStyle( const QString& fileName );

private slots:
    void styleChanged( int index );
    void drawSpeedChanged( int value );
    void drawSpeedTextChanged( QString text );
    void drawSpeedTextChangeFinished();
    void apply();
    void cancel();

private:
    void setScrolling();
    void addStyleSheets();
    void setUiValues();
    bool getContentStringOfFile( const QString& fileName, QString& text );
    QString replaceInvalidFileNameChars( QString text );

    Ui::settings* _ui;
    settingsVar* _settings;
    bool *_apply;
};

#endif // SETTINGS_H
