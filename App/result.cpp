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


#include "result.h"
#include "ui_result.h"

result::result( QWidget *parent ) :
    QDialog( parent ),
    _ui( new Ui::result )
{
    _ui->setupUi( this );

    connect( _ui->btn_save, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect( _ui->btn_close, SIGNAL( clicked() ), this, SLOT( closeReport() ) );

    QScrollerProperties scrollerProperties;
    scrollerProperties.setScrollMetric(
                QScrollerProperties::MaximumClickThroughVelocity, 0 );
    QVariant overshootPolicy = QVariant::fromValue<
            QScrollerProperties::OvershootPolicy>(
                    QScrollerProperties::OvershootAlwaysOff );
    scrollerProperties.setScrollMetric(
            QScrollerProperties::VerticalOvershootPolicy, overshootPolicy );
    scrollerProperties.setScrollMetric(
            QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy );
    QScroller* scroller = QScroller::scroller( _ui->txt_result );
    scroller->grabGesture( _ui->txt_result, QScroller::LeftMouseButtonGesture );
    scroller->setScrollerProperties( scrollerProperties );
}

result::~result()
{
    delete _ui;
}

void result::setReportHtml( const std::string &text )
{
    _ui->txt_result->setHtml( QString::fromStdString( text ) );
}

void result::save()
{
    QString path = getSavePath();
    if( path.isEmpty() )
    {
        return;
    }

    QString prefix = _ui->txt_prefixReport->text();
    prefix.replace("/", "");
    prefix.replace("\\", "");
    prefix.replace(":", "");
    prefix.replace("*", "");
    prefix.replace("?", "");
    prefix.replace("\"", "");
    prefix.replace("<", "");
    prefix.replace(">", "");
    prefix.replace("|", "");

    QString filename = path + "/" + prefix
            + QDateTime::currentDateTime().toString( "yyyy-MM-dd__hh-mm-ss" )
            + ".html";
    QFile file( filename );
    if( file.open( QIODevice::ReadWrite ) )
    {
        QTextStream stream( &file );
        stream << _ui->txt_result->toHtml();
        _ui->lbl_info->setText( "Daten gespeichert (" + filename + ")." );
    }
    else
    {
        _ui->lbl_info->setText( "Fehler: Daten konnten nicht in " + path +
                                " gespeichert werden!" );
    }
}

QString result::getSavePath()
{
    // QStandardPaths did not work ...
    QString path = qgetenv( "EXTERNAL_STORAGE" );
    if( path.isEmpty() )
    {
        _ui->lbl_info->setText( "Fehler: Pfad konnte nicht bestimmt werden!" );
        return path;
    }

    path += "/rat";

    QDir dir( path );
    if( !dir.exists() )
    {
        if( !dir.mkpath( path ) )
        {
            _ui->lbl_info->setText( "Fehler: Dateiordner konnte nicht erstellt "
                                    "werden!" );
            return "";
        }
    }

    return path;
}

void result::closeReport()
{
    this->close();
}
