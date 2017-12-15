#ifndef NAVIGATIONCONTROLS_H
#define NAVIGATIONCONTROLS_H

#include <vector>
#include <QWidget>
#include <functional>

using namespace std;

namespace Ui {
class NavigationControls;
}

/*!
 * \brief The NavigationControls class provide navigational controls for the list views, as well as
 * the option to set the current operational folder for images and object models.
 */
class NavigationControls : public QWidget
{
    Q_OBJECT

private:
    QString currentPath = "";
    Ui::NavigationControls *ui;

public:
    explicit NavigationControls(QWidget *parent = 0);
    ~NavigationControls();
    void setPathToOpen(const QString &pathpath);

public slots:
    void folderButtonClicked();

signals:
    void navigateLeft();
    void navigateRight();
    void pathChanged(const QString &newPath);
};

#endif // NAVIGATIONCONTROLS_H