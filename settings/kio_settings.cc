  
   #include <kio/slavebase.h>
   #include <kinstance.h>
   #include <kdebug.h>
   #include <stdlib.h>
   #include <qtextstream.h>
   #include <klocale.h>
   #include <sys/stat.h>
   #include <dcopclient.h>
   #include <qdatastream.h>
   #include <time.h>
   #include <kprocess.h>
   #include <kservice.h>
   #include <kservicegroup.h>
   #include <kstandarddirs.h>
   class SettingsProtocol : public KIO::SlaveBase
   {
   public:
      SettingsProtocol( const QCString &pool, const QCString &app);
      virtual ~SettingsProtocol();
#if 0
      virtual void get( const KURL& url );
#endif
      virtual void stat(const KURL& url);
      virtual void listDir(const KURL& url);
      void listRoot();
   private:
	DCOPClient *m_dcopClient;
  };

  extern "C" {
      int kdemain( int, char **argv )
      {
          kdDebug()<<"kdemain for devices"<<endl;
          KInstance instance( "kio_devices" );
          SettingsProtocol slave(argv[2], argv[3]);
          slave.dispatchLoop();
          return 0;
      }
  }



static void createFileEntry(KIO::UDSEntry& entry, const QString& name, const QString& url, const QString& mime,const QString& iconName);
static void createDirEntry(KIO::UDSEntry& entry, const QString& name, const QString& url, const QString& mime,const QString& iconName);

SettingsProtocol::SettingsProtocol( const QCString &pool, const QCString &app): SlaveBase( "devices", pool, app )
{
	m_dcopClient=new DCOPClient();
	if (!m_dcopClient->attach())
	{
		kdDebug()<<"ERROR WHILE CONNECTING TO DCOPSERVER"<<endl;
	}
}

SettingsProtocol::~SettingsProtocol()
{
	delete m_dcopClient;
}

void SettingsProtocol::stat(const KURL& url)
{
        QStringList     path = QStringList::split('/', url.encodedPathAndQuery(-1), false);
        KIO::UDSEntry   entry;
        QString mime;
	QString mp;

	QString relPath=url.path();
	if (!relPath.startsWith("/Settings")) relPath="Settings"+relPath;
	else relPath=relPath.right(relPath.length()-1);

	KServiceGroup::Ptr grp = KServiceGroup::group(relPath);	

	createDirEntry(entry, i18n("Settings"), url.url(), "inode/directory",grp->icon());
	statEntry(entry);
	finished();
	return;
#if 0
	switch (path.count())
	{
		case 0:
		        createDirEntry(entry, i18n("Settings"), "settings:/", "inode/directory");
		        statEntry(entry);
		        finished();
			break;
		default:

                QStringList info=deviceInfo(url.fileName());

                if (info.empty())
                {
                        error(KIO::ERR_SLAVE_DEFINED,i18n("Unknown device"));
                        return;
                }


                QStringList::Iterator it=info.begin();
                if (it!=info.end())
                {
                        QString device=*it; ++it;
                        if (it!=info.end())
                        {
				++it;
				if (it!=info.end())
				{
	                                QString mp=*it; ++it;++it;
	                                if (it!=info.end())
	                                {
	                                        bool mounted=((*it)=="true");
	                                        if (mounted)
	                                        {
//	                                                if (mp=="/") mp="";
	                                                redirection(mp);
	                                                finished();
	                                        }
	                                        else
	                                        {
							if (mp.startsWith("file:/"))
							{
			        	        	        KProcess *proc = new KProcess;
        		        	                 	*proc << "kio_devices_mounthelper";
                		                 		*proc << "-m" << url.url();
	                        		         	proc->start(KProcess::Block);
        	                        		 	delete proc;

	        		                        	redirection(mp);
        		        	                	finished();
							}
							else
								error(KIO::ERR_SLAVE_DEFINED,i18n("Device not accessible"));


//	                                                error(KIO::ERR_SLAVE_DEFINED,i18n("Device not mounted"));
	                                        }
	                                        return;
					}
                                }
                        }
                }
                error(KIO::ERR_SLAVE_DEFINED,i18n("Illegal data received"));
		return;
		break;
        }
#endif
}



