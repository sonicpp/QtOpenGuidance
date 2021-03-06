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

#pragma once

#include <QObject>

#include <QQuaternion>
#include <QVector3D>
#include <QPointF>
#include <QPolygonF>
#include <QLineF>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../3d/BufferMesh.h"

#include "../gui/FieldsOptimitionToolbar.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"
#include "../kinematic/Plan.h"

#include "../kinematic/GeographicConvertionWrapper.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

class CgalThread;
class CgalWorker;

class GlobalPlannerLines : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlannerLines( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw );

    ~GlobalPlannerLines() {}

  public slots:
    void setPose( const Point_3& position, const QQuaternion orientation, const PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;

        aPointTransform->setRotation( orientation );
        bPointTransform->setRotation( orientation );

        createPlanAB();
        showPlan();
      }
    }

    void setPoseLeftEdge( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = position;

        auto point2D = to2D( position );

        if( implementSegment.source() != point2D ) {
          implementSegment = Segment_2( point2D, implementSegment.target() );
          createPlanAB();
        }
      }
    }

    void setPoseRightEdge( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = position;

        auto point2D = to2D( position );

        if( implementSegment.target() != point2D ) {
          implementSegment = Segment_2( implementSegment.source(), point2D );
          createPlanAB();
        }
      }
    }

    void setField( std::shared_ptr<Polygon_with_holes_2> field ) {
      currentField = field;
    }

    void a_clicked() {
      aPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );

      aPoint = position;

      qDebug() << "a_clicked()"/* << aPoint*/;
    }

    void b_clicked() {
      qDebug() << "b_clicked()";
      bPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );
      bPointEntity->setEnabled( true );

      bPoint = position;

      abSegment = Segment_3( aPoint, bPoint );

      clearPlan();
      createPlanAB();
    }

    void snap_clicked() {
      snapPlanAB();
      qDebug() << "snap_clicked()";
    }

    void turnLeft_clicked() {
      qDebug() << "turnLeft_clicked()";
    }
    void turnRight_clicked() {
      qDebug() << "turnRight_clicked()";
    }

    void setPlannerSettings( int pathsToGenerate, int pathsInReserve ) {
      this->pathsToGenerate = pathsToGenerate;
      this->pathsInReserve = pathsInReserve;

//      createPlanAB();
    }

    void setPassSettings( int forwardPasses, int reversePasses, bool startRight, bool mirror ) {
      if( ( forwardPasses == 0 || reversePasses == 0 ) ) {
        this->forwardPasses = 0;
        this->reversePasses = 0;
      } else {
        this->forwardPasses = forwardPasses;
        this->reversePasses = reversePasses;
      }

      this->startRight = startRight;
      this->mirror = mirror;

//      createPlanAB();
    }

    void setPassNumberTo( int /*passNumber*/ ) {}

    void setRunNumber( uint32_t runNumber ) {
      this->runNumber = runNumber;
    }

  signals:
    void planChanged( const Plan& );

//    void alphaChanged( double optimal, double solid );
//    void fieldStatisticsChanged( double, double, double );
//    void requestFieldOptimition( uint32_t runNumber,
//                                 std::vector<K::Point_2>* points,
//                                 FieldsOptimitionToolbar::AlphaType alphaType,
//                                 double customAlpha,
//                                 double maxDeviation,
//                                 double distanceBetweenConnectPoints );
//    void requestNewRunNumber();

  private:
    bool isLineAlreadyInPlan( const std::shared_ptr<PathPrimitiveLine>& line );
    void createPlanAB();
    void snapPlanAB();
    void sortPlan();
    void showPlan();
    void clearPlan();

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    QQuaternion orientation = QQuaternion();

    int pathsToGenerate = 5;
    int pathsInReserve = 3;
    int forwardPasses = 0;
    int reversePasses = 0;
    bool startRight = false;
    bool mirror = false;

    std::shared_ptr<Polygon_with_holes_2> currentField;

    Point_3 aPoint = Point_3( 0, 0, 0 );
    Point_3 bPoint = Point_3( 0, 0, 0 );
    Segment_3 abSegment = Segment_3( Point_3( 0, 0, 0 ), Point_3( 0, 0, 0 ) );

    Segment_2 implementSegment = Segment_2( Point_2( 0, 0 ), Point_2( 0, 0 ) );

    Point_3 positionLeftEdgeOfImplement = Point_3( 0, 0, 0 );
    Point_3 positionRightEdgeOfImplement = Point_3( 0, 0, 0 );

    Plan plan = Plan( Plan::Type::OnlyLines );

  private:
    QWidget* mainWindow = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    GeographicConvertionWrapper* tmw = nullptr;

    // markers
    Qt3DCore::QEntity* aPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* aPointMesh = nullptr;
    Qt3DCore::QTransform* aPointTransform = nullptr;
    Qt3DCore::QEntity* aTextEntity = nullptr;
    Qt3DCore::QTransform* aTextTransform = nullptr;

    Qt3DCore::QEntity* bPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* bPointMesh = nullptr;
    Qt3DCore::QTransform* bPointTransform = nullptr;
    Qt3DCore::QEntity* bTextEntity = nullptr;
    Qt3DCore::QTransform* bTextTransform = nullptr;

  private:
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    CgalThread* threadForCgalWorker = nullptr;
    CgalWorker* cgalWorker = nullptr;
    uint32_t runNumber = 0;

    Qt3DCore::QEntity* m_pointsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity2 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity3 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity4 = nullptr;
    BufferMesh* m_pointsMesh = nullptr;
    BufferMesh* m_segmentsMesh = nullptr;
    BufferMesh* m_segmentsMesh2 = nullptr;
    BufferMesh* m_segmentsMesh3 = nullptr;
    BufferMesh* m_segmentsMesh4 = nullptr;
    Qt3DExtras::QPhongMaterial* m_pointsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial2 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial3 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial4 = nullptr;
};

class GlobalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    GlobalPlannerFactory( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
      : BlockFactory(),
        mainWindow( mainWindow ),
        rootEntity( rootEntity ),
        tmw( tmw ) {
      qRegisterMetaType<Plan>();
    }

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner Lines" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }


    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new GlobalPlannerLines( mainWindow, rootEntity, tmw );
      auto* b = createBaseBlock( scene, obj, id, true );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );

      b->addInputPort( QStringLiteral( "Field" ), QLatin1String( SLOT( setField( std::shared_ptr<Polygon_with_holes_2> ) ) ) );

      b->addInputPort( QStringLiteral( "A clicked" ), QLatin1String( SLOT( a_clicked() ) ) );
      b->addInputPort( QStringLiteral( "B clicked" ), QLatin1String( SLOT( b_clicked() ) ) );
      b->addInputPort( QStringLiteral( "Snap clicked" ), QLatin1String( SLOT( snap_clicked() ) ) );
      b->addInputPort( QStringLiteral( "Turn Left" ), QLatin1String( SLOT( turnLeft_clicked() ) ) );
      b->addInputPort( QStringLiteral( "Turn Right" ), QLatin1String( SLOT( turnRight_clicked() ) ) );

      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

      return b;
    }

  private:
    QWidget* mainWindow = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    GeographicConvertionWrapper* tmw = nullptr;
};

