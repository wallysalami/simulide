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

#include "dataplotwidget.h"
#include "plotbase.h"
#include "mainwindow.h"
#include "circuit.h"
//#include "simulator.h"
//#include "oscope.h"

#include <math.h>   // fabs(x,y)

DataPlotWidget::DataPlotWidget(  QWidget* parent, PlotBase* plotB  )
              : QWidget( parent )
{
    m_plotB = plotB;

    this->setVisible( false );

    for( int i=0; i<2; i++ )
    {
        m_hTick[i] = 10;
        m_vTick[i] = 1;
        m_hPos[i]  = 0;
        m_vPos[i]  = 0;
        m_probe[i] = 0l;
        m_auto[i] = true;
    }
    m_prevHTick = 10;
    m_prevVTick = 1;
    m_prevVPos = 0;
    m_prevHPos = 0;
    m_prevCh = 0;
}

DataPlotWidget::~DataPlotWidget()
{
}

void DataPlotWidget::hTickDialChanged( int ch, int Hscale ) // Dial changed
{
    if( m_auto[ch] ) return;
    
    int delta = m_hTick[ch]/10;
    if( delta < 1 ) delta = 1;

    if( Hscale < m_prevHTick ) m_hTick[ch] += delta;
    else                       m_hTick[ch] -= delta;

    if( m_hTick[ch] < 1 ) m_hTick[ch] = 1;

    m_prevHTick = Hscale;

    updateHTickBox( ch );
}

void DataPlotWidget::hTickBoxChanged( int ch ) // User entered value
{
    if( m_auto[ch] ) return;

    double  val  = m_hTickBox->value();
    QString unit = m_hTickBox->suffix();

    if     ( unit == " ms" ) val *= 1e3;
    else if( unit == " s")   val *= 1e6;

    double spus = Simulator::self()->stepsPerus();
    val = spus*10*val/m_plotB->m_dataSize+0.5;

    if( val < 1 ) val = 1;

    m_hTick[ch] = (int)val;

    updateHTickBox( ch );
}

void DataPlotWidget::setHTick( int ch, int hTick )
{
    m_hTick[ch] = hTick;
    updateHTickBox( ch );
}

void DataPlotWidget::updateHTickBox( int ch )
{
    int activeChan = m_buttonGroup->checkedId();
    if(( activeChan == 2 )
    || ( activeChan == ch ))
    {
        double spus = Simulator::self()->stepsPerus();
        double val  = m_hTick[ch]*m_plotB->m_dataSize/10/spus;

        QString unit = " us";

        if( val > 999*spus )
        {
            val /= 1e3; unit = " ms";
            if( val > 999*spus ) { val /= 1e3; unit = " s"; }
        }
        int Vdecimals = 0;
        if     ( val < 10)   Vdecimals = 3;
        else if( val < 100)  Vdecimals = 2;
        else if( val < 1000) Vdecimals = 1;

        m_hTickBox->setDecimals( Vdecimals );
        m_hTickBox->setValue( val );
        m_hTickBox->setSuffix( unit );
    }
}

void DataPlotWidget::vTickDialChanged( int ch, int Vscale ) // Changed by Dial
{
    if( m_auto[ch] ) return;
    
    double vscale = (double)Vscale;
    if( vscale < m_prevVTick ) m_vTick[ch] *= 1.01;
    else                       m_vTick[ch] /= 1.01;

    if( m_vTick[ch] < 0.001 ) m_vTick[ch] = 0.001;
    m_prevVTick = vscale;
    m_display->setVTick( ch, m_vTick[ch] );

    int activeChan = m_buttonGroup->checkedId();
    if(( activeChan == 2 )
    || ( activeChan == ch ))m_vTickBox->setValue( m_vTick[ch] );
}

void DataPlotWidget::vTickBoxChanged( int ch )
{
    if( m_auto[ch] ) return;
    m_vTick[ch] = m_vTickBox->value();
    m_display->setVTick( ch, m_vTick[ch] );
}

void DataPlotWidget::setVTick( int ch, double vTick )
{
    m_vTick[ch] = vTick;
    m_display->setVTick( ch, vTick );

    int activeChan = m_buttonGroup->checkedId();
    if(( activeChan == 2 )
    || ( activeChan == ch ))m_vTickBox->setValue( vTick );
}