void SettingsProtocol::listDir(const KURL& url)
{
	        
	KIO::UDSEntry   entry;
	uint count=0;

	
	QString relPath=url.path();
	if (!relPath.startsWith("/Settings")) relPath="Settings"+relPath;
	else relPath=relPath.right(relPath.length()-1);
	if (relPath.at(relPath.length()-1)!='/') relPath+="/";
	kdDebug()<<"SettingsProtocol: "<<relPath<<"***********************"<<endl;
	KServiceGroup::Ptr root = KServiceGroup::group(relPath);
    
    	if (!root || !root->isValid()) {
		error(KIO::ERR_SLAVE_DEFINED,i18n("Unknown settings direcory"));
        	return;
	}
        
       KServiceGroup::List list = root->entries(true, true);
	
       KServiceGroup::List::ConstIterator it = list.begin();


       for (; it != list.end(); ++it) {

            KSycocaEntry * e = *it;

 	       if (e->isType(KST_KServiceGroup)) {

        	    KServiceGroup::Ptr g(static_cast<KServiceGroup *>(e));
	            QString groupCaption = g->caption();

		    kdDebug()<<"Settings Protocol: before emptiness check"<<endl;
        	    // Avoid adding empty groups.
	            KServiceGroup::Ptr subMenuRoot = KServiceGroup::group(g->relPath());
        	    if (subMenuRoot->childCount() == 0)
                	continue;

		    kdDebug()<<"Settings Protocol: before dotfile check"<<endl;
	            // Ignore dotfiles.
        	    if ((g->name().at(0) == '.'))
	                continue;

		    count++;
		    kdDebug()<<"Settings Protocol: adding group entry"<<endl;
                    QString relPath=g->relPath();
		    relPath=relPath.right(relPath.length()-9); //Settings/ ==9
		    createDirEntry(entry, groupCaption, "settings:/"+relPath, "inode/directory",g->icon());
	            listEntry(entry, false);
	        }
        	else {
	            KService::Ptr s(static_cast<KService *>(e));
        	    //insertMenuItem(s, id++, -1, &suppressGenericNames);
		    QString desktopEntryPath=s->desktopEntryPath();
		    desktopEntryPath="file:"+locate("apps",desktopEntryPath);
		    createFileEntry(entry,s->name(),desktopEntryPath, "application/x-desktop",s->icon());
	            listEntry(entry, false);
	        }
	}

        totalSize(count);
	listEntry(entry, true);
	finished();
}



#if 0
 void HelloProtocol::get( const KURL& url )
 {
/*	mimeType("application/x-desktop");
	QCString output;
	output.sprintf("[Desktop Action Format]\n"
			"Exec=kfloppy\n"
			"Name=Format\n"
			"[Desktop Entry]\n"
			"Actions=Format\n"
			"Dev=/dev/fd0\n"
			"Encoding=UTF-8\n"
			"Icon=3floppy_mount\n"
			"MountPoint=/media/floppy\n"
			"ReadOnly=false\n"
			"Type=FSDevice\n"
			"UnmountIcon=3floppy_unmount\n"
			);
     data(output);
     finished();
 */
  redirection("file:/");
  //finished();
}
#endif

void addAtom(KIO::UDSEntry& entry, unsigned int ID, long l, const QString& s = QString::null)
{
        KIO::UDSAtom    atom;
        atom.m_uds = ID;
        atom.m_long = l;
        atom.m_str = s;
        entry.append(atom);
}

static void createFileEntry(KIO::UDSEntry& entry, const QString& name, const QString& url, const QString& mime,const QString& iconName)
{
        entry.clear();
        addAtom(entry, KIO::UDS_NAME, 0, name);
//        addAtom(entry, KIO::UDS_FILE_TYPE, S_IFDIR);//REG);
        addAtom(entry, KIO::UDS_URL, 0, url);
        addAtom(entry, KIO::UDS_ACCESS, 0500);
        addAtom(entry, KIO::UDS_MIME_TYPE, 0, mime);
        addAtom(entry, KIO::UDS_SIZE, 0);
        addAtom(entry, KIO::UDS_GUESSED_MIME_TYPE, 0, "application/x-desktop");
	addAtom(entry, KIO::UDS_CREATION_TIME,1);
	addAtom(entry, KIO::UDS_MODIFICATION_TIME,time(0));
//	addAtom(entry, KIO::UDS_ICON_NAME,0,iconName);

}


static void createDirEntry(KIO::UDSEntry& entry, const QString& name, const QString& url, const QString& mime,const QString& iconName)
{
        entry.clear();
        addAtom(entry, KIO::UDS_NAME, 0, name);
        addAtom(entry, KIO::UDS_FILE_TYPE, S_IFDIR);
        addAtom(entry, KIO::UDS_ACCESS, 0500);
        addAtom(entry, KIO::UDS_MIME_TYPE, 0, mime);
        addAtom(entry, KIO::UDS_URL, 0, url);
        addAtom(entry, KIO::UDS_SIZE, 0);
        addAtom(entry, KIO::UDS_GUESSED_MIME_TYPE, 0, "inode/directory");
	addAtom(entry, KIO::UDS_ICON_NAME,0,iconName);
//        addAtom(entry, KIO::UDS_GUESSED_MIME_TYPE, 0, "application/x-desktop");
}
