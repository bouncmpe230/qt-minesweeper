#pragma once

#include <QDialog>

class TutorialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TutorialDialog(QWidget *parent = nullptr);

signals:
    void beginnerGameRequested();
};

