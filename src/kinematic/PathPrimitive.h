// Copyright( C ) 2019 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#ifndef PATHPRIMITIVE_H
#define PATHPRIMITIVE_H

#include <QObject>

#include <QtWidgets>
#include <QComboBox>

#include <QJsonObject>

#include <Qt3DCore/QEntity>

class PathPrimitive : public QObject {
    Q_OBJECT

  public:
    PathPrimitive( double x1, double y1, double x2, double y2, bool anyDirection ):
      x1( x1 ), y1( y1 ), x2( x2 ), y2( y2 ), anyDirection( anyDirection ) {}

  public:
    double x1, y1;
    double x2, y2;

    bool anyDirection;
};

class PathPrimitiveLine : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveLine( double x1, double y1, double x2, double y2, bool isSegment, bool anyDirection )
      : PathPrimitive( x1, y1, x2, y2, anyDirection ), isSegment( isSegment ) {
      qDebug() << "PathPrimitiveLine()";
    }

    ~PathPrimitiveLine() {
      qDebug() << "~PathPrimitiveLine()";
    }

  public:
    bool isSegment;
};

class PathPrimitiveCircle : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveCircle( double x1, double y1, double x2, double y2, double xCenter, double yCenter, bool anyDirection )
      : PathPrimitive( x1, y1, x2, y2, anyDirection ),
        xCenter( xCenter ), yCenter( yCenter ) {}

  public:
    double xCenter, yCenter;
};

#endif // PATHPRIMITIVE_H
