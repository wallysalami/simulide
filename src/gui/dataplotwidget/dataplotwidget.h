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

#ifndef DATAPLOTWIDGET_H
#define DATAPLOTWIDGET_H

#include <QtWidgets>
#include <QVector>

#include "plotdisplay.h"
#include "probe.h"

class PlotBase;

class MAINMODULE_EXPORT DataPlotWidget : public QWidget
{
    friend class DataChannel;
    friend class OscopeChannel;
    friend class PlotBase;
    Q_OBJECT

    public:
        DataPlotWidget(QWidget* parent , PlotBase* PlotBase );
        ~DataPlotWidget();

        void setupWidget();
        void showControlls( bool show );
        void setSize( int width, int height );

        void setHTick( int ch, int hTick );
        void setVTick( int ch, double vTick );
        void setHPos( int ch, double vPos );
        void setVPos( int ch, double vPos );
        void setAuto( int ch, bool au );

        void updateWidgets();

        bool eventFilter( QObject* object, QEvent* event );

    public slots:
        void avdChanged( bool ad );
        void autoChanged( bool au );
        void triggerChanged( bool tr);

        void channel1Changed();
        void channel2Changed();

        void updateHTickBox( int ch );
        void updateHPosBox( int ch );

        void hTickDial1Changed( int Hscale );
        void vTickDial1Changed( int Vscale );
        void hPosDial1Changed( int hPos );
        void vPosDial1Changed( int vPos );

        void hTickBox1Changed();
        void vTickBox1Changed();
        void hPosBox1Changed();
        void vPosBox1Changed();

        void buttonChanged( int ch );

        void probeRemoved( Probe* p );
        void setProbe( int ch );

    private:
        void hTickDialChanged( int ch, int Hscale );
        void vTickDialChanged( int ch, int Vscale );
        void hPosDialChanged( int ch, int vPos );
        void vPosDialChanged( int ch, int vPos );

        void hTickBoxChanged( int ch );
        void vTickBoxChanged( int ch );
        void hPosBoxChanged( int ch );
        void vPosBoxChanged( int ch );

        QDial* createDial();
        QDoubleSpinBox* createSpinBox( QString suffix );
        QHBoxLayout* createQHBoxLayout();

        QHBoxLayout* m_horizontalLayout;
        QVBoxLayout* m_vChannelLayout;
        QVBoxLayout* m_controllsLayout;

        QLabel* m_hTickLabel;
        QLabel* m_hPosLabel;
        QLabel* m_vTickLabel;
        QLabel* m_vPosLabel;
        QLabel* m_data1Label[2];
        QLabel* m_data2Label[2];

        QCheckBox* m_advaCheck;
        QCheckBox* m_autoCheck;
        QCheckBox* m_trigCheck;

        QDial*          m_hTickDial;
        QDoubleSpinBox* m_hTickBox;
        QDial*          m_hPosDial;
        QDoubleSpinBox* m_hPosBox;
        QDial*          m_vTickDial;
        QDoubleSpinBox* m_vTickBox;
        QDial*          m_vPosDial;
        QDoubleSpinBox* m_vPosBox;

        QButtonGroup* m_buttonGroup;
        QPushButton* m_button[3];

        QLineEdit* m_channel[2];

        QFrame* m_line[3];
        QPalette m_palette;

        bool m_singleHTick;

        int m_sizeX;
        int m_sizeY;

        int    m_prevCh;
        int    m_prevHTick;
        int    m_prevHPos;
        double m_prevVTick;
        double m_prevVPos;

        int    m_hTick[2];
        double m_vTick[2];
        int    m_hPos[2];
        double m_vPos[2];
        bool   m_auto[2];

        PlotDisplay* m_display;

        Probe*  m_probe[2];

        PlotBase* m_plotB;
};

#endif
