#pragma once

#include <QMainWindow>
#include "LexEditor.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void findColorDefinitions();
    void findLanugages();

    Ui::MainWindow *ui;
    Scintilla::NotepadPlusStyle theme;
    QStringList languages;
    QStringList colors;
};
