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

#include "plotbase.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "circuitwidget.h"
#include "dataplotwidget.h"


PlotBase::PlotBase( QObject* parent, QString type, QString id )
        : Component( parent, type, id )
        , eElement( (id+"-eElement").toStdString() )
        , m_topW()
{
    m_pin.resize(3);
    m_ePin.resize(3);

    m_pin[0] = new Pin( 180, QPoint( -80-8, 0 ), id+"-Pin0", 0, this );
    m_pin[1] = new Pin( 180, QPoint( -80-8, 16), id+"-Pin1", 0, this );
    m_pin[2] = new Pin( 180, QPoint( -80-8, 40), id+"-PinG", 0, this );

    for( int i=0; i<3; i++ )
    {
        m_ePin[i] = m_pin[i];
        m_pin[i]->setLabelColor( QColor( 0, 0, 0 ) );
        m_pin[i]->setLength( 5 );
    }
    for( int ch=0; ch<2; ch++ )
    {
        m_pinConnected[ch] = false;
        m_probeConnected[ch] = false;
        m_probe[ch] = "";
    }

    m_topW      = 0l;
    m_proxy     = 0l;
    m_dataPlotW = 0l;

    m_advanc = true; // Force create widgets

    //m_channel.resize( 2 );

    //m_proxy->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, true );

    Simulator::self()->addToUpdateList( this );
    Simulator::self()->addToSimuClockList( this );
}

PlotBase::~PlotBase() 
{
}

void PlotBase::resetState()
{
    for(int ch=0; ch<2; ch++ )
    {
        m_channel[ch]->resetState();
        m_dataPlotW->setProbe( ch );
    }
}

void PlotBase::updateStep()
{
    for( int i=0; i<2; i++ )
    {
        if( m_pin[i]->isConnected() ) m_pinConnected[i] = true;
        else                          m_pinConnected[i] = false;

        if(( !m_pinConnected[i] && !m_probeConnected[i])
          || !Simulator::self()->isRunning() )
        {
            m_display->setData( i, 0l );
            if( m_dataPlotW->m_data1Label[i] ) m_dataPlotW->m_data1Label[i]->setText( "" );
            m_dataPlotW->m_data2Label[i]->setText( "" );
        }
        else
        {
            m_display->setData( i, &m_channel[i]->m_data );
            m_channel[i]->updateStep();
        }
    }

    m_dataPlotW->m_display->update();
}

void PlotBase::simuClockStep()
{
    for(int ch=0; ch<2; ch++ )
    {
        double data =  0;
        if     ( m_pinConnected[ch] )   data = m_pin[ch]->getVolt();
        else if( m_probeConnected[ch] ) data = m_dataPlotW->m_probe[ch]->getVolt();
        else continue;

        data = data - m_pin[2]->getVolt();
        m_channel[ch]->step( data );
    }
}

