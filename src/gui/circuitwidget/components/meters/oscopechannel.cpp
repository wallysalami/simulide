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

#include "oscopechannel.h"
#include "simulator.h"

OscopeChannel::OscopeChannel()
             : DataChannel()
{
    m_ampli = 0;
    m_filter = 0.1;

    m_trigger = true;

    m_hTick = 1;
    m_vTick = 1;
    m_Vpos = 0;
}

OscopeChannel::~OscopeChannel() 
{
}

void OscopeChannel::resetState()
{
    if( m_trigger )
    {
        //qDebug() <<"OscopeChannel::resetState";
        m_reading  = true;
        m_saveData = false;
        m_rising   = false;
        m_falling  = false;

        m_step = 0;
        m_totalP = 0;
        m_numCycles = 0;
        m_numMax = 0;
        m_lastData =0;
        m_bufferCounter = 0;
        m_ampli = 0;
        m_hTick = 1;
        m_vTick = 1;
        m_Vpos = 0;
        m_freq = 500;
    }
    else
    {
        if( m_dataPlotW->m_auto[m_channel] )
        {
            m_hTick = 1e6;
            m_vTick = 0.5;
            m_Vpos = 0;
        }
        m_dataPlotW->m_display->clear();
    }
    for( int i=0; i<m_data.size(); i++ ) m_data[i] = 0.0 ;
    m_counter = 0;
    m_tick = 0;
    m_max =-1e12;
    m_min = 1e12;
    m_dispMax = 5;
    m_dispMin =-5;

    if( m_dataPlotW->m_auto[m_channel] )
    {
        m_dataPlotW->setHTick( m_channel, m_hTick );
        m_dataPlotW->setVTick( m_channel, m_vTick );
        m_dataPlotW->setHPos( m_channel, 0 );
        m_dataPlotW->setVPos( m_channel, m_Vpos );
    }
    DataChannel::updateStep();
    m_dataPlotW->m_data1Label[m_channel]->setText( "" );
    m_dataPlotW->m_data2Label[m_channel]->setText( "" );
}

void OscopeChannel::updateStep()
{
    if( m_trigger )
    {
        double spus = Simulator::self()->stepsPerus();
        m_stepsPerS = spus*1e6;

        if( m_step > m_stepsPerS ) resetState();
        if( m_numMax < 2 ) return;
        m_numMax--;

        double freq = m_stepsPerS/(double)(m_totalP/m_numMax);

        if( m_freq != freq )
        {
            m_freq = freq;
            int Fdecs = 1;
            QString unit = "  Hz";

            if( freq > 999 )
            {
                freq /= 1e3; unit = " KHz";
                if( freq > 999 ) { freq /= 1e3; unit = " MHz"; }
            }
            if     ( freq < 10 )  Fdecs = 3;
            else if( freq < 100 ) Fdecs = 2;
            m_dataPlotW->m_data1Label[m_channel]->setText( " "+QString::number( freq, 'f', Fdecs )+unit );

            if( m_freq == 0 ) resetState();
        }
        m_totalP = 0;
        m_numMax = 0;
        m_lastMax = 0;
        m_step   = 0;

        if( !m_saveData )
        {
            DataChannel::updateStep();
            m_reading = true;
        }
    }
    else
    {
        if( Simulator::self()->isRunning() )
        {
            m_dispMax = m_max;
            m_dispMin = m_min;
            m_ampli   = m_max-m_min;
        }

        if( m_dataPlotW->m_auto[m_channel] )
        {
            m_hTick = Simulator::self()->circuitRate()/Simulator::self()->stepsPerus()*m_dataPlotW->m_sizeX/m_data.size()/10;
        }
        DataChannel::updateStep();
    }
    if( m_dataPlotW->m_auto[m_channel] ) m_dataPlotW->setHPos( m_channel, 0 );
}

