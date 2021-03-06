/***************************************************************************
 *   Copyright (C) 202 by santiago González                               *
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

#ifndef TUNNEL_H
#define TUNNEL_H

#include "component.h"
#include "itemlibrary.h"
#include "e-node.h"

class Pin;

class MAINMODULE_EXPORT Tunnel : public Component, public eElement
{
    Q_OBJECT
    Q_PROPERTY( QString  Uid     READ uid     WRITE setUid )
    Q_PROPERTY( QString  Name    READ name    WRITE setName    DESIGNABLE true USER true )
    Q_PROPERTY( bool     Rotated READ rotated WRITE setRotated DESIGNABLE true USER true )

    public:
        Tunnel( QObject* parent, QString type, QString id );
        ~Tunnel();

 static Component* construct( QObject* parent, QString type, QString id );
 static LibraryItem *libraryItem();
        
        QString name() { return m_name; }
        void setName( QString name );

        bool rotated() { return m_rotated; }
        void setRotated( bool rot );

        virtual void attach();
        virtual void remove();

        QString uid() { return m_uid; }
        void setUid( QString uid ) { m_uid = uid; }

        void setPacked( bool p );

        Pin* getPin() { return m_pin[0]; }

        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );

    protected:
        void removeTunnel();

        int m_size;
        QString m_name;
        QString m_uid;

        bool m_rotated;
        bool m_packed;

 static QHash<QString, eNode*> m_nodes;
 static QHash<QString, int> m_count;
};


#endif
