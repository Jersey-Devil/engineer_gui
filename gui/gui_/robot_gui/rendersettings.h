#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QDialog>

namespace Ui {
class RenderSettings;
}

class RenderSettings : public QDialog
{
    Q_OBJECT

public:
    explicit RenderSettings(QWidget *parent = 0);
    ~RenderSettings();

private:
    Ui::RenderSettings *ui;
};

#endif // RENDERSETTINGS_H
