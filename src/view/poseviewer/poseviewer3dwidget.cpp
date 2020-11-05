#include "poseviewer3dwidget.hpp"
#include "misc/global.hpp"

#include <QFrame>
#include <QImage>
#include <QMouseEvent>

#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QPickingSettings>

PoseViewer3DWidget::PoseViewer3DWidget(QWidget *parent)
    : Qt3DWidget(parent),
      root(new Qt3DCore::QEntity),
      renderSurfaceSelector(new Qt3DRender::QRenderSurfaceSelector),

      viewport(new Qt3DRender::QViewport),
      clearBuffers(new Qt3DRender::QClearBuffers),
      noDraw(new Qt3DRender::QNoDraw),
      backgroundLayerFilter(new Qt3DRender::QLayerFilter),
      backgroundLayer(new Qt3DRender::QLayer),
      backgroundCamera(new Qt3DRender::QCamera),
      backgroundCameraSelector(new Qt3DRender::QCameraSelector),
      backgroundNoDepthMask(new Qt3DRender::QNoDepthMask),
      posesLayerFilter(new Qt3DRender::QLayerFilter),
      posesLayer(new Qt3DRender::QLayer),
      posesCamera(new Qt3DRender::QCamera),
      posesCameraSelector(new Qt3DRender::QCameraSelector),
      posesDepthTest(new Qt3DRender::QDepthTest),
      clickVisualizationLayerFilter(new Qt3DRender::QLayerFilter),
      clickVisualizationLayer(new Qt3DRender::QLayer),
      clickVisualizationCameraSelector(new Qt3DRender::QCameraSelector),
      clickVisualizationCamera(new Qt3DRender::QCamera),
      clickVisualizationNoDepthMask(new Qt3DRender::QNoDepthMask),
      clickVisualizationRenderable(new ClickVisualizationRenderable) {
}

PoseViewer3DWidget::~PoseViewer3DWidget() {
}

void PoseViewer3DWidget::initializeQt3D() {
    setRootEntity(root);

    // First branch that clears the buffers
    clearBuffers->setParent(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor(Qt::white);
    noDraw->setParent(clearBuffers);

    // Second branch that draws the background image
    backgroundLayerFilter->setParent(viewport);
    backgroundLayerFilter->addLayer(backgroundLayer);
    backgroundCameraSelector->setParent(backgroundLayerFilter);
    backgroundCamera->setParent(backgroundCameraSelector);
    backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    backgroundCamera->setPosition(QVector3D(0, 0, 1));
    backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    backgroundCameraSelector->setCamera(backgroundCamera);
    backgroundNoDepthMask->setParent(backgroundCameraSelector);

    // We need to clear the depth buffer so that we can draw the click overlay
    clearBuffers2 = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    noDraw2 = new Qt3DRender::QNoDraw(clearBuffers2);

    // Third branch that draws the poses
    // We don't need a layer filter here because only the poses travel through this branch somehow
    posesLayerFilter->setParent(viewport);
    posesLayerFilter->addLayer(backgroundLayer);
    posesLayerFilter->addLayer(clickVisualizationLayer);
    posesLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::DiscardAnyMatchingLayers);
    posesCameraSelector->setParent(posesLayerFilter);
    posesCameraSelector->setCamera(posesCamera);
    posesCamera->setPosition({0, 0, 0});
    posesCamera->setViewCenter({0, 0, 1});
    posesCamera->setUpVector({0, -1, 0});
    posesDepthTest->setParent(posesCameraSelector);
    posesDepthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);

    setActiveFrameGraph(viewport);

    // Fourth branch draws the clicks
    clickVisualizationLayerFilter->setParent(viewport);
    clickVisualizationLayerFilter->addLayer(clickVisualizationLayer);
    clickVisualizationCameraSelector->setParent(clickVisualizationLayerFilter);
    clickVisualizationCamera->setParent(clickVisualizationCameraSelector);
    clickVisualizationCamera->lens()->setOrthographicProjection(0, this->size().width(),
                                                                0, this->size().height(),
                                                                0.1f, 1000.f);
    clickVisualizationCamera->setPosition(QVector3D(0, 0, 1));
    clickVisualizationCamera->setViewCenter(QVector3D(0, 0, 0));
    clickVisualizationCamera->setUpVector(QVector3D(0, 1, 0));
    clickVisualizationCameraSelector->setCamera(clickVisualizationCamera);
    clickVisualizationNoDepthMask->setParent(clickVisualizationCameraSelector);
    clickVisualizationRenderable->setParent(root);
    clickVisualizationRenderable->addComponent(clickVisualizationLayer);
    clickVisualizationRenderable->setSize(this->size());

    // No need to set a QRenderSurfaceSelector because this is already in the Qt3DWidget
    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
}

void PoseViewer3DWidget::setBackgroundImageAndPoses(const QString &image,
                                                    const QMatrix3x3 &cameraMatrix,
                                                    const QVector<PosePtr> &poses) {
    setBackgroundImage(image, cameraMatrix);
    for (const PosePtr &pose : poses) {
        addPose(*pose);
    }
}