void DataPlotWidget::hPosDialChanged( int ch, int ihPos ) // Changed by Dial
{
    if( m_auto[ch] ) return;

    double hPos = (double)ihPos;

    int delta = m_hTick[ch];

    if( delta < 1 ) delta = 1;

    if( hPos < m_prevHPos ) m_hPos[ch] -= delta;
    else                    m_hPos[ch] += delta;

    m_prevHPos = hPos;

    updateHPosBox( ch );
    ///m_display->setHPos( ch, m_hPos[ch] );
}

void DataPlotWidget::hPosBoxChanged( int ch )
{
    if( m_auto[ch] ) return;

    double  val  = m_hPosBox->value();
    QString unit = m_hPosBox->suffix();

    if     ( unit == " ms" ) val *= 1e3;
    else if( unit == " s")   val *= 1e6;

    double spus = Simulator::self()->stepsPerus();
    val = spus*val;
    int iVal = val/m_hTick[ch];
    iVal *= m_hTick[ch];

    m_hPos[ch] = iVal;

    updateHPosBox( ch );
}

void DataPlotWidget::setHPos( int ch, double hPos )
{
    m_hPos[ch] = hPos;
    updateHPosBox( ch );
}

void DataPlotWidget::updateHPosBox( int ch )
{
    int activeChan = m_buttonGroup->checkedId();
    if(( activeChan == 2 )
    || ( activeChan == ch ))
    {
        double spus = Simulator::self()->stepsPerus();
        double val  = m_hPos[ch]/spus;

        QString unit = " us";

        if( fabs( val ) > 999*spus )
        {
            val /= 1e3; unit = " ms";
            if( fabs( val ) > 999*spus ) { val /= 1e3; unit = " s"; }
        }
        int Vdecimals = 0;
        double fval = fabs( val );
        if     ( fval < 10)   Vdecimals = 3;
        else if( fval < 100)  Vdecimals = 2;
        else if( fval < 1000) Vdecimals = 1;

        m_hPosBox->setDecimals( Vdecimals );
        m_hPosBox->setValue( val );
        m_hPosBox->setSuffix( unit );
    }
}

void DataPlotWidget::vPosDialChanged( int ch, int ivPos ) // Changed by Dial
{
    if( m_auto[ch] ) return;
    
    double vPos = (double)ivPos;
    
    if( vPos < m_prevVPos ) m_vPos[ch] -= 0.05*m_vTick[ch];
    else                    m_vPos[ch] += 0.05*m_vTick[ch];

    m_prevVPos = vPos;
    m_display->setVPos( ch, m_vPos[ch] );

    int activeChan = m_buttonGroup->checkedId();
    if(( activeChan == 2 )
    || ( activeChan == ch )) m_vPosBox->setValue( m_vPos[ch] );
}

void DataPlotWidget::vPosBoxChanged( int ch )
{
    if( m_auto[ch] ) return;
    m_vPos[ch] = m_vPosBox->value();
    m_display->setVPos( ch, m_vPos[ch] );
}

void DataPlotWidget::setVPos( int ch, double vPos )
{
    m_vPos[ch] = vPos;
    m_display->setVPos( ch, vPos );

    int activeChan = m_buttonGroup->checkedId();
    if(( activeChan == 2 )
    || ( activeChan == ch )) m_vPosBox->setValue( vPos );
}

void DataPlotWidget::buttonChanged( int ch )
{
    ///qDebug() <<"DataPlotWidget::buttonChanged"<<ch;
    m_palette.setColor( QPalette::Base, m_display->m_color[ch] );
    m_hTickBox->setPalette( m_palette );
    m_hPosBox->setPalette( m_palette );
    m_vTickBox->setPalette( m_palette );
    m_vPosBox->setPalette( m_palette );
    m_autoCheck->setPalette( m_palette );
    m_trigCheck->setPalette( m_palette );

    if( ch < 2 )
    {
        m_trigCheck->setChecked( m_plotB->m_channel[ch]->m_trigger);
        m_autoCheck->setChecked( m_auto[ch] );
        setHTick( ch, m_hTick[ch] );
        setVTick( ch, m_vTick[ch] );
        setVPos( ch, m_vPos[ch] );
        m_prevCh = ch;
    }
}

