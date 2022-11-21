#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LexEditor.h"

#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->colorsCombo, &QComboBox::currentIndexChanged, [this](int newIndex){
        auto themeName = "themes/" + ui->colorsCombo->currentData(Qt::DisplayRole).toString();
        theme = ScintillaQt::loadXmlStyle(themeName.toStdString());
        ScintillaQt::setStyle(ui->editor, theme);
        ui->editor->setFocus(true);
    });


    auto lineCount = ui->editor->lineCount();
    auto pixelWidth = 8 + ((log10(lineCount), 3)) * ui->editor->textWidth(STYLE_LINENUMBER, "8");
    ui->editor->setMarginWidthN(0, pixelWidth);
    findColorDefinitions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::findColorDefinitions()
{
    auto dir = QDir( "themes", "*.xml");
    auto colorFiles = dir.entryList(QDir::Files);
    ui->colorsCombo->clear();
    ui->colorsCombo->addItems(colorFiles);

    if (ui->colorsCombo->currentIndex() == -1)  {
        ui->colorsCombo->setCurrentIndex(0);
        ui->editor->setFocus(true);
    }
}

void MainWindow::findLanugages()
{
}
