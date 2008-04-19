/***************************************************************************
 *   Copyright (C) 2007 by Tobias Koenig <tokoe@kde.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "generator_comicbook.h"

#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <okular/core/document.h>
#include <okular/core/page.h>
#include <okular/core/fileprinter.h>

OKULAR_EXPORT_PLUGIN( ComicBookGenerator, "okular_comicbook" )

ComicBookGenerator::ComicBookGenerator( QObject *parent, const QVariantList &args )
    : Generator( parent, args )
{
    setFeature( Threaded );
    setFeature( PrintNative );
    setFeature( PrintToFile );
}

ComicBookGenerator::~ComicBookGenerator()
{
}

bool ComicBookGenerator::loadDocument( const QString & fileName, QVector<Okular::Page*> & pagesVector )
{
    if ( !mDocument.open( fileName ) )
        return false;

    int pages = mDocument.pages();
    pagesVector.resize( pages );

    QSize aux;
    for ( int i = 0; i < pages; ++i ) {
         aux = mDocument.pageImage( i ).size();

         if ( aux.height() > aux.width() ) {
             Okular::Page * page = new Okular::Page( i, 600, 800, Okular::Rotation0 );
             pagesVector[i] = page;
         }
         else {
             Okular::Page * page = new Okular::Page( i, 800, 600, Okular::Rotation0 );
             pagesVector[i] = page;
         }
    }

    return true;
}

bool ComicBookGenerator::doCloseDocument()
{
    mDocument.close();

    return true;
}

QImage ComicBookGenerator::image( Okular::PixmapRequest * request )
{
    int width = request->width();
    int height = request->height();

    QImage image = mDocument.pageImage( request->pageNumber() );

    return image.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
}

bool ComicBookGenerator::print( QPrinter& printer )
{
    QPainter p( &printer );

    QList<int> pageList = Okular::FilePrinter::pageList( printer, document()->pages(),
                                                         document()->bookmarkedPageList() );

    for ( int i = 0; i < pageList.count(); ++i ) {

        QImage image = mDocument.pageImage( pageList[i] - 1 );

        if ( ( image.width() > printer.width() ) || ( image.height() > printer.height() ) )

            image = image.scaled( printer.width(), printer.height(),
                                  Qt::KeepAspectRatio, Qt::SmoothTransformation );

        if ( i != 0 )
            printer.newPage();

        p.drawImage( 0, 0, image );
    }

    return true;
}

#include "generator_comicbook.moc"

