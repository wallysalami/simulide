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

#include "intcircuit.h"
#include "componentselector.h"
#include "propertieswidget.h"
#include "circuit.h"
#include "tunnel.h"
#include "utils.h"
#include "node.h"
#include "simuapi_apppath.h"

Component* IntCircuit::construct( QObject* parent, QString type, QString id )
{
    IntCircuit* intCircuit = new IntCircuit( parent, type,  id );
    if( m_error > 0 )
    {
        Circuit::self()->compList()->removeOne( intCircuit );
        intCircuit->deleteLater();
        intCircuit = 0l;
        m_error = 0;
    }
    return intCircuit;
}

LibraryItem* IntCircuit::libraryItem()
{
    return new LibraryItem(
        tr("IntCircuit"),
        tr(""),         // Not dispalyed
        "",
        "IntCircuit",
        IntCircuit::construct );
}

IntCircuit::IntCircuit( QObject* parent, QString type, QString id )
          : Chip( parent, type, id )
{
    QString compName = m_id.split("-").first(); // for example: "atmega328-1" to: "atmega328"
    QString dataFile = ComponentSelector::self()->getXmlFile( compName );

    m_mainComponent = 0l;

    //qDebug()<<"IntCircuit::IntCircuit"<<compName<<dataFile;

    if( dataFile == "" )
    {
          MessageBoxNB( "IntCircuit::IntCircuit", "                               \n"+
                    tr( "There are no data files for " )+compName+"    ");
          m_error = 23;
          return;
    }
    QFile file( dataFile );
    if( !file.open(QFile::ReadOnly | QFile::Text) )
    {
          MessageBoxNB( "IntCircuit::IntCircuit", "                               \n"+
                    compName+" "+ tr("Cannot read file %1:\n%2.").arg(dataFile).arg(file.errorString()));
          m_error = 21;
          return;
    }
    QDomDocument domDoc;
    if( !domDoc.setContent(&file) )
    {
         MessageBoxNB( "IntCircuit::IntCircuit", "                               \n"+
                   tr( "Cannot set file %1\nto DomDocument") .arg(dataFile));
         file.close();
         m_error = 22;
         return;
    }
    file.close();

    QDomElement root  = domDoc.documentElement();
    QDomNode    rNode = root.firstChild();

    while( !rNode.isNull() )
    {
        QDomElement element = rNode.toElement();
        QDomNode    node    = element.firstChild();

        while( !node.isNull() )         // Find the "package", for example 628A is package: 627A, Same pins
        {
            QDomElement element = node.toElement();
            
            if( element.attribute("name") == compName )
            {
                QDir dataDir(  dataFile );
                dataDir.cdUp();             // Indeed it doesn't cd, just take out file name
                
                m_pkgeFile = dataDir.filePath( element.attribute( "package" ) );
                if( !m_pkgeFile.endsWith( ".package" ) ) m_pkgeFile += ".package" ;
                
                QString subcFile = dataDir.filePath( element.attribute( "subcircuit" ) );
                if( !subcFile.endsWith( ".simu" ) ) subcFile += ".simu" ;

                if( element.hasAttribute( "icon" ) ) m_BackGround = ":/" + element.attribute( "icon" );

                //qDebug() << "IntCircuit::IntCircuit"<<m_pkgeFile <<m_subcFile ;
                loadSubCircuit( subcFile );
                break;
            }
            node = node.nextSibling();
        }
        rNode = rNode.nextSibling();
    }
    if( !m_error ) initChip();
}

IntCircuit::~IntCircuit()
{
}

