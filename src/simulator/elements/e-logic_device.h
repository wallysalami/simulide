/***************************************************************************
 *   Copyright (C) 2010 by santiago González                               *
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

#ifndef ELOGICDEVICE_H
#define ELOGICDEVICE_H

#include <string>
#include <math.h>

#include "e-source.h"
#include "e-pin.h"

class MAINMODULE_EXPORT eLogicDevice : public eElement
{
    public:

        eLogicDevice( std::string id );
        ~eLogicDevice();

        int  numInps() const            { return m_numInputs; }
        virtual void setNumInps( int inputs );
                                            
        int  numOuts() const            { return m_numOutputs; }
        void setNumOuts( int outputs );

        double inputHighV() const          { return m_inputHighV; }
        void  setInputHighV( double volt ) { m_inputHighV = volt; }

        double inputLowV() const          { return m_inputLowV; }
        void  setInputLowV( double volt ) { m_inputLowV = volt; }

        double outHighV() const           { return m_outHighV; }
        void  setOutHighV( double volt );

        double outLowV() const            { return m_outLowV; }
        void  setOutLowV( double volt );

        double inputImp() const           { return m_inputImp; }
        void  setInputImp( double imp );

        double outImp() const            { return m_outImp; }
        void  setOutImp( double imp );

        bool clockInv() const            { return m_clockSource->isInverted(); }
        void setClockInv( bool inv );

        bool inverted() { return m_inverted; }
        void setInverted( bool inverted );

        bool invertInps() { return m_invInputs; }
        void setInvertInps( bool invert );

        int eTrigger() { return m_etrigger; }
        virtual void seteTrigger( int trigger );
        
        void setOutputEnabled( bool enabled );
        void updateOutEnabled();

        void initEpins(){;}

        virtual ePin* getEpin( QString pinName );

        int getClockState();
        bool outputEnabled();

        virtual void stamp();
        virtual void resetState();

        virtual void createPins( int inputs, int outputs );
        void setClockPin( eSource* clockSource) { m_clockSource = clockSource; }
        void setInput( int n, eSource* input );
        void createClockPin();
        void createOutEnablePin();

    protected:
        void createClockPin( ePin* epin );
        void createClockeSource( ePin* epin );
        void createOutEnablePin( ePin* epin );
        void createOutEnableeSource( ePin* epin );
        void createInput( ePin* epin );
        void createOutput( ePin* epin );
        
        void createInputs( int inputs );
        void createOutputs( int outputs );
        void deleteInputs( int inputs );
        void deleteOutputs( int inputs );
        void setOut( int num, bool out );
        
        bool getInputState( int input );
        bool getOutputState( int output );

        double m_inputHighV;
        double m_inputLowV;
        double m_outHighV;
        double m_outLowV;

        double m_inputImp;
        double m_outImp;

        int m_numInputs;
        int m_numOutputs;

        int m_etrigger;

        bool m_clock;
        bool m_outEnable;
        bool m_inverted;
        bool m_invInputs;

        eSource* m_outEnSource;
        eSource* m_clockSource;

        std::vector<eSource*> m_output;
        std::vector<eSource*> m_input;
        std::vector<bool>     m_inputState;
};

#endif

