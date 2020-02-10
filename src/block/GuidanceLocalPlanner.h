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

#ifndef LOCALPLANNER_H
#define LOCALPLANNER_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include <QVector>
#include <QSharedPointer>

class LocalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit LocalPlanner()
      : BlockBase() {}

  public slots:
    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;


        // get nearest line/segment
        double distance = qInf();
//        double headingOfABLine = 0;

        QSharedPointer<PathPrimitive> lineBuffer;

        for( const auto& primitive : qAsConst( plan ) ) {
          auto* line = qobject_cast<PathPrimitiveLine*>( primitive.data() );

          if( line ) {
            double distanceTmp = line->distanceToPoint( QPointF( double( position.x() ), double( position.y() ) ) );

            if( qAbs( distanceTmp ) < qAbs( distance ) ) {
              lineBuffer = primitive;
//              headingOfABLine = line->line.angle();
              distance = distanceTmp;
            }

//            qDebug() << distance << distanceTmp << headingOfABLine << line->line;
          }
        }

        // make a new plan with the nearest line, reverse lines that have anyDirection==true
        auto* line = qobject_cast<PathPrimitiveLine*>( lineBuffer.data() );

        if( line ) {
          QVector<QSharedPointer<PathPrimitive>> planTmp;

          if( line->anyDirection ) {
            qreal angleToHeading = line->line.angleTo( QLineF::fromPolar( 100, -qreal( orientation.toEulerAngles().z() ) ) );

            if( !( ( angleToHeading < 80 ) || ( angleToHeading > ( 360 - 80 ) ) ) ) {
              qDebug() << "line->reverse()";
              line->reverse();
            }
          }

          planTmp << lineBuffer;
          emit planChanged( planTmp );
        }
      }
    }

    void setPlan( const QVector<QSharedPointer<PathPrimitive>>& plan ) {
      this->plan = plan;
      emit planChanged( plan );
    }


  signals:
    void planChanged( QVector<QSharedPointer<PathPrimitive>> );

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

  private:
    QVector<QSharedPointer<PathPrimitive>> plan;
};

class LocalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    LocalPlannerFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Local Planner" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new LocalPlanner();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( QVector<QSharedPointer<PathPrimitive>> ) ) ) );
      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const QVector<QSharedPointer<PathPrimitive>>& ) ) ) );

      return b;
    }
};

#endif // LOCALPLANNER_H

