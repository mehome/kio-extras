/* This file is part of the KDE Project
   Copyright (c) 2004 K�vin Ottens <ervin ipsquad net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "mediamanager.h"

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

#include <kdirnotify_stub.h>

#include "fstabbackend.h"

MediaManager::MediaManager(const QCString &obj)
    : KDEDModule(obj)
{
	connect( &m_mediaList, SIGNAL(mediumAdded(const QString&, const QString&)),
	         SLOT(slotMediumAdded(const QString&, const QString&)) );
	connect( &m_mediaList, SIGNAL(mediumRemoved(const QString&, const QString&)),
	         SLOT(slotMediumRemoved(const QString&, const QString&)) );
	connect( &m_mediaList,
	         SIGNAL(mediumStateChanged(const QString&, const QString&)),
	         SLOT(slotMediumChanged(const QString&, const QString&)) );

	m_backends.setAutoDelete(true);

	m_backends.append( new FstabBackend(m_mediaList) );
}


QStringList MediaManager::fullList()
{
	QPtrList<Medium> list = m_mediaList.list();

	QStringList result;

	QPtrList<Medium>::const_iterator it = list.begin();
	QPtrList<Medium>::const_iterator end = list.end();
	for (; it!=end; ++it)
	{
		result+= (*it)->properties();
		result+= Medium::SEPARATOR;
	}

	return result;
}

QStringList MediaManager::properties(const QString &name)
{
	const Medium *m = m_mediaList.findByName(name);

	if (m!=0L)
	{
		return m->properties();
	}
	else
	{
		return QStringList();
	}
}

ASYNC MediaManager::setUserLabel(const QString &name, const QString &label)
{
	m_mediaList.setUserLabel(name, label);
}

void MediaManager::slotMediumAdded(const QString &/*id*/, const QString &name)
{
	kdDebug() << "MediaManager::slotMediumAdded: " << name << endl;
	
	KDirNotify_stub notifier("*", "*");
	notifier.FilesAdded( KURL("media:/") );

	emit mediumAdded(name);
}

void MediaManager::slotMediumRemoved(const QString &/*id*/, const QString &name)
{
	kdDebug() << "MediaManager::slotMediumRemoved: " << name << endl;
	
	KDirNotify_stub notifier("*", "*");
	notifier.FilesRemoved( KURL("media:/"+name) );

	emit mediumRemoved(name);
}

void MediaManager::slotMediumChanged(const QString &/*id*/, const QString &name)
{
	kdDebug() << "MediaManager::slotMediumChanged: " << name << endl;
	
	KDirNotify_stub notifier("*", "*");
	notifier.FilesChanged( KURL("media:/"+name) );

	emit mediumChanged(name);
}


extern "C" {
    KDEDModule *create_mediamanager(const QCString &obj)
    {
        KGlobal::locale()->insertCatalogue("kio_media");
        return new MediaManager(obj);
    }
}

#include "mediamanager.moc"