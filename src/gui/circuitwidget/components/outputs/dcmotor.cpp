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

#include <cmath>

#include "dcmotor.h"
#include "simulator.h"
#include "pin.h"

static const char* DcMotor_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","RPM Nominal"),
    QT_TRANSLATE_NOOP("App::Property","Volt Nominal"),
};

Component* DcMotor::construct( QObject* parent, QString type, QString id )
{
    return new DcMotor( parent, type, id );
}

LibraryItem* DcMotor::libraryItem()
{
    return new LibraryItem(
        tr("Dc Motor"),
        tr("Outputs"),
        "dcmotor.png",
        "DcMotor",
        DcMotor::construct );
}

DcMotor::DcMotor( QObject* parent, QString type, QString id )
        : Component( parent, type, id )
        , eResistor( id.toStdString() )
{
    Q_UNUSED( DcMotor_properties );

    m_graphical = true;
    
    m_area = QRectF( -33,-33, 66, 66 );
    m_color = QColor( 50, 50, 70 );

    m_ang  = 0;
    m_voltNom = 5;

    setRpm( 60 );
    
    QString pinId = m_id;
    pinId.append(QString("-lPin"));
    QPoint pinPos = QPoint(-40,0);
    m_ePin[0] = new Pin( 180, pinPos, pinId, 0, this);

    pinId = m_id;
    pinId.append(QString("-rPin"));
    pinPos = QPoint(40,0);
    m_ePin[1] = new Pin( 0, pinPos, pinId, 1, this);

    m_idLabel->setPos(-12,-24);
    setLabelPos(-22,-48, 0);

    m_unit = "Ω";
    setResist( m_resist );
    
    Simulator::self()->addToSimuClockList( this );
    Simulator::self()->addToUpdateList( this );

    setShowId( true );
}

DcMotor::~DcMotor()
{
}

void DcMotor::stamp()
{
    eNode* enode = m_ePin[0]->getEnode();// Register for changes callback
    if( enode ) enode->addToChangedFast(this);

    enode = m_ePin[1]->getEnode();// Register for changes callback
    if( enode ) enode->addToChangedFast(this);
}

void DcMotor::updateStep()
{
    double stps = Simulator::self()->stepsPerus()*1e6;

    m_ang += m_motStPs*m_delta/stps;
    if( m_ang > 16*360 ) m_ang -= 16*360;
//qDebug() << "DcMotor::updateStep"<< m_delta << m_ang<<m_motStPs<<stps;
    m_delta = 0;
    update();
}

void DcMotor::simuClockStep()
{
    double volt = m_ePin[1]->getVolt() - m_ePin[0]->getVolt();
    m_speed = volt/m_voltNom;
    m_delta += m_speed;
}

int DcMotor::rpm()
{
    return m_rpm;
}

void DcMotor::setRpm( int rpm )
{
    if( rpm < 1 ) rpm = 1;
    m_rpm = rpm;
    m_motStPs = 16*360*rpm/60;

    update();
}

double DcMotor::volt()
{
    return m_voltNom;
}

void DcMotor::setVolt( double volt )
{
    m_voltNom = volt;
}

double DcMotor::resist()
{
    return m_value;
}

void DcMotor::setResist( double res )
{
    if( res < 1e-12 ) res = 1e-12;

    Component::setValue( res );       // Takes care about units multiplier
    eResistor::setResSafe( m_value*m_unitMult );
}

void DcMotor::setUnit( QString un )
{
    Component::setUnit( un );
    eResistor::setResSafe( m_value*m_unitMult );
}

void DcMotor::remove()
{
    if( m_ePin[0]->isConnected() ) (static_cast<Pin*>(m_ePin[0]))->connector()->remove();
    if( m_ePin[1]->isConnected() ) (static_cast<Pin*>(m_ePin[1]))->connector()->remove();
    
    Simulator::self()->remFromSimuClockList( this );
    Simulator::self()->remFromUpdateList( this ); 
    
    Component::remove();
}

void DcMotor::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    p->setBrush( QColor(50, 70, 100) );
    p->drawEllipse(-33,-33, 66, 66 );

    p->setPen( QColor(0, 0, 0) );
    p->setBrush( QColor(255, 255, 255) );
    p->drawEllipse(-28,-28, 56, 56 );

    // rotating pointer
    p->setPen ( QColor(0, 0, 0) );
    p->setBrush( QColor(50, 70, 100) );
    p->drawPie(-25,-25, 50, 50, m_ang-120, 240 );

    p->setBrush( QColor(50, 70, 100) );
    p->drawEllipse(-20,-20, 40, 40 );

    // Speed and Direction Indicator
    double speed = m_speed;
    double exedd = 0;
    if( m_speed > 1 )
    {
        exedd = m_speed -1;
        speed = 1;
    }
    else if( m_speed < -1 )
    {
        exedd = m_speed +1;
        speed = -1;
    }
    p->setPen ( QColor(50, 70, 100) );
    p->setBrush( QColor(100, 200, 70) );
    p->drawPie(-20,-20, 40, 40, 16*90, speed*16*180 );

    p->setBrush( QColor(200, 100, 70) );
    p->drawPie(-20,-20, 40, 40, -16*90, exedd*16*180 );
    //qDebug() << "DcMotor::paint"<< m_speed << speed << exedd;
}
#include "moc_dcmotor.cpp"
