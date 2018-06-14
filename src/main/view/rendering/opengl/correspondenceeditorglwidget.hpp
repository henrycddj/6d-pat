#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/correspondence.hpp"
#include "view/rendering/opengl/correspondencerenderable.hpp"
#include "view/rendering/opengl/clickvisualizationoverlay.hpp"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>

typedef QSharedPointer<CorrespondenceRenderable> ObjectModelRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class CorrespondenceEditorGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit CorrespondenceEditorGLWidget(QWidget *parent = 0);
    void addCorrespondence(const Correspondence &correspondence);
    void updateCorrespondence(const Correspondence &correspondence);
    void removeCorrespondence(const Correspondence &correspondence);
    void removeCorrespondences();
    CorrespondenceRenderable *getObjectModelRenderable(
            const Correspondence &correspondence);
    void setObjectsOpacity(float opacity);
    void addClick(QPoint position, QColor color);
    void removeClicks();
    void reset();

    ~CorrespondenceEditorGLWidget();

signals:
    void positionClicked(QPoint position);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void addCorrespondence(const Correspondence &correspondence, bool update);

    void initializeBackgroundProgram();
    void initializeObjectProgram();

    // Background stuff

    QVector<ObjectModelRenderablePtr> objectModelRenderables;
    QOpenGLShaderProgramPtr objectsProgram;
    int projectionMatrixLoc;
    int normalMatrixLoc;
    int lightPosLoc;
    int opacityLoc;
    // Matrix created from the intrinsic camera parameters
    QMatrix4x4 projectionMatrix;
    float opacity = 1.f;

    int xRot;
    int yRot;
    int zRot;

    // To handle dragging of the widget and clicking
    QPoint lastPos;
    bool mouseDown = false;
    bool mouseMoved = false;
    ClickVisualizationOverlay *clickOverlay;

    float farPlane = 2000.f;
    float nearPlane = 100.f;
};

#endif
