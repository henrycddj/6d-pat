#ifndef OBJECTRENDERABLE_H
#define OBJECTRENDERABLE_H

#include "misc/global.hpp"
#include "model/objectmodel.hpp"

#include <QObject>
#include <QVector3D>
#include <QVector>
#include <QColor>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QSceneLoader>

class ObjectRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT

    Q_PROPERTY(Qt3DRender::QSceneLoader::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)

Q_SIGNALS:
    void statusChanged(Qt3DRender::QSceneLoader::Status status);
    void selectedChanged(bool selected);
    void clicksChanged();

public:
    ObjectRenderable(Qt3DCore::QEntity *parent);
    ObjectRenderable(Qt3DCore::QEntity *parent, const ObjectModel *m_objectModel);
    Qt3DRender::QSceneLoader::Status status() const;
    bool isSelected() const;

public Q_SLOTS:
    void setObjectModel(const ObjectModel *m_objectModel);
    void addClick(QVector3D click, QColor color);
    void setSelected(bool selected);
    void removeClicks();

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    bool m_selected;
    QVector<QVector3D> m_clicks{QVector3D(0, 0, 0)};
    QVector<QVector3D> m_clickColors{QVector3D(0, 0, 0)};


    Qt3DRender::QSceneLoader *m_sceneLoader;

    void initialize();
};

#endif // OBJECTRENDERABLE_H