void DataPlotWidget::setAuto( int ch, bool au )
{
    m_autoCheck->setChecked( au );
    m_auto[ch] = au;
}

void DataPlotWidget::showControlls( bool show )
{
    m_autoCheck->setVisible( show );
    m_trigCheck->setVisible( show );
    m_hTickDial->setVisible( show );
    m_hTickBox->setVisible( show );
    m_hTickLabel->setVisible( show );
    m_hPosDial->setVisible( show );
    m_hPosBox->setVisible( show );
    m_hPosLabel->setVisible( show );
    m_vTickDial->setVisible( show );
    m_vTickBox->setVisible( show );
    m_vTickLabel->setVisible( show );
    m_vPosDial->setVisible( show );
    m_vPosBox->setVisible( show );
    m_vPosLabel->setVisible( show );

    for( int i=0; i<3; i++ )
    {
        m_line[i]->setVisible( show );
        m_button[i]->setVisible( show );
    }
}

void DataPlotWidget::setSize( int width, int height )
{
    m_sizeX = width;
    m_sizeY = height;
    m_display->setSize( width, height );
}

void DataPlotWidget::updateWidgets()
{
    double ws = (double)m_sizeY/200;
    double sc = (6+4*ws)/10;            //qDebug() <<"DataPlotWidget::scale"<<sc;
    double fs = MainWindow::self()->fontScale();
    int ds = sc*27;

    QFont font = this->font();
    font.setPixelSize( int(9*fs*sc) );
    font.setBold( true );
    QFont fontd = this->font();
    fontd.setPixelSize( int(10*fs*sc) );
    fontd.setBold( true );

    m_advaCheck->setFixedSize( sc*65, sc*14 );
    m_advaCheck->setFont( font );
    m_autoCheck->setFixedSize( sc*50, sc*14 );
    m_autoCheck->setFont( font );
    m_trigCheck->setFixedSize( sc*60, sc*14 );
    m_trigCheck->setFont( font );

    for( int i=0; i<2; i++ )
    {
        m_channel[i]->setFixedSize( sc*65, sc*17 );
        m_channel[i]->setFont( font );

        m_data1Label[i]->setFixedSize( sc*65, sc*16 );
        m_data1Label[i]->setFont( fontd );
        m_data2Label[i]->setFixedSize( sc*65, sc*16 );
        m_data2Label[i]->setFont( fontd );
    }

    m_hTickLabel->setFont( font );
    m_hTickLabel->setFixedSize( sc*25, sc*15 );
    m_hPosLabel->setFont( font );
    m_hPosLabel->setFixedSize( sc*25, sc*15 );
    m_vTickLabel->setFont( font );
    m_vTickLabel->setFixedSize( sc*25, sc*15 );
    m_vPosLabel->setFont( font );
    m_vPosLabel->setFixedSize( sc*25, sc*15 );

    m_hTickDial->setFixedSize( ds, ds );
    m_hPosDial->setFixedSize( ds, ds );
    m_vTickDial->setFixedSize( ds, ds );
    m_vPosDial->setFixedSize( ds, ds );

    m_hTickBox->setFixedSize( sc*73, sc*20 );
    m_hTickBox->setFont( font );
    m_hPosBox->setFixedSize( sc*73, sc*20 );
    m_hPosBox->setFont( font );
    m_vTickBox->setFixedSize( sc*73, sc*20 );
    m_vTickBox->setFont( font );
    m_vPosBox->setFixedSize( sc*73, sc*20 );
    m_vPosBox->setFont( font );

    for( int i=0; i<3; i++ )
    {
        if( i!= 1 ) m_line[i]->setFixedHeight( ws*ws*10 );
        m_button[i]->setFixedSize( sc*38, sc*20);
        m_button[i]->setFont( font );
    }
    m_line[1]->setFixedHeight( ws*ws*4 );
}

