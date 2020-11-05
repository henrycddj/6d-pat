#include "poseeditor.hpp"
#include "ui_poseeditor.h"
#include "misc/generalhelper.hpp"
#include "view/misc/displayhelper.hpp"

#include <opencv2/core/mat.hpp>
#include <QtGlobal>
#include <QUrl>
#include <QThread>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QItemSelectionRange>
#include <QDoubleSpinBox>
#include <QStringList>

PoseEditor::PoseEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoseEditor),
    poseEditor3DWindow(new PoseEditor3DWindow) {
    ui->setupUi(this);

    connect(poseEditor3DWindow, &PoseEditor3DWindow::positionClicked,
            this, &PoseEditor::onObjectModelClickedAt);
    QWidget *poseEditor3DWindowContainer = QWidget::createWindowContainer(poseEditor3DWindow);
    ui->graphicsContainer->layout()->addWidget(poseEditor3DWindowContainer);

    listViewPosesModel = new QStringListModel();
    ui->listViewPoses->setModel(listViewPosesModel);
    connect(ui->listViewPoses->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PoseEditor::onSelectedPoseChanged);
    listViewImagesModel = new QStringListModel;
    ui->listViewImages->setModel(listViewImagesModel);
    // To prevent clicking again in the object models list view because clicking too
    // quickly crashes the program
    connect(poseEditor3DWindow, &PoseEditor3DWindow::loadingObjectModel,
            this, &PoseEditor::loadingObjectModel);
    connect(poseEditor3DWindow, &PoseEditor3DWindow::objectModelLoaded,
            this, &PoseEditor::objectModelLoaded);
    connect(ui->listViewPoses->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &PoseEditor::onListViewPosesSelectionChanged);
    connect(poseEditor3DWindow, &PoseEditor3DWindow::objectModelLoaded,
            this, &PoseEditor::onObjectModelLoaded);
}

PoseEditor::~PoseEditor() {
    delete ui;
}

void PoseEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    if (this->modelManager) {
        disconnect(modelManager, &ModelManager::poseAdded,
                   this, &PoseEditor::onPoseAdded);
        disconnect(modelManager, &ModelManager::poseDeleted,
                   this, &PoseEditor::onPoseDeleted);
        disconnect(modelManager, &ModelManager::dataChanged,
                   this, &PoseEditor::onDataChanged);
    }
    this->modelManager = modelManager;
    connect(modelManager, &ModelManager::poseAdded,
            this, &PoseEditor::onPoseAdded);
    connect(modelManager, &ModelManager::poseDeleted,
            this, &PoseEditor::onPoseDeleted);
    connect(modelManager, &ModelManager::dataChanged,
               this, &PoseEditor::onDataChanged);
}

void PoseEditor::setPoseRecoverer(PoseRecoverer *poseRecoverer) {
    Q_ASSERT(poseRecoverer);
    if (this->poseRecoverer) {
        disconnect(poseRecoverer, &PoseRecoverer::correspondencesChanged,
                   this, &PoseEditor::onCorrespondencesChanged);
        disconnect(poseRecoverer, &PoseRecoverer::poseRecovered,
                   this, &PoseEditor::onCorrespondencesChanged);
        disconnect(poseRecoverer, &PoseRecoverer::stateChanged,
                   this, &PoseEditor::onPoseRecovererStateChanged);
    }
    this->poseRecoverer = poseRecoverer;
    connect(poseRecoverer, &PoseRecoverer::correspondencesChanged,
            this, &PoseEditor::onCorrespondencesChanged);
    connect(poseRecoverer, &PoseRecoverer::poseRecovered,
            this, &PoseEditor::onCorrespondencesChanged);
    connect(poseRecoverer, &PoseRecoverer::stateChanged,
            this, &PoseEditor::onPoseRecovererStateChanged);
}

void PoseEditor::setEnabledPoseEditorControls(bool enabled) {
    // TODO could add elements to a list and loop over the list
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    // The next line is the difference to setEnabledAllControls
    ui->buttonRemove->setEnabled(enabled);
    ui->buttonSave->setEnabled(enabled);
    ui->buttonDuplicate->setEnabled(enabled);
}

