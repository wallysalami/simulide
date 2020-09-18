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

#ifndef DATACHANNEL_H
#define DATACHANNEL_H

#include "dataplotwidget.h"


class MAINMODULE_EXPORT DataChannel
{
    friend class DataPlotWidget;
    friend class PlotBase;
    friend class Oscope;
    public:

        DataChannel();
        ~DataChannel();
        
        virtual void resetState(){;}
        virtual void updateStep();
        virtual void step( double data ){;}

    protected:
        QVector<double> m_data;
        QVector<double> m_buffer;

        bool m_trigger;
        //bool m_fastMode;

        int m_channel;
        int m_bufferCounter;
        int m_counter;

        int m_tick;
        int m_hTick;

        double m_max;
        double m_min;
        double m_dispMax;
        double m_dispMin;
        double m_ampli;

        DataPlotWidget* m_dataPlotW;
};

#endif