void DataPlotWidget::setupWidget()
{
    m_palette.setColor( QPalette::Text,Qt::black );

    m_display = new PlotDisplay( this );

    m_horizontalLayout = createQHBoxLayout();
    this->setLayout( m_horizontalLayout );
    m_horizontalLayout->addStretch();

    m_vChannelLayout = new QVBoxLayout();
    m_vChannelLayout->setContentsMargins(0, 0, 0, 0);
    m_vChannelLayout->setSpacing(0);
    m_vChannelLayout->setAlignment( Qt::AlignLeft );

    m_advaCheck = new QCheckBox( "Expand", this );
    m_advaCheck->setChecked( true );
    m_vChannelLayout->addWidget( m_advaCheck );

    m_data2Label[0] = new QLabel( "", this );
    m_data1Label[0] = new QLabel( "", this );
    m_vChannelLayout->addWidget( m_data1Label[0] );
    m_vChannelLayout->addWidget( m_data2Label[0] );

    for( int i=0; i<2; i++ )
    {
        m_palette.setColor( QPalette::Base, m_display->m_color[i] );
        m_channel[i] = new QLineEdit( this );
        m_channel[i]->setPlaceholderText( "No Probe" );
        m_channel[i]->setPalette( m_palette );
        m_vChannelLayout->addWidget( m_channel[i] );
    }
    m_data2Label[1] = new QLabel( "", this );
    m_data1Label[1] = new QLabel( "", this );
    m_vChannelLayout->addWidget( m_data2Label[1] );
    m_vChannelLayout->addWidget( m_data1Label[1] );

    m_horizontalLayout->addLayout( m_vChannelLayout );

    m_horizontalLayout->addWidget( m_display );
    m_horizontalLayout->addStretch();

    m_controllsLayout = new QVBoxLayout();
    m_controllsLayout->setContentsMargins(0, 0, 0, 0);
    m_controllsLayout->setSpacing(0);

    QHBoxLayout* buttonLayout = createQHBoxLayout();
    buttonLayout->addStretch();
    m_buttonGroup = new QButtonGroup( this );
    for( int i=0; i<3; i++ )
    {
        QString text = "Ch";
        if( i<2 ) text += QString::number(i+1);
        else      text = tr("All");

        m_button[i] = new QPushButton( text, this );
        m_button[i]->setCheckable( true );
        m_palette.setColor( QPalette::Button, m_display->m_color[i] );
        m_button[i]->setPalette( m_palette );

        m_buttonGroup->addButton( m_button[i] );
        m_buttonGroup->setId( m_button[i], i );

        buttonLayout->addWidget( m_button[i] );

        m_line[i] = new QFrame(this);
        m_line[i]->setFrameShape( QFrame::HLine ) ;
        m_line[i]->setFrameShadow( QFrame::Sunken );
        m_line[i]->setLineWidth( 2 );
    }
    m_button[2]->setChecked( true );
    buttonLayout->addStretch();
    m_controllsLayout->addLayout( buttonLayout );

    QHBoxLayout* hTickLayout = createQHBoxLayout();
    m_hTickDial  = createDial();
    m_hTickBox   = createSpinBox( " S" );
    m_hTickBox->installEventFilter( this );
    m_hTickLabel = new QLabel( " Div", this );
    hTickLayout->addWidget(m_hTickDial );
    hTickLayout->addWidget( m_hTickBox );
    hTickLayout->addWidget( m_hTickLabel );
    QHBoxLayout* hPosLayout = createQHBoxLayout();
    m_hPosDial  = createDial();
    m_hPosBox   = createSpinBox( " S" );
    m_hPosBox->installEventFilter( this );
    m_hPosBox->setMinimum( -1000000 );
    m_hPosBox->setValue( 0 );
    m_hPosLabel = new QLabel( " Pos", this );
    hPosLayout->addWidget( m_hPosDial );
    hPosLayout->addWidget( m_hPosBox );
    hPosLayout->addWidget( m_hPosLabel );
    m_line[1]->setLineWidth( 1 );
    QHBoxLayout* vTickLayout = createQHBoxLayout();
    m_vTickDial  = createDial();
    m_vTickBox   = createSpinBox( " V" );
    m_vTickBox->installEventFilter( this );
    m_vTickLabel = new QLabel( " Div", this );
    vTickLayout->addWidget( m_vTickDial );
    vTickLayout->addWidget( m_vTickBox );
    vTickLayout->addWidget( m_vTickLabel );
    QHBoxLayout* vPosLayout = createQHBoxLayout();
    m_vPosDial  = createDial();
    m_vPosBox   = createSpinBox( " V" );
    m_vPosBox->installEventFilter( this );
    m_vPosBox->setMinimum( -1000000 );
    m_vPosLabel = new QLabel( " Pos", this );
    vPosLayout->addWidget( m_vPosDial );
    vPosLayout->addWidget( m_vPosBox );
    vPosLayout->addWidget( m_vPosLabel );

    m_controllsLayout->addWidget( m_line[0] );
    m_controllsLayout->addLayout( hTickLayout );
    m_controllsLayout->addLayout( hPosLayout );
    m_controllsLayout->addWidget( m_line[1] );
    m_controllsLayout->addLayout( vTickLayout );
    m_controllsLayout->addLayout( vPosLayout );
    m_controllsLayout->addWidget( m_line[2] );

    QHBoxLayout* checkLayout = createQHBoxLayout();
    checkLayout->addStretch();
    m_autoCheck = new QCheckBox( "Auto", this );
    m_autoCheck->setChecked( true );
    checkLayout->addWidget( m_autoCheck );
    m_trigCheck = new QCheckBox( "Trigger", this );
    m_trigCheck->setChecked( true );
    checkLayout->addWidget( m_trigCheck );
    checkLayout->addStretch();

    m_controllsLayout->addLayout( checkLayout );
    m_horizontalLayout->addLayout( m_controllsLayout );
    m_horizontalLayout->addStretch();

    connect( m_advaCheck, SIGNAL( clicked(bool) ),
             this,        SLOT  ( avdChanged(bool)) );

    connect( m_autoCheck, SIGNAL( clicked(bool) ),
             this,        SLOT  ( autoChanged(bool)) );

    connect( m_trigCheck, SIGNAL( clicked(bool) ),
             this,        SLOT  ( triggerChanged(bool)) );

    connect( m_channel[0], SIGNAL( editingFinished()  ),
             this,        SLOT  ( channel1Changed()) );

    connect( m_channel[1], SIGNAL( editingFinished()  ),
             this,        SLOT  ( channel2Changed()) );


    connect( m_hTickDial, SIGNAL( valueChanged(int) ),
             this,        SLOT  ( hTickDial1Changed(int)) );
             
    connect( m_vTickDial, SIGNAL( valueChanged(int) ),
             this,         SLOT  ( vTickDial1Changed(int)) );

    connect( m_hPosDial, SIGNAL( valueChanged(int) ),
             this,       SLOT  ( hPosDial1Changed(int)) );

    connect( m_vPosDial, SIGNAL( valueChanged(int) ),
             this,       SLOT  ( vPosDial1Changed(int)) );


    connect( m_hTickBox, &QDoubleSpinBox::editingFinished,
             this,  &DataPlotWidget::hTickBox1Changed );

    connect( m_vTickBox, &QDoubleSpinBox::editingFinished,
             this,  &DataPlotWidget::vTickBox1Changed );

    connect( m_hPosBox, &QDoubleSpinBox::editingFinished,
             this,  &DataPlotWidget::hPosBox1Changed );

    connect( m_vPosBox, &QDoubleSpinBox::editingFinished,
             this,  &DataPlotWidget::vPosBox1Changed );

    connect( m_buttonGroup, SIGNAL( buttonClicked(int)),
             this,          SLOT  ( buttonChanged(int)) );
}

