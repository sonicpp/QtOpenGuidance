// Copyright( C ) 2020 Christian Riggenbach
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

#ifndef ORIENTATIONDOCK_H
#define ORIENTATIONDOCK_H

#include <QGroupBox>
#include <QQuaternion>

namespace Ui {
  class OrientationDock;
}

class OrientationDock : public QGroupBox {
    Q_OBJECT

  public:
    explicit OrientationDock( QWidget* parent = nullptr );
    ~OrientationDock();

  public slots:
    void setOrientation( QQuaternion orientation );
    void setName( const QString& name );

    void setCaptionEnabled( bool enabled );
    void setFontOfLabel( const QFont& font );

  private:
    Ui::OrientationDock* ui = nullptr;

  public:
    const QFont& fontOfLabel();
    bool captionEnabled();

    int precision = 0;
    int fieldWidth = 0;
    double scale = 1;
};

#endif // ORIENTATIONDOCK_H