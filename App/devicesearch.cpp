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


#include "devicesearch.h"
#include "ui_devicesearch.h"

deviceSearch::deviceSearch( QWidget *parent ) :
    QDialog( parent ),
    _ui( new Ui::deviceSearch ), _demoCounter( 0 )
{
    _ui->setupUi( this );
    _ui->btn_connectToSelection->setEnabled( false );

    connect( _ui->btn_scan, SIGNAL( clicked() ), this, SLOT( startScan() ) );
    connect( _ui->btn_connectToSelection, SIGNAL( clicked() ), this,
             SLOT( connectToSelection() ) );
    connect( _ui->btn_connectToDefault, SIGNAL( clicked() ), this,
             SLOT( connectToDefault() ) );
    connect( _ui->lst_devices, SIGNAL( itemSelectionChanged() ), this,
             SLOT( deviceSelectionChanged() ) );
    connect( _ui->btn_titel, SIGNAL( clicked() ), this, SLOT( demoCheck() ) );
}

deviceSearch::~deviceSearch()
{
    delete _ui;
}

void deviceSearch::init( QString* btAdress, bluetooth* btDevice )
{
    _btAdress = btAdress;
    _btDevice = btDevice;

    connect( _btDevice, SIGNAL( deviceFound( QString ) ), this,
             SLOT( deviceFound( QString ) ) );
    connect( _btDevice, SIGNAL( scanFinished() ), this,
             SLOT( scanFinished() ) );

    if( _btDevice->valid() )
    {
        _ui->lbl_info->setText( "Drücke '" + _ui->btn_scan->text() + "' um "
                                "verfügbare Bluetooth-Geräte anzuzeigen." );
        _ui->btn_scan->setEnabled( true );
    }
    else
    {
        _ui->lbl_info->setText( "Bluetooth ist deaktiviert. Aktiviere "
                                "Bluetooth um nach Geräten suchen zu können!" );
        _ui->btn_scan->setEnabled( false );
    }
}

void deviceSearch::startScan()
{
    _ui->lst_devices->clear();

    if( _btDevice->valid() )
    {
        _btDevice->scan();
        _ui->lbl_info->setText( "Suche nach Bluetooth-Geräten ...\n"
                                "(Bitte etwas Geduld)" );
    }
    else
    {
        _ui->lbl_info->setText( "Bluetooth ist deaktiviert. Aktiviere "
                                "Bluetooth um nach Geräten suchen zu können!" );
        _ui->btn_scan->setEnabled( false );
    }
}

void deviceSearch::deviceFound( const QString& nameAndAdress )
{
    for( int i = 0; i < _ui->lst_devices->count(); i++ )
    {
        if( nameAndAdress == _ui->lst_devices->item( i )->text() )
        {
            return; // item already in list
        }
    }

    QListWidgetItem *item = new QListWidgetItem( nameAndAdress );

    if( nameAndAdress.mid( nameAndAdress.length() - DEFAULT_ADRESS.length() - 1,
                           DEFAULT_ADRESS.length() ) == DEFAULT_ADRESS )
    {
        item->setTextColor( QColor( Qt::green ) );
    }

    _ui->lst_devices->addItem( item );
}

void deviceSearch::scanFinished()
{    
    _ui->lbl_info->setText( "Suche beendet." );
}

void deviceSearch::deviceSelectionChanged()
{
    _ui->btn_connectToSelection->setEnabled( false );

    for( int i = 0; i < _ui->lst_devices->count(); i++ )
    {
        if( _ui->lst_devices->item( i )->isSelected() )
        {
            _ui->btn_connectToSelection->setEnabled( true );
            break;
        }
    }
}

void deviceSearch::connectToSelection()
{
    for( int i = 0; i < _ui->lst_devices->count(); i++ )
    {
        if( _ui->lst_devices->item( i )->isSelected() )
        {
            (*_btAdress) = _ui->lst_devices->item( i )->text().mid(
                        _ui->lst_devices->item( i )->text().length() -
                        DEFAULT_ADRESS.length() - 1, DEFAULT_ADRESS.length() );
            break;
        }
    }
    this->close();
}

void deviceSearch::connectToDefault()
{
    if( _demoCounter == 7 )
    {
        (*_btAdress) = "de:mo:de:mo:de:mo";
    }
    else
    {
        (*_btAdress) = DEFAULT_ADRESS;
    }
    this->close();
}

void deviceSearch::demoCheck()
{
    _demoCounter++;
    if( _demoCounter == 7 )
    {
        _ui->btn_connectToDefault->setText( "Demogerät wählen" );
    }
    else
    {
        _ui->btn_connectToDefault->setText( "Mit Standardgerät verbinden" );
    }
}