QDial* DataPlotWidget::createDial()
{
    QDial* dial = new QDial(this);
    dial->setNotchesVisible( true );
    dial->setWrapping(true);
    dial->setMinimum( 1 );
    dial->setMaximum( 20 );
    dial->setSingleStep( 1 );
    return dial;
}

QDoubleSpinBox* DataPlotWidget::createSpinBox( QString suffix )
{
    QDoubleSpinBox* sb = new QDoubleSpinBox( this );
    sb->setMaximum( 1000000 );
    sb->setMinimum( 0.001 );
    sb->setDecimals( 3 );
    sb->setSuffix( suffix );
    sb->setValue( 0 );
    return sb;
}

QHBoxLayout* DataPlotWidget::createQHBoxLayout()
{
    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->setContentsMargins( 0, 0, 0, 0 );
    hbl->setAlignment( Qt::AlignLeft );
    return hbl;
}

bool DataPlotWidget::eventFilter( QObject* object, QEvent* event )
{
    if( event->type() == QEvent::FocusIn )
    {
        if( (object == m_hTickBox)
         || (object == m_hPosBox)
         || (object == m_vTickBox)
         || (object == m_vPosBox) )
        {
            Circuit::self()->deselectAll();
        }
    }
    return false;
}

void DataPlotWidget::avdChanged( bool ad ) { m_plotB->setAdvanc( ad ); }
void DataPlotWidget::autoChanged( bool au )
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        for( int i=0; i<2; i++ ) m_auto[i] = au;
    }
    else m_auto[ch] = au;
}
void DataPlotWidget::triggerChanged( bool tr )
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        m_plotB->setTrigger2( tr );
        m_plotB->setTrigger1( tr );
    }
    else
    {
        if( ch ) m_plotB->setTrigger2( tr );
        else     m_plotB->setTrigger1( tr );
    }
}
void DataPlotWidget::channel1Changed(){ setProbe( 0 ); }
void DataPlotWidget::channel2Changed(){ setProbe( 1 ); }

