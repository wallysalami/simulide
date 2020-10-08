/***************************************************************************
 *   Copyright (C) 2019 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include "ky023.h"
#include "pin.h"
#include "simulator.h"
#include "circuit.h"

#define WIDTH 56
#define HEIGHT 72
#define GAP 4
#define JOYSTICK_SIZE 50

#define VIN 5

Component* KY023::construct( QObject* parent, QString type, QString id )
{ return new KY023( parent, type, id ); }

LibraryItem* KY023::libraryItem()
{
    return new LibraryItem(
            tr( "KY-023" ),
            tr( "Sensors" ),
            "sr04.png",
            "KY023",
            KY023::construct);
}

KY023::KY023( QObject* parent, QString type, QString id )
    : Component( parent, type, id )
    , eElement( id.toStdString() )
{
    m_joystickW.setupWidget();
    m_joystickW.setFixedSize( JOYSTICK_SIZE, JOYSTICK_SIZE );
    onvaluechanged(500, 500);
    
    m_proxy = Circuit::self()->addWidget( &m_joystickW );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-WIDTH/2 + (WIDTH - JOYSTICK_SIZE) / 2, -HEIGHT/2 + 2*GAP) );
    
    m_button = new QToolButton();
    m_button->setMaximumSize( 16,16 );
    m_button->setGeometry(-20,-16,16,16);
    
    m_proxy_button = Circuit::self()->addWidget( m_button );
    m_proxy_button->setParentItem( this );
    m_proxy_button->setPos( QPoint(WIDTH/2 + 5, -8) );
    
    m_area = QRect( -WIDTH/2, -HEIGHT/2 + GAP, WIDTH, HEIGHT );
    setLabelPos(-WIDTH/2, -HEIGHT/2 - GAP, 0);
    
    m_pin.resize(5);
    
    QString pinid = id;
    pinid.append(QString("-vccpin"));
    QPoint pinpos = QPoint(-8,48);
    Pin* vccPin = new Pin( 270, pinpos, pinid, 0, this);
    vccPin->setLabelText( " Vcc" );
    vccPin->setUnused( true );
    m_pin[0] = vccPin;
    
    pinid = id;
    pinid.append(QString("-gndpin"));
    pinpos = QPoint(-16,48);
    Pin* gndPin = new Pin( 270, pinpos, pinid, 0, this);
    gndPin->setLabelText( " Gnd" );
    gndPin->setUnused( true );
    m_pin[1] = gndPin;
    
    pinid = id;
    pinid.append(QString("-vrx"));
    pinpos = QPoint(0,48);
    m_vrxpin = new Pin( 270, pinpos, pinid, 0, this);
    m_vrxpin->setLabelText( " VRX" );
    m_pin[2] = m_vrxpin;
    
    pinid.append(QString("-eSource"));
    m_vrx = new eSource( pinid.toStdString(), m_vrxpin );
    m_vrx->setVoltHigh( VIN );
    m_vrx->setImp( 40 );
    
    pinid = id;
    pinid.append(QString("-vry"));
    pinpos = QPoint(8,48);
    m_vrypin = new Pin( 270, pinpos, pinid, 0, this);
    m_vrypin->setLabelText( " VRY" );
    m_pin[3] = m_vrypin;

    pinid.append(QString("-eSource"));
    m_vry = new eSource( pinid.toStdString(), m_vrypin );
    m_vry->setVoltHigh( VIN );
    m_vry->setImp( 40 );
    
    pinid = id;
    pinid.append(QString("-sw"));
    pinpos = QPoint(16,48);
    m_swpin = new Pin( 270, pinpos, pinid, 0, this);
    m_swpin->setLabelText( " SW" );
    m_pin[4] = m_swpin;

    pinid.append(QString("-eSource"));
    m_sw = new eSource( pinid.toStdString(), m_swpin );
    m_sw->setVoltHigh( VIN );
    m_sw->setImp( 40 );
    
    Simulator::self()->addToUpdateList( this );
    
    connect( &m_joystickW, SIGNAL( valueChanged(int, int) ),
             this,        SLOT  ( onvaluechanged(int, int) ));
    
    connect( m_button, SIGNAL( pressed() ),
             this,     SLOT  ( onbuttonpressed() ));
    
    connect( m_button, SIGNAL( released() ),
             this,     SLOT  ( onbuttonreleased() ));
    
    resetState();
}
KY023::~KY023(){ Simulator::self()->remFromSimuClockList( this ); }

void KY023::onbuttonpressed()
{
    m_closed = true;
    m_changed = true;
    
    update();
}

void KY023::onbuttonreleased()
{
    m_closed = false;
    m_changed = true;
    
    update();
}

void KY023::onvaluechanged(int xValue, int yValue)
{
    m_vOutX = double(VIN * xValue) / 1000;
    m_vOutY = double(VIN * (1000 - yValue)) / 1000;
    
    m_changed = true;
}

void KY023::updateStep()
{
    if ( m_changed ) {
        m_vrx->setVoltHigh( m_vOutX );
        m_vrx->setOut( true );
        m_vrx->stampOutput();
        
        m_vry->setVoltHigh( m_vOutY );
        m_vry->setOut( true );
        m_vry->stampOutput();
        
        m_changed = false;
    }
}

void KY023::remove()
{
    delete m_vrx;
    delete m_vry;
    Component::remove();
}

void KY023::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    Component::paint( p, option, widget );
    
    p->setBrush(Qt::black);
    p->drawRoundedRect( m_area, 2, 2 );
}

#include "moc_ky023.cpp"
