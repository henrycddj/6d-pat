#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QCloseEvent>
#include <QLayout>

const char* MainWindow::SETTINGS_NAME = "FlorettiKonfetti Inc.";
const char* MainWindow::SETTINGS_PROGRAM_NAME = "Otiat";
const char* MainWindow::SETTINGS_GROUP_NAME = "mainwindow";
const char* MainWindow::WINDOW_SIZE_KEY = "windowsize";
const char* MainWindow::WINDOW_POSITION_KEY = "windowposition";
const char* MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY = "splitterMainLeftSize";
const char* MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY = "splitterMainRightSize";
const char* MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY = "splitterLeftLeftSize";
const char* MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY = "splitterLeftRightSize";
const char* MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY = "splitterRightLeftSize";
const char* MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY = "splitterRightRightSize";

//! The main window of the application that holds the individual components.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();
    statusBar()->showMessage(QString("Loading..."));
    connect(ui->widgetGalleryLeft, &Gallery::selectedItemChanged, ui->widgetLeftBottom, &CorrespondenceEditor::setImage);
}

//! This function persistently stores settings of the application.
void MainWindow::writeSettings() {
    QSettings settings(MainWindow::SETTINGS_NAME, MainWindow::SETTINGS_PROGRAM_NAME);

    settings.beginGroup(MainWindow::SETTINGS_GROUP_NAME);
    settings.setValue(MainWindow::WINDOW_SIZE_KEY, size());
    settings.setValue(MainWindow::WINDOW_POSITION_KEY, pos());

    //! We store the aspect ratios of the splitters for convenience of the user
    QList<int> splitterMainSizes = ui->splitterMain->sizes();
    settings.setValue(MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY, splitterMainSizes.at(0));
    settings.setValue(MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY, splitterMainSizes.at(1));
    QList<int> splitterLefSizes = ui->splitterLeft->sizes();
    settings.setValue(MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY, splitterLefSizes.at(0));
    settings.setValue(MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY, splitterLefSizes.at(1));
    QList<int> splitterRightSizes = ui->splitterRight->sizes();
    settings.setValue(MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY, splitterRightSizes.at(0));
    settings.setValue(MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY, splitterRightSizes.at(1));
    settings.endGroup();
}

//! This function reads the persisted settings and restores the program's state.
void MainWindow::readSettings() {
    QSettings settings(MainWindow::SETTINGS_NAME, MainWindow::SETTINGS_PROGRAM_NAME);

    settings.beginGroup(MainWindow::SETTINGS_GROUP_NAME);
    resize(settings.value(MainWindow::WINDOW_SIZE_KEY, QSize(600, 400)).toSize());
    move(settings.value(MainWindow::WINDOW_POSITION_KEY, QPoint(200, 200)).toPoint());
    QList<int> splitterMainSizes;
    splitterMainSizes.append(settings.value(MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY, 300).toInt());
    splitterMainSizes.append(settings.value(MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY, 300).toInt());
    ui->splitterMain->setSizes(splitterMainSizes);
    QList<int> splitterLeftSizes;
    splitterLeftSizes.append(settings.value(MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY, 200).toInt());
    splitterLeftSizes.append(settings.value(MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY, 400).toInt());
    ui->splitterLeft->setSizes(splitterLeftSizes);
    QList<int> splitterRightSizes;
    splitterRightSizes.append(settings.value(MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY, 200).toInt());
    splitterRightSizes.append(settings.value(MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY, 400).toInt());
    ui->splitterRight->setSizes(splitterRightSizes);
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showStatusMessage(string message) {
    this->statusBar()->showMessage(QString(message.c_str()));
}

void MainWindow::setPathOnLeftBreadcrumbView(boost::filesystem::path pathToShow) {
    ui->widgetBreadcrumbLeft->setPathToShow(pathToShow);
}

void MainWindow::setPathOnRightBreadcrumbView(boost::filesystem::path pathToShow) {
    ui->widgetBreadcrumbRight->setPathToShow(pathToShow);
}

void MainWindow::setPathOnLeftNavigationControls(QString path) {
    ui->widgetNavigationLeft->setPathToOpen(path.toStdString());
}

void MainWindow::setPathOnRightNavigationControls(QString path) {
    ui->widgetNavigationRight->setPathToOpen(path.toStdString());
}

void MainWindow::addListenerToLeftNavigationControls(NavigationControlsListener listener) {
    ui->widgetNavigationLeft->addListener(listener);
}

void MainWindow::addListenerToRightNavigationControls(NavigationControlsListener listener) {
    ui->widgetNavigationRight->addListener(listener);
}

void MainWindow::setGalleryImageModel(GalleryImageModel* model) {
    this->ui->widgetGalleryLeft->setModel(model);
}

void MainWindow::setGalleryObjectModelModel(GalleryObjectModelModel* model) {
    this->ui->widgetGalleryRight->setModel(model);
    //! We connect the model that displays the 3D model objects to the selected image so that the model
    //! can display the respective models
    connect(ui->widgetGalleryLeft, &Gallery::selectedItemChanged, model, &GalleryObjectModelModel::onSelectedImageChanged);
    //! To update the listview of the 3D models we call the reset method when the model receives the signal
    //! that the selection of images on the left has been changed
    connect(model, &GalleryObjectModelModel::displayedObjectModelsChanged, ui->widgetGalleryRight, &Gallery::reset);
}
