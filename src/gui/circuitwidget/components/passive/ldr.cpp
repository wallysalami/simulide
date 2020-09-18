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

#include "ldr.h"


Component* Ldr::construct( QObject* parent, QString type, QString id )
{ return new Ldr( parent, type, id ); }

LibraryItem* Ldr::libraryItem()
{
    return new LibraryItem(
            tr( "LDR" ),
            tr( "Passive" ),
            "ldr.png",
            "LDR",
            Ldr::construct);
}

Ldr::Ldr( QObject* parent, QString type, QString id )
   : VarResBase( parent, type, id  )
{
    m_area = QRectF( -11, -11, 22, 16 );

    setValLabelPos(-16, 30, 0);
    setShowVal( true );

    m_lux = 1;
    m_unit = "Lux";

    m_r1 = 127410;
    m_gamma = 0.8582;

    m_dial->setMinimum(1);
    m_dial->setMaximum(1000);

    Component::setValue( 1 );
    eResistor::setResSafe( m_r1 );
}
Ldr::~Ldr(){}

void Ldr::updateStep()
{
    if( m_changed )
    {
        int val = m_dial->value();
        m_value = val;
        m_lux   = val;
        m_valLabel->setPlainText( QString::number(m_value)+" "+m_unit );

        //double res = double( m_maxRes-(m_maxRes-m_minRes)*val/1000 );
        double res = double(m_r1)*pow( double(val), -m_gamma );

        eResistor::setResSafe( res );
        m_changed = false;
        //qDebug()<<"Ldr::updateStep"<<m_resist;
    }
}

int Ldr::minLux()
{
    return m_dial->minimum();
}

void Ldr::setMinLux( int min )
{
    m_dial->setMinimum( min );
}

int Ldr::maxLux()
{
    return m_dial->maximum();
}

void Ldr::setMaxLux( int max )
{
    m_dial->setMaximum( max );
}

int Ldr::lux()
{
    return m_lux;
}

void Ldr::setLux( int lux )
{
    int max = m_dial->maximum();
    int min = m_dial->minimum();

    if     ( lux > max ) lux = max;
    else if( lux < min ) lux = min;

    m_lux = lux;
    m_dial->setValue( lux );
    m_changed = true;
}

int Ldr::r1()
{
    return m_r1;
}

void Ldr::setR1( int r1 )
{
    m_r1 = r1;
    m_changed = true;
}

double Ldr::gamma()
{
    return m_gamma;
}

void Ldr::setGamma( double ga )
{
    m_gamma = ga;
    m_changed = true;
}

void Ldr::paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    if( m_hidden ) return;

    Component::paint( p, option, widget );

    p->drawRect( -10.5, -4, 21, 8 );
    
    p->drawLine(-5,-11,-1,-7 );
    p->drawLine(-1, -7,-1,-9 );
    p->drawLine(-1, -7,-3,-7 );

    p->drawLine( 1,-11, 5,-7 );
    p->drawLine( 5, -7, 5,-9 );
    p->drawLine( 5, -7, 3,-7 );
}

//#include "moc_ldr.cpp"
