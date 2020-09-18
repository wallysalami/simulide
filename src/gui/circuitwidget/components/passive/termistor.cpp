/***************************************************************************
 *   Copyright (C) 2020 by santiago González                               *
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

#include "termistor.h"


Component* Termistor::construct( QObject* parent, QString type, QString id )
{ return new Termistor( parent, type, id ); }

LibraryItem* Termistor::libraryItem()
{
    return new LibraryItem(
            tr( "Termistor" ),
            tr( "Passive" ),
            "termistor.png",
            "Termistor",
            Termistor::construct);
}

Termistor::Termistor( QObject* parent, QString type, QString id )
         : VarResBase( parent, type, id  )
{
    m_area = QRectF( -11, -11, 22, 16 );

    setValLabelPos(-16, 30, 0);
    setShowVal( true );

    m_temp = 25;
    m_unit = "ºC";

    m_bVal = 3455;
    m_r25  = 10000;
    m_ptc  = false;

    m_dial->setMinimum(0);
    m_dial->setMaximum(200);
    m_dial->setValue( 25 );

    Component::setValue( 25 );
    eResistor::setResSafe( m_r25 );
}
Termistor::~Termistor(){}


void Termistor::updateStep()
{
    if( m_changed )
    {
        int val = m_dial->value();
        m_value = val;
        m_temp  = val;
        m_valLabel->setPlainText( QString::number(m_value)+" "+m_unit );

        double t0 = 25+273.15;    // Temp in Kelvin
        double t = m_temp+273.15;
        double e = 2.7182;
        double k = t*t0/(t-t0);

        double res = m_r25/pow( e, m_bVal/k );
        //double res = double( m_maxRes-(m_maxRes-m_minRes)*val/1000 );

        eResistor::setResSafe( res );
        m_changed = false;
        //qDebug()<<"Termistor::updateStep"<<m_resist;
    }
}

bool Termistor::ptc()
{
    return m_ptc;
}

void Termistor::setPtc( bool ptc )
{
    return; // TODO
    m_ptc = ptc;
    m_changed = true;
}

int Termistor::minTemp()
{
    return m_dial->minimum();
}

void Termistor::setMinTemp( int min )
{
    m_dial->setMinimum( min );
}

int Termistor::maxTemp()
{
    return m_dial->maximum();
}

void Termistor::setMaxTemp( int max )
{
    m_dial->setMaximum( max );
}

int Termistor::temp()
{
    return m_temp;
}

void Termistor::setTemp( int temp )
{
    int max = m_dial->maximum();
    int min = m_dial->minimum();

    if     ( temp > max ) temp = max;
    else if( temp < min ) temp = min;

    m_temp = temp;
    m_dial->setValue( temp );
    m_changed = true;
}

int Termistor::bVal()
{
    return m_bVal;
}

void Termistor::setBval( int bval )
{
    m_bVal = bval;
    m_changed = true;
}

int Termistor::r25()
{
    return m_r25;
}

void Termistor::setR25( int r25 )
{
    m_r25 = r25;
    m_changed = true;
}

void Termistor::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    if( m_hidden ) return;

    Component::paint( p, option, widget );

    p->drawRect( -10.5, -4, 21, 8 );
    
    p->drawLine(-8, 6,  6,-8 );
    p->drawLine( 6,-8, 10,-8 );
}

//#include "moc_ldr.cpp"