void PlotBase::setAdvanc( bool advanc )
{
    m_advanc = advanc;

    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim ) Simulator::self()->pauseSim();

    if( !m_topW ) m_topW = new TopWidget();

    if( !m_dataPlotW )
    {
        m_dataPlotW = new DataPlotWidget( m_topW, this );
        m_dataPlotW->setupWidget();
        m_topW->setupWidget( m_dataPlotW );
    }
    else m_dataPlotW->setVisible( false );
    if( !m_proxy )
    {
        m_proxy = Circuit::self()->addWidget( m_topW );
        m_proxy->setParentItem( this );
    }
    m_display = m_dataPlotW->m_display;

    if( advanc )
    {
        m_sizeX  = m_adSizeX;
        m_sizeY  = m_adSizeY;
        m_extraSize = 202+m_display->m_margin*6;
    }
    else
    {
        m_sizeX  = m_baSizeX;
        m_sizeY  = m_baSizeY;
        m_extraSize = 81+m_display->m_margin*6;
    }
    double ws = (double)m_sizeY/200;
    double sc = (7+3*ws)/10;
    m_extraSize *= sc;

    int widgetSizeX = m_sizeX+m_extraSize;
    int widgetSizeY = m_sizeY+20;
    int centerY = widgetSizeY/2;

    m_area = QRectF( -80-4, -centerY-4,  widgetSizeX+8, widgetSizeY+8 );
    setLabelPos(-80,-centerY-20, 0);

    //m_dataPlotW->m_dataSize = m_dataSize;
    m_dataPlotW->setSize( m_sizeX, m_sizeY );
    m_dataPlotW->setFixedSize( widgetSizeX, widgetSizeY );
    m_dataPlotW->updateWidgets();
    m_dataPlotW->showControlls( advanc );
    m_dataPlotW->m_advaCheck->setChecked( advanc );
    m_dataPlotW->setVisible( true );
    m_topW->setFixedSize( widgetSizeX, widgetSizeY );

    for( int i=0; i<2; i++ )
    {
        m_channel[i]->m_dataPlotW = m_dataPlotW;

        if( m_channel[i]->m_data.size() != m_dataSize )
            m_channel[i]->m_data.resize( m_dataSize );

        if( m_channel[i]->m_buffer.size() != m_bufferSize )
        m_channel[i]->m_buffer.resize( m_bufferSize );

        m_dataPlotW->setVTick( i, m_dataPlotW->m_vTick[i] );
        m_dataPlotW->setVPos( i, m_dataPlotW->m_vPos[i] );
    }

    m_proxy->setPos( QPoint( -80, -centerY) );

    Circuit::self()->update();
    if( pauseSim ) Simulator::self()->runContinuous();
}

void PlotBase::setDataSize( int size )
{
    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim ) Simulator::self()->pauseSim();

    m_dataSize = size;

    for( int i=0; i<2; i++ )
    {
        int dataSize = m_channel[i]->m_data.size();
        if( dataSize != m_dataSize )
            m_channel[i]->m_data.resize( m_dataSize );

        //m_dataPlotW->m_display->setData( i, &m_channel[i]->m_data );

        //m_channel[i]->resetState();
        double htick = (double)m_dataPlotW->m_hTick[i]*(double)dataSize/(double)m_dataSize;
        m_dataPlotW->setHTick( i, (int)htick );

        //m_dataPlotW->updateHTickBox( i );
    }
    if( pauseSim ) Simulator::self()->runContinuous();
}

void PlotBase::remove()
{
    Simulator::self()->remFromUpdateList( this );
    Simulator::self()->remFromSimuClockList( this );

    //for(int i=0; i<2; i++ ) delete m_channel[i];
    //m_channel.clear();

    Component::remove();
}

/*void PlotBase::setFastMode( bool fast )
{
    m_dataPlotW->m_display->m_fastMode = fast;
    for( int i=0; i<2; i++ )  m_channel[i]->m_fastMode = fast;
}*/

void PlotBase::setTrigger1( bool t )
{
    m_channel[0]->m_trigger = t;
    m_dataPlotW->m_data1Label[0]->setHidden( !t );

    ///m_dataPlotW->m_trigCheck->setChecked( t );
}

void PlotBase::setTrigger2( bool t )
{
    m_channel[1]->m_trigger = t;
    m_dataPlotW->m_data1Label[1]->setHidden( !t );

    ///m_dataPlotW->m_trigCheck->setChecked( t );
}

void PlotBase::setProbe1( QString p )
{
    m_probe[0] = p;
    m_dataPlotW->m_channel[0]->setText( p );
    m_dataPlotW->setProbe( 0 );
}

void PlotBase::setProbe2( QString p )
{
    m_probe[1] = p;
    m_dataPlotW->m_channel[1]->setText( p );
    m_dataPlotW->setProbe( 1 );
}

void PlotBase::paint( QPainter *p, const QStyleOptionGraphicsItem* option, QWidget *widget )
{
    Component::paint( p, option, widget );
    
    p->setBrush( Qt::darkGray );
    p->drawRoundedRect( m_area, 4, 4 );
    
    p->setBrush( Qt::white );
    QPen pen = p->pen();
    pen.setWidth( 0 );
    pen.setColor( Qt::white );
    p->setPen(pen);
    
    p->drawRoundedRect( QRectF( -80-4, -(m_sizeY+20)/2-4, m_sizeX+m_extraSize+4, m_sizeY+20+4 ), 3, 3 );
}

#include "moc_plotbase.cpp"


