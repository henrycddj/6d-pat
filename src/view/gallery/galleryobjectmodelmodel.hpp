#ifndef GALLERYOBJECTMODELMODEL_H
#define GALLERYOBJECTMODELMODEL_H

#include "model/modelmanager.hpp"
#include "rendering/offscreenengine.hpp"

#include <QAbstractListModel>
#include <QPixmap>
#include <QMap>
#include <QVector>
#include <QRgb>
#include <QThread>
#include <QScopedPointer>
#include <QSize>

/*!
 * \brief The GalleryObjectModelModel class provides object model images to the Gallery.
 * It renders the ObjectModels offline and returns the images of them.
 */
class GalleryObjectModelModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QSize previewRenderingSize READ previewRenderingSize WRITE setPreviewRenderingSize)

public:

    explicit GalleryObjectModelModel(ModelManager* modelManager);
    ~GalleryObjectModelModel();

    //! Implementations of QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex&) const;
    void setSegmentationCodesForObjectModels(const QMap<QString, QString> &codes);
    void setPreviewRenderingSize(const QSize &size);
    QSize previewRenderingSize();

public Q_SLOTS:

    /*!
     * \brief onSelectedImageChanged sets the index of the currently selected image on this
     * model. When the index changes the object models will be reloaded, and if possible,
     * the segmentation images used to load only the respective models.
     * \param index
     */
    void onSelectedImageChanged(int index);

Q_SIGNALS:

    //!
    //! \brief displayedObjectModelsChanged this signal is Q_EMITted, whenever the object models
    //! to display change, e.g. because the user clicked a different image.
    //!
    void displayedObjectModelsChanged();

private:

    ModelManager* modelManager;
    QVector<ObjectModelPtr> objectModelsCache;
    QMap<QString, QImage> renderedObjectsModels;
    OffscreenEngine offscreenEngine{QSize(300, 300)};
    void renderObjectModels();
    QVector<ImagePtr> imagesCache;
    QMap<QString, QString> codes;
    //! We need this in case that an object model will not be displayed due to its color
    //! which then "tears" a hole into the indices
    QMap<int, int> indexMapping;
    void createIndexMapping();
    QVector<QColor> colorsOfCurrentImage;
    int currentSelectedImageIndex = -1;
    //! Store the index of the currently rendered image to be able to set the correct image
    //! when the renderer returns
    int currentlyRenderedImageIndex = 0;
    bool renderingObjectModels = false;
    QVariant dataForObjectModel(const ObjectModel& objectModel, int role) const;

private Q_SLOTS:

    bool isNumberOfToolsCorrect() const;
    void onObjectModelsChanged();
    void onImagesChanged();
    void onObjectModelRendered(const QImage &image);

};

#endif // GALLERYOBJECTMODELMODEL_H