void IntCircuit::loadSubCircuit( QString fileName )
{
    QFile file( fileName );

    //qDebug() << "Circuit::loadCircuit"<<m_filePath;

    if( !file.open(QFile::ReadOnly | QFile::Text) )
    {
        QMessageBox::warning(0l, "IntCircuit::loadSubcircuit",
        tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        m_error = 32;
        return;
    }
    QDomDocument domDoc;

    if( !domDoc.setContent(&file) )
    {
        QMessageBox::warning( 0l, "IntCircuit::loadSubcircuit",
        tr("Cannot set file %1\nto DomDocument").arg(fileName));
        file.close();
        m_error = 33;
        return;
    }
    file.close();

    loadDomDoc( &domDoc );

    /*if( m_error != 0 )
    {
        remove();
        for( Component* comp : m_compList ) removeComp( comp ); // Clean Nodes
    }*/
}

void IntCircuit::loadDomDoc( QDomDocument* doc )
{
    Circuit* circ = Circuit::self();
    QHash<QString, QString> idMap;
    QHash<QString, eNode*> nodMap;

    QDomNode node = doc->documentElement().firstChild();

    while( !node.isNull() )
    {
        QDomElement   element = node.toElement();
        const QString tagName = element.tagName();

        if( tagName == "item" )
        {
            QString objNam = element.attribute( "objectName"  ); // Data in simu file
            QString type   = element.attribute( "itemtype"  );
            QString id     = objNam.split("-").first()+"-"+circ->newSceneId(); // Create new id

            element.setAttribute( "objectName", id  );

            if( type == "Connector" )
            {
                Pin* startpin  = circ->getConPin( element.attribute( "startpinid" ), &idMap );
                Pin* endpin    = circ->getConPin( element.attribute( "endpinid" ), &idMap );

                if( startpin && endpin )    // Create Connector
                {
                    Connector* con  = new Connector( this, type, id, startpin, endpin );

                    element.setAttribute( "startpinid", startpin->objectName() );
                    element.setAttribute(   "endpinid", endpin->objectName() );

                    circ->loadProperties( element, con );

                    QString enodeId = element.attribute( "enodeid" );
                    eNode*  enode   = nodMap[enodeId];

                    if( !enode )          // Create eNode and add to enodList
                    {
                        enode = new eNode( "Circ_eNode-"+circ->newSceneId() );
                        nodMap[enodeId] = enode;
                    }
                    con->setEnode( enode );
                    con->setVisib( false );
                    con->setHidden( true );
                    con->setParentItem( this );
                    Circuit::self()->conList()->removeOne( con );
                }
                else // Start or End pin not found
                {
                    if( !startpin ) qDebug() << "\n   ERROR!!  Circuit::loadDomDoc:  null startpin in " << objNam;
                    if( !endpin )   qDebug() << "\n   ERROR!!  Circuit::loadDomDoc:  null endpin in "   << objNam;
                }
            }
            else if( type == "Package" )
            {
                ;
            }
            else
            {
                idMap[objNam] = id;        // Map simu id to new id

                Component* comp = 0l;
                if( type == "Node" ) comp = new Node( this, type, id );
                else                 comp = circ->createItem( type, id );

                if( comp )
                {
                    circ->loadProperties( element, comp );
                    comp->setParentItem( this );
                    comp->setHidden( true );
                    Circuit::self()->compList()->removeOne( comp );

                    if( comp->mainComp() ) m_mainComponent = comp; // This component will add it's Context Menu

                    if( type == "Tunnel" ) // Make Tunnel names unique for this subcircuit
                    {
                        Tunnel* tunnel = static_cast<Tunnel*>( comp );
                        tunnel->setUid( tunnel->name() );
                        tunnel->setName( m_id+"-"+tunnel->name() );
                    }
                    m_compList.append( comp );
                }
                else qDebug() << " ERROR Creating Component: "<< type << id;
            }
        }
        node = node.nextSibling();
    }
}

void IntCircuit::addPin(QString id, QString type, QString label, int pos, int xpos, int ypos, int angle, int length  )
{
    //qDebug() << "IntCircuit::addPin"<<id<<label;
    QColor color = Qt::black;
    if( !m_isLS ) color = QColor( 250, 250, 200 );

    Tunnel* tunnel = new Tunnel( this, "Tunnel", m_id+"-"+id );
    Circuit::self()->compList()->removeOne( tunnel );

    tunnel->setParentItem( this );
    tunnel->setAcceptedMouseButtons( 0 );
    tunnel->setShowId( false );
    tunnel->setName( m_id+"-"+id ); // Make tunel name unique for this component
    tunnel->setPos( xpos, ypos );
    tunnel->setPacked( true );

    Pin* pin = tunnel->getPin();
    pin->setLabelColor( color );
    pin->setLabelText( label );
    connect( this, SIGNAL( moved() ), pin, SLOT( isMoved() ) );

    if     ( type == "inverted" ) pin->setInverted( true );
    else if( type == "unused" )   pin->setUnused( true );
    else if( type == "null" )
    {
        pin->setVisible( false );
        pin->setLabelText( "" );
    }

    if     ( angle == 90 )  tunnel->setRotation( angle );
    else if( angle == 180 ) tunnel->setRotated( true );
    else if( angle == 270 ) tunnel->setRotation( angle );

    pin->setLength( length );
    pin->setFlag( QGraphicsItem::ItemStacksBehindParent, (length<8) );

    m_ePin[pos-1] = pin;
    m_pin[pos-1]  = pin;
}

void IntCircuit::remove()
{
    for( Component* comp : m_compList )
    {
        comp->setParentItem( 0l );
        Circuit::self()->removeComp( comp );
    }
    Component::remove();
}

void IntCircuit::slotProperties()
{
    if( m_properties ) m_propertiesW->show();
    else
    {
        Component::slotProperties();
        if( m_mainComponent )
            m_propertiesW->properties()->addObject( m_mainComponent );
    }
}

void IntCircuit::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !acceptedMouseButtons() ) event->ignore();
    else
    {
        event->accept();
        QMenu* menu = new QMenu();
        if( m_mainComponent ) m_mainComponent->contextMenu( event, menu );
        Component::contextMenu( event, menu );
        menu->deleteLater();
    }
}

#include "moc_intcircuit.cpp"

