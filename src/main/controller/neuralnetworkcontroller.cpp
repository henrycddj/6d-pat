#include <Python.h>

#include "neuralnetworkcontroller.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QThreadPool>

NeuralNetworkController::NeuralNetworkController(const QString &trainPythonScript,
                                                 const QString &inferencePythonScript) :
    trainPythonScript(trainPythonScript),
    inferencePythonScript(inferencePythonScript) {
}

NeuralNetworkController::~NeuralNetworkController() {
    QThreadPool::globalInstance()->clear();
    Py_Finalize();
}

void NeuralNetworkController::training(const QString &configPath) {
    if (networkRunnable) {
        QThreadPool::globalInstance()->waitForDone();
    }
    networkRunnable = new NeuralNetworkRunnable(trainPythonScript);
    setPathsOnConfig(configPath);
    networkRunnable->setConfigPath(configPath);
    connect(networkRunnable, &NeuralNetworkRunnable::processFinished,
            this, &NeuralNetworkController::trainingFinished);
    QThreadPool::globalInstance()->start(networkRunnable);
    Q_EMIT trainingStarted();
}

void NeuralNetworkController::inference(const QString &configPath) {
    if (networkRunnable) {
        QThreadPool::globalInstance()->waitForDone();
    }
    networkRunnable = new NeuralNetworkRunnable(inferencePythonScript);
    setPathsOnConfig(configPath);
    networkRunnable->setConfigPath(configPath);
    connect(networkRunnable, &NeuralNetworkRunnable::processFinished,
            this, &NeuralNetworkController::inferenceFinished);
    QThreadPool::globalInstance()->start(networkRunnable);
    Q_EMIT inferenceStarted();
}

void NeuralNetworkController::setImages(const QVector<Image> &images) {
    this->images = images;
}

void NeuralNetworkController::setCorrespondencesFilePath(const QString &filePath) {
    this->correspondencesFilePath = filePath;
}

void NeuralNetworkController::stop() {
    if (networkRunnable) {
        QThreadPool::globalInstance()->clear();
        Q_EMIT networkStopped();
    }
}

QString NeuralNetworkController::getTrainPythonScript() const
{
    return trainPythonScript;
}

void NeuralNetworkController::setTrainPythonScript(const QString &value)
{
    trainPythonScript = value;
}

QString NeuralNetworkController::getInferencePythonScript() const
{
    return inferencePythonScript;
}

void NeuralNetworkController::setInferencePythonScript(const QString &value)
{
    inferencePythonScript = value;
}

void NeuralNetworkController::setPathsOnConfig(const QString &configPath) {
    QFile configFile(configPath);
    if (configFile.open(QFile::ReadWrite)) {
        QByteArray data = configFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDocument.object();
        QString imageListFilePath = jsonObject["IMAGE_LIST"].toString();
        QFile imageListFile(imageListFilePath);
        if (imageListFile.open(QFile::ReadWrite)) {
            QJsonArray imageList;
            for (Image &image : images) {
                imageList << image.getImagePath();
            }
            imageListFile.resize(0);
            imageListFile.write(QJsonDocument(imageList).toJson());
        }
        jsonObject["OUTPUT_FILE"] = correspondencesFilePath;
        configFile.resize(0);
        configFile.write(QJsonDocument(jsonObject).toJson());
    }
}