void OscopeChannel::step( double data )
{
    if( m_trigger )
    {
        stepTrigger( data );
        return;
    }
    if( ++m_tick < m_hTick ) return;
    m_tick = 0;

    if     ( data > m_max ) m_max = data;
    else if( data < m_min ) m_min = data;

    /*if( m_running )
    {
        //if( ++m_bufferCounter >= m_buffer.size()-1 )
        {
            //m_bufferCounter = m_buffer.size()-1;
            for( int i=0; i<m_buffer.size()-1; i++ ) m_buffer[i] = m_buffer[i+1];
        }
        m_buffer[m_buffer.size()-1] = data;
    }
    else*/
    {
        if( m_counter >= m_data.size() ) m_counter = 0;
        m_data[m_counter] = data;
        //if ( m_fastMode ) m_dataPlotW->m_display->printData( m_channel, m_counter );
        m_counter++;
    }
}

void OscopeChannel::stepTrigger( double data )
{
    m_step++;

    double delta = data-m_lastData;

    if     ( data > m_max ) m_max = data;
    else if( data < m_min ) m_min = data;

    if( ++m_bufferCounter >= m_buffer.size() ) m_bufferCounter = 0;
    m_buffer[m_bufferCounter] = data;

    if( delta > m_filter )                                     // Rising
    {
        if( m_falling && !m_rising )                        // Min Found
        {
            if( m_numMax > 0 ) m_totalP += m_step-m_lastMax;

            m_lastMax = m_step;
            m_numMax++;
            m_numCycles++;

            m_ampli = m_max-m_min;
            m_dispMax = m_max;
            m_dispMin = m_min;

            m_falling = false;
            m_max = -1e12;
        }
        m_rising = true;
        m_lastData = data;

        if( m_reading && !m_saveData && ( m_numCycles > 1 ))// Wait for a full wave
        {
            double mid = m_min + m_ampli/2;

            if( data >= mid )                             // Rising edge
            {
                m_numCycles = 0;
                int per = m_stepsPerS/m_freq;
                if( per > 1 )
                {
                    if( m_dataPlotW->m_auto[m_channel] )
                    {
                        m_Vpos = 0;
                        m_vTick = m_ampli/10;
                        if     ( m_vTick > 1000 )  m_vTick = 1000;
                        else if( m_vTick < 0.001 ) m_vTick = 0.001;

                        m_hTick = abs(per/(m_data.size()/2));
                        if     ( m_hTick < 1 )      m_hTick = 1;
                        //else if( m_hTick > 3500 ) m_hTick = 3500;
                    }
                    m_tick = 0;
                    m_counter = 0;
                    m_saveData = true;
                }
            }
        }
    }
    else if( delta < -m_filter )                              // Falling
    {
        if( m_rising && !m_falling )                        // Max Found
        {
            m_rising = false;
            m_min = 1e12;
        }
        m_falling = true;
        m_lastData = data;
    }
    if( m_saveData )                          // Data Set saving
    {
        if( m_counter == 0 ) // Get data from buffer
        {
            //qDebug() <<"OscopeChannel::simuClockStep"<<m_bufferCounter<<m_hTick;
            int delta = m_dataPlotW->m_hPos[m_channel];
            int mid = m_data.size()/2+delta/m_hTick;
            if( mid > m_data.size()-1 ) mid = m_data.size()-1;
            int bufferC = m_bufferCounter-mid*m_hTick;
            while( bufferC < 0 ) bufferC += m_buffer.size();

            for( m_counter=0; m_counter<mid; m_counter++ )
            {
                m_data[m_counter] = m_buffer[bufferC];
                bufferC += m_hTick;
                if( bufferC >= m_buffer.size() ) bufferC -= m_buffer.size();
            }

            m_data[mid] = data;
            m_counter = mid+1;
        }
        else if( ++m_tick == m_hTick ) //save Data
        {
            m_data[m_counter] = data;
            m_counter++;
            m_tick = 0;

            if( m_counter == m_data.size() )  // DataSet Ready to display
            {
                m_saveData = false;
                m_numCycles = 0;
                m_counter = 0;
            }
        }
    }
}