void DataPlotWidget::hTickDial1Changed( int Hscale )
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        hTickDialChanged( 0, Hscale );
        m_hTick[1] = m_hTick[0];
    }
    else hTickDialChanged( ch, Hscale );
}
void DataPlotWidget::vTickDial1Changed( int Vscale )
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        vTickDialChanged( 0, Vscale );
        setVTick( 1, m_vTick[0] );
    }
    else vTickDialChanged( ch, Vscale );
}
void DataPlotWidget::hPosDial1Changed( int hPos )
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        hPosDialChanged( 0, hPos );
        setHPos( 1, m_hPos[0] );
    }
    else hPosDialChanged( ch, hPos );
}
void DataPlotWidget::vPosDial1Changed( int vPos )
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        vPosDialChanged( 0, vPos );
        setVPos( 1, m_vPos[0] );
    }
    else vPosDialChanged( ch, vPos );
}

void DataPlotWidget::hTickBox1Changed()
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        hTickBoxChanged( 0 );
        m_hTick[1] = m_hTick[0];
    }
    else hTickBoxChanged( ch );
}
void DataPlotWidget::vTickBox1Changed()
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        vTickBoxChanged( 0 );
        setVTick( 1, m_vTick[0] );
    }
    else vTickBoxChanged( ch );
}
void DataPlotWidget::hPosBox1Changed()
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        hPosBoxChanged( 0 );
        setHPos( 1, m_hPos[0] );
    }
    else hPosBoxChanged( ch );
}
void DataPlotWidget::vPosBox1Changed()
{
    int ch = m_buttonGroup->checkedId();
    if( ch == 2 )
    {
        vPosBoxChanged( 0 );
        setVPos( 1, m_vPos[0] );
    }
    else vPosBoxChanged( ch );
}

void DataPlotWidget::setProbe( int ch )
{
    Probe* probe = 0l;
    Probe* oldProbe = m_probe[ch];
    QString nam = m_channel[ch]->text();

    Component* comp = Circuit::self()->getComponent( nam );
    if( comp ) probe = static_cast<Probe*>( comp );

    m_probe[ch] = probe;
    m_plotB->connectProbe( ch, probe!= 0l );
    m_plotB->m_probe[ch] = nam;

    if( oldProbe ) disconnect( oldProbe, SIGNAL( removed( Probe* ) ),
                               this,  SLOT  ( probeRemoved( Probe* )) );

    if( probe )  connect( probe, SIGNAL( removed( Probe* ) ),
                          this,  SLOT  ( probeRemoved( Probe* )) );
}

void DataPlotWidget::probeRemoved( Probe* p )
{
    for( int i=0; i<2; i++ )
    {
        if( m_probe[i] == p )
        {
            m_channel[i]->setText( "" );
            setProbe( i );
        }
    }
}

#include "moc_dataplotwidget.cpp"