void PoseEditor::setEnabledAllControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->buttonRemove->setEnabled(enabled);
    ui->buttonCreate->setEnabled(enabled);
    ui->listViewPoses->setEnabled(enabled);
    ui->buttonSave->setEnabled(enabled);
    ui->buttonDuplicate->setEnabled(enabled);
    ui->buttonCopy->setEnabled(enabled);
    ui->listViewImages->setEnabled(enabled);
}

void PoseEditor::resetControlsValues() {
    ignoreValueChanges = true;
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    ignoreValueChanges = false;
}

void PoseEditor::setEnabledPoseInvariantControls(bool enabled) {
    ui->listViewImages->setEnabled(enabled);
    ui->listViewPoses->setEnabled(enabled);
    ui->buttonCopy->setEnabled(enabled);
}

void PoseEditor::addPosesToComboBoxPoses(const Image &image,
                                         const QString &poseToSelect) {
    QVector<PosePtr> poses =
            modelManager->getPosesForImage(image);
    posesIndices.clear();
    ignoreValueChanges = true;
    QStringList list("None");
    int index = 1;
    int _index = 0;
    bool poseSelected = false;
    for (PosePtr &pose : poses) {
        QString id = pose->id();
        list << id;
        if (id == poseToSelect) {
            _index = index;
            poseSelected = true;
        }
        posesIndices[pose->id()] = index;
        index++;
    }
    listViewPosesModel->setStringList(list);
    if (poses.size() > 0) {
        ui->listViewPoses->setEnabled(true);
    }
    if (!poseSelected) {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(0, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    } else {
        QModelIndex indexToSelect = ui->listViewPoses->model()->index(_index, 0);
        ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    }
    ignoreValueChanges = false;
}

void PoseEditor::setPoseValuesOnControls(const Pose &pose) {
    // TODO connect the signals that are to be added to the Pose class directly to the UI elements
    QVector3D position = pose.position();
    ignoreValueChanges = true;
    ui->spinBoxTranslationX->setValue(position.x());
    ui->spinBoxTranslationY->setValue(position.y());
    ui->spinBoxTranslationZ->setValue(position.z());
    QVector3D rotationVector = pose.rotation().toEulerAngles();
    ui->spinBoxRotationX->setValue(rotationVector[0]);
    ui->spinBoxRotationY->setValue(rotationVector[1]);
    ui->spinBoxRotationZ->setValue(rotationVector[2]);
    ignoreValueChanges = false;
}

void PoseEditor::onSpinBoxValueChanged() {
    if (!ignoreValueChanges) {
        updateCurrentlyEditedPose();
        ui->buttonSave->setEnabled(true);
        poseDirty = true;
    }
}

void PoseEditor::onPoseRecovererStateChanged(PoseRecoverer::State state) {
    poseEditor3DWindow->setClicks(poseRecoverer->points3D());
    ui->buttonCreate->setEnabled(state == PoseRecoverer::ReadyForPoseCreation);
}

void PoseEditor::onObjectModelLoaded() {
    // Reacts to when the 3D widget has loaded the object model
    // In the time between we want to prevent clicks to prevent crashes
    if (!currentlySelectedPose.isNull()) {
        setEnabledPoseEditorControls(true);
        setEnabledPoseInvariantControls(true);
        // Button save gets enabled by setting setEnabledPoseEditorControls
        ui->buttonSave->setEnabled(false);
    } else if (!currentlySelectedImage.isNull()) {
        setEnabledPoseInvariantControls(true);
    }
}

void PoseEditor::onListViewPosesSelectionChanged() {
    // Reacts after a different pose from the poses list view has been selected
    setEnabledAllControls(false);
    if (ui->listViewPoses->selectionModel()->selectedRows().at(0).row() == 0) {
        setEnabledPoseInvariantControls(true);
    }
}

void PoseEditor::checkPoseDirty() {
    if (poseDirty) {
        int result = QMessageBox::warning(this,
                             "Pose modifications unsaved",
                             "You have unsaved modifications of the currently edited pose."
                             " The action you just performed would discard these modifications. "
                             "Save them now?",
                             QMessageBox::Yes,
                             QMessageBox::No);
        if (result == QMessageBox::Yes) {
            onButtonSaveClicked();
        }
    }
    poseDirty = false;
}

void PoseEditor::onObjectModelClickedAt(const QVector3D &position) {
    qDebug() << "Object model (" + currentlySelectedObjectModel->getPath() + ") clicked at: (" +
                QString::number(position.x())
                + ", "
                + QString::number(position.y())
                + ", "
                + QString::number(position.z())+ ").";
    poseRecoverer->add3DPoint(position);
}

void PoseEditor::updateCurrentlyEditedPose() {
    if (currentlySelectedPose) {
        currentlySelectedPose->setPosition(QVector3D(ui->spinBoxTranslationX->value(),
                                           ui->spinBoxTranslationY->value(),
                                           ui->spinBoxTranslationZ->value()));
        QVector3D rotation(ui->spinBoxRotationX->value(),
                           ui->spinBoxRotationY->value(),
                           ui->spinBoxRotationZ->value());
        currentlySelectedPose->setRotation(QQuaternion::fromEulerAngles(rotation));
    }
}

void PoseEditor::onPoseAdded(PosePtr pose) {
    checkPoseDirty();
    addPosesToComboBoxPoses(*currentlySelectedImage, pose->id());
    ui->buttonCreate->setEnabled(false);
    // Gets enabled somehow
    ui->buttonSave->setEnabled(false);
    ui->buttonDuplicate->setEnabled(true);
    ui->buttonRemove->setEnabled(true);
    poseEditor3DWindow->setClicks({});
}

void PoseEditor::onPoseDeleted(PosePtr /*pose*/) {
    // Just select the default entry
    ui->listViewPoses->selectionModel()->select(ui->listViewPoses->model()->index(0, 0),
                                                QItemSelectionModel::ClearAndSelect);
    onComboBoxPoseIndexChanged(0);
}

void PoseEditor::onButtonRemoveClicked() {
    modelManager->removePose(currentlySelectedPose->id());
    QVector<PosePtr> poses = modelManager->getPosesForImage(*currentlySelectedImage);
    if (poses.size() > 0) {
        // This reloads the drop down list and does everything else
        addPosesToComboBoxPoses(*currentlySelectedImage);
    } else {
        reset();
    }
}

void PoseEditor::onButtonCopyClicked() {
    if (currentlySelectedImage.isNull()) {
        QMessageBox::warning(this,
                             "Pose copying",
                             "Oops something went wrong. No image selected. Please select an image from the gallery first.",
                             QMessageBox::Ok);
        return;
    }
    if (!ui->listViewImages->selectionModel()->hasSelection()) {
        QMessageBox::warning(this,
                             "Pose copying",
                             "No image to copy poses from selected. Please select one from the list.",
                             QMessageBox::Ok);
        return;
    }
    checkPoseDirty();
    QModelIndexList selection = ui->listViewImages->selectionModel()->selectedRows();
    int selectedImage = selection[0].row();
    ImagePtr image = modelManager->getImages()[selectedImage];
    QVector<PosePtr> posesForImage = modelManager->getPosesForImage(*image);
    for (PosePtr pose: posesForImage) {
        modelManager->addPose(*currentlySelectedImage,
                              *pose->objectModel(),
                              pose->position(),
                              pose->rotation().toRotationMatrix());
    }
    QMessageBox::information(this,
                             "Pose copying",
                             QString::number(posesForImage.size()) + " poses copied.",
                             QMessageBox::Ok);
}

void PoseEditor::onButtonCreateClicked() {
    checkPoseDirty();
    QString message("");
    switch (poseRecoverer->state()) {
    case PoseRecoverer::ReadyForPoseCreation: {
        bool success = poseRecoverer->recoverPose();
        if (success) {
            message = "Successfully recovered pose.";
        } else {
            message = "Unkown error while trying to recover pose.";
        }
        break;
    }
    case PoseRecoverer::NotEnoughCorrespondences: {
        message = "Not enough 2D - 3D correspondences.";
        break;
    }
    case PoseRecoverer::Missing2DPoint: {
        message = "Missing matching 2D counter part to selected 3D point. Click somewhere on the image.";
        break;
    }
    case PoseRecoverer::Missing3DPoint: {
        message = "Missing matching 3D counter part to selected 2D point. Click somewhere on the 3D model.";
        break;
    }
    case PoseRecoverer::Empty: {
        message = "No correspondences selected, yet. Please create some by click the 2D image and 3D model.";
        break;
    }
    default:
        break;
    }
    QMessageBox::warning(this,
                         "Pose recovering",
                         message,
                         QMessageBox::Ok);
}

void PoseEditor::onButtonSaveClicked() {
    // Any errors occuring in the model manager while saving will be handled
    // by the maincontroller
    modelManager->updatePose(currentlySelectedPose->id(),
                             currentlySelectedPose->position(),
                             currentlySelectedPose->rotation().toRotationMatrix());
    ui->buttonSave->setEnabled(false);
    poseDirty = false;
}

void PoseEditor::onButtonDuplicateClicked() {
    // Any errors occuring in the model manager while saving will be handled
    // by the maincontroller
    checkPoseDirty();
    modelManager->addPose(*currentlySelectedImage,
                          *currentlySelectedPose->objectModel(),
                          currentlySelectedPose->position(),
                          currentlySelectedPose->rotation().toRotationMatrix());
}

void PoseEditor::onComboBoxPoseIndexChanged(int index) {
    // Left over of the original combobox but now functions
    // to set the poses index manually
    if (index < 0 || ignoreValueChanges)
        return;
    else if (index == 0) {
        // First index is placeholder
        setEnabledPoseEditorControls(false);
        currentlySelectedPose.reset();
        resetControlsValues();
    } else {
        QVector<PosePtr> posesForImage =
                modelManager->getPosesForImage(*currentlySelectedImage);
        PosePtr pose = posesForImage.at(--index);
        setPoseToEdit(pose);
    }
}

void PoseEditor::onDataChanged(int data) {
    // Reacts to the model manager's signal
    if (data == Data::Poses) {
        reset();
        addPosesToComboBoxPoses(*currentlySelectedImage);
    } else {
        reset();
    }
}

void PoseEditor::onSelectedPoseChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/) {
    // Reacts to selecting a different pose from the poses list view and loads the corresponding
    // pose
    QItemSelectionRange range = selected.front();
    PosePtr pose;
    int index = range.top();
    if (index < 0 || ignoreValueChanges)
        return;
    else if (index == 0) {
        // First index is placeholder
        poseEditor3DWindow->reset();
        setEnabledPoseEditorControls(false);
        currentlySelectedPose.reset();
        resetControlsValues();
        // Gets enabled somehow
        ui->buttonSave->setEnabled(false);
        // Need to enable both lists here, normally we enable them when the pose
        // has successfully loaded, but we are loading none here
        setEnabledPoseInvariantControls(true);
    } else {
        QVector<PosePtr> posesForImage =
                modelManager->getPosesForImage(*currentlySelectedImage);
        pose = posesForImage.at(--index);
        setPoseToEdit(pose);
    }
    if (doNotEmitPoseSelected) {
        doNotEmitPoseSelected = false;
    } else {
        Q_EMIT poseSelected(pose);
    }
}

