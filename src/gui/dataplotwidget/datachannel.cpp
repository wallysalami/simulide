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

#include "datachannel.h"
#include "simulator.h"

DataChannel::DataChannel()
{
}

DataChannel::~DataChannel()
{
}

void DataChannel::updateStep()
{
    m_dataPlotW->m_display->setMaxMin( m_channel, m_dispMax, m_dispMin );

    if( m_dataPlotW->m_auto[m_channel] )
    {
        m_dataPlotW->setHTick( m_channel, m_hTick );
        m_dataPlotW->setVTick( m_channel, (m_dispMax-m_dispMin)/10 );
        m_dataPlotW->setVPos( m_channel, 0 );
    }
    else
    {
        m_hTick = m_dataPlotW->m_hTick[m_channel];
        //m_dataPlotW->setVPos( m_channel, m_dataPlotW->m_vPos[m_channel] );
        //m_vTick = m_dataPlotW->m_vTick[m_channel];
        //m_Vpos  = m_dataPlotW->m_Vpos[m_channel];
    }

    QString unit = " V";
    double val = m_ampli;

    if( val < 1 )
    {
        unit = " mV";
        val *= 1e3;
    }
    int Vdecimals = 0;
    if     ( val < 10) Vdecimals = 2;
    else if( val < 100) Vdecimals = 1;
    m_dataPlotW->m_data2Label[m_channel]->setText( "Amp "+QString::number( val,'f', Vdecimals )+unit );
}



