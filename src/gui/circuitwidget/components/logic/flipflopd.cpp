/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
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

#include "flipflopd.h"
#include "circuit.h"
#include "pin.h"


Component* FlipFlopD::construct( QObject* parent, QString type, QString id )
{
        return new FlipFlopD( parent, type, id );
}

LibraryItem* FlipFlopD::libraryItem()
{
    return new LibraryItem(
        tr( "FlipFlop D" ),
        tr( "Logic/Memory" ),
        "2to2.png",
        "FlipFlopD",
        FlipFlopD::construct );
}

FlipFlopD::FlipFlopD( QObject* parent, QString type, QString id )
         : LogicComponent( parent, type, id )
         , eFlipFlopD( id.toStdString() )
{
    m_width  = 3;
    m_height = 3;
    
    QStringList pinList;

    pinList // Inputs:
            << "IL01D"
            << "IU01S"
            << "ID02R"
            << "IL02>"
            
            // Outputs:
            << "OR01Q"
            << "OR02!Q"
            ;
    init( pinList );
    
    eLogicDevice::createInput( m_inPin[0] );                  // Input D
    eLogicDevice::createInput( m_inPin[1] );                  // Input S
    eLogicDevice::createInput( m_inPin[2] );                  // Input R
    
    m_trigPin = m_inPin[3];
    eLogicDevice::createClockPin( m_trigPin );             // Input Clock
    
    eLogicDevice::createOutput( m_outPin[0] );               // Output Q
    eLogicDevice::createOutput( m_outPin[1] );               // Output Q'

    setSrInv( true );                           // Inver Set & Reset pins
    setClockInv( false );                        //Don't Invert Clock pin
    setTrigger( Clock );
}
FlipFlopD::~FlipFlopD(){}

void FlipFlopD::setTrigger( Trigger trigger )
{
    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim ) Simulator::self()->pauseSim();

    int trig = static_cast<int>( trigger );
    eLogicDevice::seteTrigger( trig );
    LogicComponent::setTrigger( trigger );

    if( pauseSim ) Simulator::self()->runContinuous();
    Circuit::self()->update();
}

#include "moc_flipflopd.cpp"
