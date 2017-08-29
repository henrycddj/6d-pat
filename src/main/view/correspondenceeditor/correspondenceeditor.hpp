#ifndef CORRESPONDENCEEDITOR_H
#define CORRESPONDENCEEDITOR_H

#include "model/image.hpp"
#include "model/objectmodel.hpp"
#include "model/objectimagecorrespondence.hpp"
#include "model/modelmanager.hpp"
#include <QWidget>

namespace Ui {
class CorrespondenceEditor;
}

class CorrespondenceEditor : public QWidget
{
    Q_OBJECT

private:
    Ui::CorrespondenceEditor *ui;
    ModelManager* modelManager;

public:
    explicit CorrespondenceEditor(QWidget *parent = 0, ModelManager* modelManager = 0);
    ~CorrespondenceEditor();
    void setModelManager(ModelManager* modelManager);

public slots:
    void setImage(int index);
    void updateCorrespondence(ObjectImageCorrespondence* correspondence);

signals:
    void correspondenceChanged(ObjectImageCorrespondence* correspondence);
};

#endif // CORRESPONDENCEEDITOR_H
