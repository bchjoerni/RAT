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


#include "settings.h"
#include "ui_settings.h"

settings::settings( settingsVar* variables, QWidget *parent ) :
    QDialog( parent ),
    _ui( new Ui::settings ),
    _settings( variables )
{
    _ui->setupUi(this);

    addStyleSheets();
    setUiValues();
    setScrolling();

    connect( _ui->bar_drawSpeed, SIGNAL( valueChanged( int ) ), this,
             SLOT( drawSpeedChanged( int ) ) );
    connect( _ui->cob_stylesheets, SIGNAL( currentIndexChanged( int ) ), this,
             SLOT( styleChanged( int ) ) );
    connect( _ui->txt_drawSpeed, SIGNAL( textChanged( QString ) ), this,
             SLOT( drawSpeedTextChanged( QString ) ) );
    connect( _ui->txt_drawSpeed, SIGNAL( editingFinished() ), this,
             SLOT( drawSpeedTextChangeFinished() ) );
    connect( _ui->btn_apply, SIGNAL( clicked() ), this, SLOT( apply() ) );
    connect( _ui->btn_cancel, SIGNAL( clicked() ), this, SLOT( cancel() ) );
}

settings::~settings()
{
    delete _ui;
}

void settings::setScrolling()
{
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
    QScroller* scroller = QScroller::scroller( _ui->scrollArea );
    scroller->grabGesture( _ui->scrollArea, QScroller::LeftMouseButtonGesture );
    scroller->setScrollerProperties( scrollerProperties );
}

void settings::apply()
{
    (*_settings).selectDevice = _ui->chb_selectDevice->isChecked();
    (*_settings).styleSheet = _ui->cob_stylesheets->itemText(
                _ui->cob_stylesheets->currentIndex() );
    (*_settings).drawSpeed = _ui->bar_drawSpeed->maximum() -
            _ui->bar_drawSpeed->value() + _ui->bar_drawSpeed->minimum();
    (*_settings).prefix = replaceInvalidFileNameChars(
                _ui->txt_prefix->text() );

    if( _ui->rad_semicolon->isChecked() )
    {
        (*_settings).delimiter = ";";
    }
    else if( _ui->rad_tabstop->isChecked() )
    {
        (*_settings).delimiter = "\t";
    }

    this->close();
}

QString settings::replaceInvalidFileNameChars( QString text )
{
    text.replace("/", "");
    text.replace("\\", "");
    text.replace(":", "");
    text.replace("*", "");
    text.replace("?", "");
    text.replace("\"", "");
    text.replace("<", "");
    text.replace(">", "");
    text.replace("|", "");
    return text;
}

void settings::cancel()
{
    setAppStyle( (*_settings).styleSheet );
    this->close();
}

void settings::addStyleSheets()
{
    _ui->cob_stylesheets->addItem( "standard" );
    _ui->cob_stylesheets->addItem( "black" );
    _ui->cob_stylesheets->addItem( "darkorange" );
    _ui->cob_stylesheets->addItem( "fire" );
    _ui->cob_stylesheets->addItem( "metal" );
    _ui->cob_stylesheets->addItem( "sandy" );
}

void settings::setUiValues()
{
    for( int i = 0; i < _ui->cob_stylesheets->count(); i++ )
    {
        if( _ui->cob_stylesheets->itemText( i ) == (*_settings).styleSheet )
        {
            _ui->cob_stylesheets->setCurrentIndex( i );
            break;
        }
    }

    _ui->bar_drawSpeed->setValue( _ui->bar_drawSpeed->maximum() -
                                  (*_settings).drawSpeed +
                                  _ui->bar_drawSpeed->minimum() );
    _ui->txt_drawSpeed->setText( QString::number(
                                     _ui->bar_drawSpeed->value() ) );
    _ui->txt_prefix->setText( (*_settings).prefix );

    if( (*_settings).delimiter == ";" )
    {
        _ui->rad_semicolon->setChecked( true );
        _ui->rad_tabstop->setChecked( false );
    }
    else if( (*_settings).delimiter == "\t" )
    {
        _ui->rad_semicolon->setChecked( false );
        _ui->rad_tabstop->setChecked( true );
    }
    else
    {
        _ui->rad_semicolon->setChecked( false );
        _ui->rad_tabstop->setChecked( false );
    }
}

void settings::drawSpeedChanged( int value )
{
    _ui->txt_drawSpeed->setText( QString::number( value) );
}


void settings::drawSpeedTextChanged( QString text )
{
    bool conversionSuccessful = false;
    text.toInt( &conversionSuccessful );

    if( !conversionSuccessful )
    {
        int settingsValue = _ui->bar_drawSpeed->maximum() -
                (*_settings).drawSpeed + _ui->bar_drawSpeed->minimum();
        _ui->txt_drawSpeed->setText( QString::number( settingsValue ) );
        _ui->bar_drawSpeed->setValue( settingsValue );
    }
}

void settings::drawSpeedTextChangeFinished()
{
    bool conversionSuccessful = false;
    int value = _ui->txt_drawSpeed->text().toInt( &conversionSuccessful );
    if( conversionSuccessful )
    {
        if( value <= _ui->bar_drawSpeed->maximum()
                && value >= _ui->bar_drawSpeed->minimum() )
        {
            _ui->bar_drawSpeed->setValue( value );
        }
        else
        {
            _ui->bar_drawSpeed->setValue( _ui->bar_drawSpeed->maximum() -
                                          (*_settings).drawSpeed +
                                          _ui->bar_drawSpeed->minimum() );
        }
    }
    else
    {
        int settingsValue = _ui->bar_drawSpeed->maximum() -
                (*_settings).drawSpeed + _ui->bar_drawSpeed->minimum();
        _ui->txt_drawSpeed->setText( QString::number( settingsValue ) );
        _ui->bar_drawSpeed->setValue( settingsValue );
    }
}

void settings::styleChanged( int index )
{
    setAppStyle( _ui->cob_stylesheets->itemText( index ) );
}

void settings::setAppStyle( const QString& fileName )
{
    QString style;
    if( getContentStringOfFile( ":/styles/stylesheets/" + fileName+ ".qss",
                                style ) )
    {
        qApp->setStyleSheet( style );
    }
}

bool settings::getContentStringOfFile( const QString& fileName,
                                              QString& text )
{
    text = "";
    QFile file( fileName );
    if( file.open( QFile::ReadOnly ) )
    {
        text = QString::fromUtf8( file.readAll() );
        file.close();
        return true;
    }
    return false;
}
