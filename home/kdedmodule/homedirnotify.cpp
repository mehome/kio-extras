/* This file is part of the KDE Project
   Copyright (c) 2005 Kévin Ottens <ervin ipsquad net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "homedirnotify.h"

#include <kdebug.h>
#include <kuser.h>

#include <kdirnotify.h>
//Added by qt3to4:
#include <QList>

#define MINIMUM_UID 500

HomeDirNotify::HomeDirNotify()
: mInited( false )
{
	QDBus::sessionBus().connect(QString(), QString(), "org.kde.KDirNotify",
				    "FilesAdded", this, SLOT(FilesAdded(QString)));
	QDBus::sessionBus().connect(QString(), QString(), "org.kde.KDirNotify",
				    "FilesRemoved", this, SLOT(FilesRemoved(QStringList)));
	QDBus::sessionBus().connect(QString(), QString(), "org.kde.KDirNotify",
				    "FilesChanged", this, SLOT(FilesChanged(QStringList)));
}

void HomeDirNotify::init()
{
	if( mInited )
		return;
	mInited = true;

	KUser current_user;
	QList<KUserGroup> groups = current_user.groups();
	QList<int> uid_list;

	QList<KUserGroup>::iterator groups_it = groups.begin();
	QList<KUserGroup>::iterator groups_end = groups.end();

	for(; groups_it!=groups_end; ++groups_it)
	{
		QList<KUser> users = (*groups_it).users();

		QList<KUser>::iterator it = users.begin();
		QList<KUser>::iterator users_end = users.end();

		for(; it!=users_end; ++it)
		{
			if ((*it).uid()>=MINIMUM_UID
			 && !uid_list.contains( (*it).uid() ) )
			{
				uid_list.append( (*it).uid() );
				
				QString name = (*it).loginName();
				KUrl url;
				url.setPath( (*it).homeDir() );

				m_homeFoldersMap[name] = url;
			}
		}
	}
}

KUrl HomeDirNotify::toHomeURL(const KUrl &url)
{
	kDebug() << "HomeDirNotify::toHomeURL(" << url << ")" << endl;
	
	init();
	QMap<QString,KUrl>::iterator it = m_homeFoldersMap.begin();
	QMap<QString,KUrl>::iterator end = m_homeFoldersMap.end();
	
	for (; it!=end; ++it)
	{
		QString name = it.key();
		KUrl base = it.value();
		
		if ( base.isParentOf(url) )
		{
			QString path = KUrl::relativePath(base.path(),
			                                  url.path());
			KUrl result("home:/"+name+'/'+path);
			result.cleanPath();
			kDebug() << "result => " << result << endl;
			return result;
		}
	}

	kDebug() << "result => KUrl()" << endl;
	return KUrl();
}

KUrl::List HomeDirNotify::toHomeURLList(const KUrl::List &list)
{
	init();
	KUrl::List new_list;

	KUrl::List::const_iterator it = list.begin();
	KUrl::List::const_iterator end = list.end();

	for (; it!=end; ++it)
	{
		KUrl url = toHomeURL(*it);

		if (url.isValid())
		{
			new_list.append(url);
		}
	}

	return new_list;
}

void HomeDirNotify::FilesAdded(const QString &directory)
{
	kDebug() << "HomeDirNotify::FilesAdded" << endl;
	
	KUrl new_dir = toHomeURL(directory);

	if (new_dir.isValid())
	{
		org::kde::KDirNotify::emitFilesAdded( new_dir.url() );
	}
}

// This hack is required because of the way we manage .desktop files with
// Forwarding Slaves, their URL is out of the ioslave (some home:/ files
// have a file:/ based UDS_URL so that they are executed correctly.
// Hence, FilesRemoved and FilesChanged does nothing... We're forced to use
// FilesAdded to re-list the modified directory.
inline void evil_hack(const KUrl::List &list)
{
	KUrl::List notified;
	
	KUrl::List::const_iterator it = list.begin();
	KUrl::List::const_iterator end = list.end();

	for (; it!=end; ++it)
	{
		KUrl url = (*it).upUrl();

		if (!notified.contains(url))
		{
			org::kde::KDirNotify::emitFilesAdded(url.url());
			notified.append(url);
		}
	}
}


void HomeDirNotify::FilesRemoved(const QStringList &fileList)
{
	kDebug() << "HomeDirNotify::FilesRemoved" << endl;
	
	KUrl::List new_list = toHomeURLList(fileList);

	if (!new_list.isEmpty())
	{
		//KDirNotify_stub notifier("*", "*");
		//notifier.FilesRemoved( new_list );
		evil_hack(new_list);
	}
}

void HomeDirNotify::FilesChanged(const QStringList &fileList)
{
	kDebug() << "HomeDirNotify::FilesChanged" << endl;
	
	KUrl::List new_list = toHomeURLList(fileList);

	if (!new_list.isEmpty())
	{
		//KDirNotify_stub notifier("*", "*");
		//notifier.FilesChanged( new_list );
		evil_hack(new_list);
	}
}

#include "homedirnotify.moc"
