/*  This file is part of the KDE libraries
    Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <qimage.h>

#include <kimageio.h>
#include <kdebug.h>

#include <ImfInputFile.h>
#include <ImfPreviewImage.h>

#include "exrcreator.h"

extern "C"
{
    ThumbCreator *new_creator()
    {
        return new EXRCreator;
    }
}

bool EXRCreator::create(const QString &path, int, int, QImage &img)
{
    Imf::InputFile in ( path.ascii() );
    const Imf::Header &h = in.header();

    if ( h.hasPreviewImage() ) {
	kdDebug() << "EXRcreator - using preview" << endl;
	const Imf::PreviewImage &preview = in.header().previewImage();
	QImage qpreview(preview.width(), preview.height(), 32, 0, QImage::BigEndian);
	for ( unsigned int y=0; y < preview.height(); y++ ) {
	    for ( unsigned int x=0; x < preview.width(); x++ ) {
		const Imf::PreviewRgba &q = preview.pixels()[x+(y*preview.width())];
		qpreview.setPixel( x, y, qRgba(q.r, q.g, q.b, q.a) );
	    }
	}
	img = qpreview;
	return true;
    } else {
        // do it the hard way
	kdDebug() << "EXRcreator - using original image" << endl;
	if (!img.load( path ))
	    return false;
	if (img.depth() != 32)
	    img = img.convertDepth( 32 );
	return true;
    }
}

ThumbCreator::Flags EXRCreator::flags() const
{
    return None;
}