void PoseViewer3DWidget::setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix) {
    QImage loadedImage(image);
    this->m_imageSize = loadedImage.size();
    this->resize(loadedImage.size());
    if (backgroundImageRenderable == Q_NULLPTR) {
        backgroundImageRenderable = new BackgroundImageRenderable(root, image);
        backgroundImageRenderable->addComponent(backgroundLayer);
    } else {
        backgroundImageRenderable->setImage(image);
    }

    float w = loadedImage.width();
    float h = loadedImage.height();
    float depth = (float) farPlane - nearPlane;
    float q = -(farPlane + nearPlane) / depth;
    float qn = -2 * (farPlane * nearPlane) / depth;
    const QMatrix3x3 K = cameraMatrix;
    projectionMatrix = QMatrix4x4(2 * K(0, 0) / w, -2 * K(0, 1) / w, (-2 * K(0, 2) + w) / w, 0,
                                                0,  2 * K(1, 1) / h,  (2 * K(1 ,2) - h) / h, 0,
                                                0,                0,                      q, qn,
                                                0,                0,                     -1, 0);
    posesCamera->setProjectionMatrix(projectionMatrix);
    backgroundImageRenderable->setEnabled(true);
    move(-500, -500);
}

void PoseViewer3DWidget::addPose(const Pose &pose) {
    PoseRenderable *poseRenderable = new PoseRenderable(root, pose);
    poseRenderables.append(poseRenderable);
    poseRenderableForId[pose.id()] = poseRenderable;
    connect(poseRenderable, &PoseRenderable::moved,
            [this, poseRenderable](Qt3DRender::QPickEvent *e){
        if (qFuzzyCompare((double) oldDepth, (double) -1.f)) {
            oldDepth = e->distance();
        } else {
            if (e->distance() > 1) {
                QVector3D click(clickPos.x(), height() - clickPos.y(), e->distance());
                qDebug() << click;
                QVector3D unprojected = click.unproject(posesCamera->viewMatrix() * poseRenderable->getTransform()->matrix(),
                                                        projectionMatrix,
                                                        QRect(0, 0, imageSize().width(), imageSize().height()));
                qDebug() << unprojected;
                qDebug() << e->worldIntersection();
                QVector3D difference = unprojected - e->worldIntersection();
                qDebug() << "diff" << difference;
                poseRenderable->setPosition(poseRenderable->position() + difference);
                oldDepth = e->distance();
            }
        }
    });
}

void PoseViewer3DWidget::updatePose(const Pose &pose) {
    // ToDo connect signals of Pose when values change to update slots of PoseRenderable
    PoseRenderable *renderable = poseRenderableForId[pose.id()];
    renderable->setPosition(pose.position());
    renderable->setRotation(pose.rotation());
}

void PoseViewer3DWidget::removePose(const QString &id) {
    for (int index = 0; index < poseRenderables.size(); index++) {
        if (poseRenderables[index]->poseID() == id) {
            PoseRenderable *renderable = poseRenderables[index];
            // Remove related framegraph
            poseRenderables.removeAt(index);
            poseRenderableForId.remove(id);
            // This also deletes the renderable
            renderable->setParent((Qt3DCore::QNode *) 0);
            break;
        }
    }
}

void PoseViewer3DWidget::removePoses() {
    for (int index = 0; index < poseRenderables.size(); index++) {
        PoseRenderable *renderable = poseRenderables[index];
        // This also deletes the renderable
        renderable->setParent((Qt3DCore::QNode *) 0);
    }
    poseRenderables.clear();
    poseRenderableForId.clear();
}

void PoseViewer3DWidget::selectPose(PosePtr pose) {

}

void PoseViewer3DWidget::setObjectsOpacity(float opacity) {
    for (PoseRenderable *poseRenderable : poseRenderables) {
        poseRenderable->setOpacity(opacity);
    }
}

void PoseViewer3DWidget::setClicks(const QVector<QPoint> &clicks) {
    clickVisualizationRenderable->setClicks(clicks);
}

void PoseViewer3DWidget::reset() {
    setClicks({});
    removePoses();
    if (backgroundImageRenderable != Q_NULLPTR) {
        backgroundImageRenderable->setEnabled(false);
    }
}

void PoseViewer3DWidget::resizeEvent(QResizeEvent *event) {
    Qt3DWidget::resizeEvent(event);
    clickVisualizationRenderable->setSize(event->size());
    //clickVisualizationCamera->lens()->setOrthographicProjection(0, this->size().width(),
    //                                                            0, this->size().height(),
    //                                                            0.1f, 1000.f);
}

void PoseViewer3DWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
    clickPos = event->globalPos();
}

void PoseViewer3DWidget::mouseMoveEvent(QMouseEvent *event) {
    // TODO
    // Left mouse to select objects and rotate them, right
    // to move the widget
    if (event->buttons() & Qt::LeftButton) {
        clickPos = event->globalPos();
        newPos.setX(clickPos.x() - lastPos.x());
        newPos.setY(clickPos.y() - lastPos.y());
        //move(newPos);
        mouseMoved = true;
    }
    Qt3DWidget::mouseMoveEvent(event);
}

void PoseViewer3DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && !mouseMoved && backgroundImageRenderable != Q_NULLPTR) {
        Q_EMIT positionClicked(event->pos());
    }
    mouseMoved = false;
}

QSize PoseViewer3DWidget::imageSize() const
{
    return m_imageSize;
}
