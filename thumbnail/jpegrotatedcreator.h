/*  This file is part of the KDE libraries
    Copyright (C) 2008 Andre Gemünd <scroogie@gmail.com>

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef _JPEGROTATEDCREATOR_H_
#define _JPEGROTATEDCREATOR_H_

#include <kio/thumbcreator.h>

class QTransform;

/**
 * Creates thumbs for JPEG images and automatically rotates
 * the output corresponding to rotation stored inside EXIF.
 */
class JpegRotatedCreator : public ThumbCreator
{
public:
    JpegRotatedCreator();
    virtual bool create(const QString &path, int, int, QImage &img);
    virtual Flags flags() const;

private:
    QTransform orientationMatrix(int exivOrientation) const;
};

#endif