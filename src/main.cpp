#include <QMainWindow>
#include <QApplication>
#include <ScintillaEdit.h>
#include "LexEditor.h"

int main(int argc, char*argv[]) {
    QApplication app(argc, argv);
    QMainWindow win;

    auto editor = new ScintillaEdit(&win);
    auto lineCount = editor->lineCount();
    auto pixelWidth = 8 + ((log10(lineCount), 3)) * editor->textWidth(STYLE_LINENUMBER, "8");
    editor->setMarginWidthN(0, pixelWidth);


    auto theme = ScintillaQt::loadXmlStyle("themes/Monokai.xml");
//    auto theme = ScintillaQt::loadXmlStyle("themes/Hello Kitty.xml");
    ScintillaQt::setStyle(editor, theme);

    win.setCentralWidget(editor);
    win.show();

    return app.exec();
}
