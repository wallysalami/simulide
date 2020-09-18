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

#ifndef VARRESBASE_H
#define VARRESBASE_H

#include "e-resistor.h"
#include "component.h"
#include "dialwidget.h"


class MAINMODULE_EXPORT VarResBase : public Component, public eResistor
{
    Q_OBJECT
    Q_PROPERTY( int    Value_Ohm      READ val    WRITE setVal    DESIGNABLE true USER true )

    public:
        VarResBase( QObject* parent, QString type, QString id );
        ~VarResBase();

        double maxRes();
        void setMaxRes( double r );

        double minRes();
        void setMinRes( double r );

        void setVal( int val );
        int val();

        virtual void initialize();
        virtual void remove();

    public slots:
        void resChanged( int volt );

    protected:
        double m_maxRes;
        double m_minRes;

        bool m_changed;

        DialWidget m_dialW;
        QDial* m_dial;
        QGraphicsProxyWidget* m_proxy;
};

#endif
