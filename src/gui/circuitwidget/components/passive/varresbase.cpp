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

#include "varresbase.h"
#include "circuit.h"
//#include "connector.h"
#include "pin.h"

static const char* VarResBase_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Max Resistance"),
    QT_TRANSLATE_NOOP("App::Property","Min Resistance")
};


VarResBase::VarResBase( QObject* parent, QString type, QString id )
            : Component( parent, type, id )
            , eResistor( id.toStdString() )
{
    Q_UNUSED( VarResBase_properties );

    m_graphical = true;

    m_area = QRectF( -11, -6, 22, 9 );
    
    m_pin.resize( 2 );
    QString pinId = m_id;
    pinId.append(QString("-lPin"));
    QPoint pinPos = QPoint(-8-8,0);
    m_pin[0] = new Pin( 180, pinPos, pinId, 0, this);
    m_ePin[0] = m_pin[0];

    pinId = m_id;
    pinId.append(QString("-rPin"));
    pinPos = QPoint(8+8,0);
    m_pin[1] = new Pin( 0, pinPos, pinId, 1, this);
    m_ePin[1] = m_pin[1];

    m_dialW.setupWidget();
    m_dialW.setFixedSize( 24, 24 );

    m_dial = m_dialW.dial;
    m_dial->setMinimum(0);
    m_dial->setMaximum(1000);
    m_dial->setSingleStep(25);

    m_proxy = Circuit::self()->addWidget( &m_dialW );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint( -12, 7) );

    m_idLabel->setPos(-12,-24);
    setLabelPos(-16,-40, 0);

    setValLabelPos( 10,-20, 0);
    setShowVal( true );

    setMinRes( 0 );
    setMaxRes( 10000000 );

    Simulator::self()->addToUpdateList( this );

    connect( m_dial, SIGNAL(valueChanged(int)),
             this,   SLOT  (resChanged(int)) );
}
VarResBase::~VarResBase(){}

void VarResBase::initialize()
{
    m_changed = true;
}

double VarResBase::maxRes() { return m_maxRes; }

void VarResBase::setMaxRes( double r )
{
    m_maxRes = r;
}

double VarResBase::minRes() { return m_minRes; }

void VarResBase::setMinRes( double r )
{
    if( r < 0.000001 ) r = 0.000001;

    m_minRes = r;
}

void VarResBase::resChanged( int res ) // Called when dial is rotated
{
    //qDebug() << res << m_resist;
    m_changed = true;
}

void VarResBase::setVal( int val )
{
    if     ( val < m_minRes ) val = m_minRes;
    else if( val > m_maxRes ) val = m_maxRes;

    m_dial->setValue( val*1000/(m_maxRes-m_minRes) );
    //resChanged( val );
}

int VarResBase::val()
{
    return eResistor::m_resist;
}

void VarResBase::remove()
{
    if( m_ePin[0]->isConnected() ) (static_cast<Pin*>(m_ePin[0]))->connector()->remove();
    if( m_ePin[1]->isConnected() ) (static_cast<Pin*>(m_ePin[1]))->connector()->remove();
    Component::remove();
}

#include "moc_varresbase.cpp"