void PoseEditor::setObjectModel(ObjectModelPtr objectModel) {
    checkPoseDirty();
    if (objectModel == Q_NULLPTR) {
        qDebug() << "Object model to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    setEnabledAllControls(false);
    currentlySelectedObjectModel = objectModel;
    poseEditor3DWindow->setObjectModel(*objectModel);
}

void PoseEditor::onSelectedObjectModelChanged(int index) {
    QVector<ObjectModelPtr> objectModels = modelManager->getObjectModels();
    Q_ASSERT_X(index >= 0 && index < objectModels.size(), "onSelectedObjectModelChanged", "Index out of bounds.");
    checkPoseDirty();
    ObjectModelPtr objectModel = objectModels[index];
    setObjectModel(objectModel);
}

void PoseEditor::onSelectedImageChanged(int index) {
    QVector<ImagePtr> images = modelManager->getImages();
    Q_ASSERT_X(index >= 0 && index < images.size(), "onSelectedImageChanged", "Index out of bounds.");
    checkPoseDirty();
    reset();
    currentlySelectedImage = images[index];
    // Should always be the case when we can set an image through onSelectedImageChanged
    // but we use the opportunity and enable the button here because it is disabled
    // when we start the program
    ui->buttonCopy->setEnabled(images.size() > 0);
    QStringList imagesList;
    for (ImagePtr &image : images) {
        imagesList << image->getImagePath();
    }
    listViewImagesModel->setStringList(imagesList);
    addPosesToComboBoxPoses(*currentlySelectedImage);
    setEnabledPoseInvariantControls(true);
}

void PoseEditor::selectPose(PosePtr pose) {
    int index = 0;
    if (!pose.isNull()) {
        index = posesIndices[pose->id()];
    }
    // Store the last selected pose in case that the user selects it again directly
    // then we have to manually enable the controls again because a signal doesn't
    // get sent
    lastSelectedPose = currentlySelectedPose;
    QModelIndex indexToSelect = ui->listViewPoses->model()->index(index, 0);
    // To prevent the pose selected signal to be sent
    doNotEmitPoseSelected = true;
    // Select the corresponding list entry
    ui->listViewPoses->selectionModel()->select(indexToSelect, QItemSelectionModel::ClearAndSelect);
    if (lastSelectedPose.isNull()) {
        // If Pose is null we don't send this signal but need it for the object models gallery
        // to get enabled again, a bit hacky but so what
        Q_EMIT objectModelLoaded();
        setEnabledPoseEditorControls(true);
        setEnabledPoseInvariantControls(true);
        // Gets enabled by the editor controls
        ui->buttonSave->setEnabled(false);
    } else {
        // We do not need to retain the last selected pose if the new pose is not null
        // it only is an issue when the new pose is null, i.e. the user deselected the pose
        lastSelectedPose.reset();
    }
}

void PoseEditor::setPoseToEdit(PosePtr pose) {
    checkPoseDirty();

    if (pose.isNull()) {
        qDebug() << "Pose to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting pose (" + pose->id() + ", " + pose->image()->getImagePath()
                + ", " + pose->objectModel()->getPath() + ") to display.";
    currentlySelectedPose = pose;
    currentlySelectedObjectModel = pose->objectModel();
    // Handled by the slot conneced in the init function of the PoseEditor
    // to the objectModelLoaded signal of the 3D widget
    //setEnabledPoseEditorControls(true);
    setPoseValuesOnControls(*pose);
    poseEditor3DWindow->setObjectModel(*pose->objectModel());
    ui->buttonSave->setEnabled(false);
}

void PoseEditor::onPoseCreationAborted() {
    ui->buttonCreate->setEnabled(false);
}

void PoseEditor::onCorrespondencesChanged() {
    ui->buttonCreate->setEnabled(poseRecoverer->state() == PoseRecoverer::ReadyForPoseCreation);
    poseEditor3DWindow->setClicks(poseRecoverer->points3D());
}

void PoseEditor::reset() {
    qDebug() << "Resetting pose editor.";
    poseEditor3DWindow->reset();
    currentlySelectedObjectModel.reset();
    currentlySelectedPose.reset();
    listViewPosesModel->setStringList({});
    listViewImagesModel->setStringList({});
    resetControlsValues();
    setEnabledAllControls(false);
}

bool PoseEditor::isDisplayingObjectModel() {
    return !currentlySelectedObjectModel.isNull();
}
