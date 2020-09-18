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

#include "tunnel.h"
#include "circuit.h"
#include "pin.h"

QHash<QString, eNode*> Tunnel::m_nodes;
QHash<QString, int>    Tunnel::m_count;

Component* Tunnel::construct( QObject* parent, QString type, QString id )
{ return new Tunnel( parent, type, id ); }

LibraryItem* Tunnel::libraryItem()
{
    return new LibraryItem(
        tr( "Tunnel." ),
        tr( "Other" ),
        "tunnel.png",
        "Tunnel",
        Tunnel::construct );
}

Tunnel::Tunnel( QObject* parent, QString type, QString id )
      : Component( parent, type, id )
      , eElement( id.toStdString() )
{
    setLabelPos(-64,-24 );

    m_rotated = false;
    m_packed = false;

    m_size = 20;
    m_area = QRect( -m_size-8-4, -4, m_size+4, 8 );

    QPoint pinPos = QPoint(0,0);
    m_pin.resize( 1 );
    m_pin[0] = new Pin( 0, pinPos, id+"-pin", 0, this);
    m_pin[0]->setLabelColor( Qt::black );

    setLabelPos(-16,-24, 0);
}

Tunnel::~Tunnel() 
{
    //if( !this->parentItem() )
        removeTunnel();
}

void Tunnel::setName( QString name )
{
    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim ) Simulator::self()->pauseSim();

    removeTunnel(); // Remove old name

    if( m_count.contains( name ) ) // There is already tunnel with this name
    {
        int count = m_count.value( name ) + 1;
        m_count[ name ] = count;

        if( !m_nodes.contains( name ))  // Seccond tunnel with this name
        {
            eNode* node = new eNode( m_id+"-"+name+"-eNode" ); // Create eNode when at least 2 tunnels.
            m_nodes.insert( name, node );
        }
        //m_node = m_nodes.value( name );
    }
    else if( name != "" ) m_count.insert( name, 1 ); // First tunnel with this name

    m_name = name;
    m_pin[0]->setLabelText( name );

    if( name == "" ) m_size = 20;
    else  m_size = m_pin[0]->labelSizeX()+4;

    setRotated( m_rotated );
    if( pauseSim ) Simulator::self()->runContinuous();
}

void Tunnel::setRotated( bool rot )
{
    m_rotated  = rot;
    if( rot )
    {
        m_area = QRect( 4, -4, m_size+4, 8 );
        m_pin[0]->setPinAngle( 180 );
        m_pin[0]->setLabelPos();
    }
    else
    {
        m_area = QRect( -m_size-8, -4, m_size+4, 8 );
        m_pin[0]->setPinAngle( 0 );
        m_pin[0]->setLabelPos();
    }
    Circuit::self()->update();
}

void Tunnel::removeTunnel()
{
    if( m_count.contains( m_name ) ) // Remove old name from lists
    {
        int count = m_count.value( m_name )-1;

        if( count > 0 )
        {
            m_count[ m_name ] = count;

            if( (count < 2) && m_nodes.contains( m_name ) ) // Delete eNode
            {
                //eNode* node = m_nodes.value( m_name );
                m_nodes.remove( m_name );
                //Simulator::self()->remFromEnodeList( node, true );
            }
        }
        else m_count.remove( m_name );
    }
}

void Tunnel::attach()
{
    if( !m_nodes.contains( m_name )
     || !m_pin[0]->isConnected() ) return;

    eNode* node = m_nodes.value( m_name );
    if( node )
    {
        //m_pin[0]->registerPinsW( node );

        QList<ePin*> epins = m_pin[0]->getEnode()->getEpins();
        for( ePin* epin : epins ) epin->setEnode( node );
    }
}

void Tunnel::remove()
{
    removeTunnel();
    
    Component::remove();
}

void Tunnel::setPacked( bool p )
{
    m_packed = p;
}

void Tunnel::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    if( m_hidden || m_packed ) return;

    Component::paint( p, option, widget );

    //p->setBrush( QColor( 255, 166, 0 ) );

    if( m_rotated )
    {
        QPointF points[5] =        {
            QPointF( m_size+8,-5 ),
            QPointF(  8,-5 ),
            QPointF(  4, 0 ),
            QPointF(  8, 5 ),
            QPointF( m_size+8, 5 ) };

        p->drawPolygon( points, 5 );
    }
    else
    {
        QPointF points[5] =        {
            QPointF(-m_size-8,-5 ),
            QPointF(  -8,-5 ),
            QPointF(  -4, 0 ),
            QPointF(  -8, 5 ),
            QPointF(-m_size-8, 5 ) };

        p->drawPolygon( points, 5 );
    }
}

#include "moc_tunnel.cpp"

