/* This file is part of KDE
   Copyright (C) 2000 by Wolfram Diestel <wolfram@steloj.de>
   Copyright (C) 2005 by Tim Way <tim@way.hrcoxmail.com>
   Copyright (C) 2005 by Volker Krause <volker.krause@rwth-aachen.de>

   This is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.
*/

#ifndef _NNTP_H
#define _NNTP_H

#include <qstring.h>
#include <kio/global.h>
#include <kio/tcpslavebase.h>

#define MAX_PACKET_LEN 4096

/* TODO:
  - test special post command
  - progress information in get, and maybe post
  - remove unnecessary debug stuff
*/

class NNTPProtocol:public KIO::TCPSlaveBase
{

 public:
  /** Default Constructor
   * @param isSSL is a true or false to indicate whether ssl is to be used
   */
  NNTPProtocol ( const QCString & pool, const QCString & app, bool isSSL );
  virtual ~NNTPProtocol();

  virtual void get(const KURL& url );
  virtual void stat(const KURL& url );
  virtual void listDir(const KURL& url );
  virtual void setHost(const QString& host, int port,
        const QString& user, const QString& pass);

  /**
    *  Special command: 1 = post article
    *  it takes no other args, the article data are
    *  requested by dataReq() and should be valid
    *  as in RFC850. It's not checked for correctness here.
    */
  virtual void special(const QByteArray& data);

 protected:

  /**
    *  Send a command to the server. Returns the response code and
    *  the response line
    */
  int sendCommand( const QString &cmd );

  /**
    *  Attempt to properly shut down the NNTP connection by sending
    *  "QUIT\r\n" before closing the socket.
    */
  void nntp_close ();

  /**
    * Attempt to initiate a NNTP connection via a TCP socket, if no existing
    * connection could be reused.
    */
  bool nntp_open();

  /**
    * Post article. Invoked by special()
    */
  bool post_article();


 private:
   QString mHost, mUser, mPass;
   bool postingAllowed, opened;
   char readBuffer[MAX_PACKET_LEN];
   ssize_t readBufferLen;

   /// fetch all available news groups
   void fetchGroups ();
   /// fetch all messages from one news group
   bool fetchGroup ( QString & group );
   /// creates an UDSEntry with file information used in stat and listDir
   void fillUDSEntry ( KIO::UDSEntry & entry, const QString & name, int size,
                       bool postingAllowed, bool is_article );
   /// error  handling for unexpected responses
   void unexpected_response ( int res_code, const QString & command );
   /**
     * grabs the response line from the server. used after most send_cmd calls. max
     * length for the returned string ( char *data ) is 4096 characters including
     * the "\r\n" terminator.
     */
   int evalResponse ( char *data, ssize_t &len );
};

#endif
