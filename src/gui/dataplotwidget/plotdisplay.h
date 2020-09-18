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

#ifndef PLOTDISPLAY_H
#define PLOTDISPLAY_H

#include <QPixmap>
#include <QtWidgets>

class PlotDisplay : public QWidget
{
    friend class DataPlotWidget;
    friend class PlotBase;
    Q_OBJECT

    public:
        PlotDisplay( QWidget* parent = 0 );

        QSize minimumSizeHint() const;
        QSize sizeHint() const;

        void setSize( int width, int height );
        void setDatSize( int size );

        void setData( int ch, QVector<double>* data );
        void setVTick( int ch, double tick );
        void setVPos( int ch, double vPos );
        void setMaxMin( int ch, double max, double min );

        void printData(int i, int d);
        void clear();

        //void step( double scale );

        //bool m_rotate;

    protected:
        virtual void paintEvent( QPaintEvent *event );

    private:
        void drawBackground( QPainter *p );
        void updateValues();

        QVector<double>* m_data[2];

        //bool m_fastMode;

        int m_width;
        int m_height;
        //int m_step;

        double m_sizeX;
        double m_sizeY;

        double m_hCenter;
        double m_vCenter;
        double m_margin;
        double m_scale[2];
        double m_dataScale[2];

        double m_vMax[2];
        double m_vMin[2];
        double m_ampli[2];
        double m_vTick[2];
        double m_vPos[2];

        double m_ceroX ;
        double m_endX;
        double m_ceroY;
        double m_endS;
        double m_lineX;
        double m_linWi;

        double m_endY[2];
        double m_midV[2];
        double m_sclY[2];
        double m_posY[2];
        double m_vMaxPos[2];
        double m_vMinPos[2];
        
        //QPointF lastPoint;
        QFont m_fontB;
        QFont m_fontS;
        QPixmap m_foreground[2];
        QPixmap m_background;
        QColor m_color[3];
        QColor m_scaleColor[3];
        QPointF m_lastP[2];
        
        //int m_sec;
};

#endif

