#include <QStringView>
#include <QDomDocument>
#include <QFile>
#include <ScintillaEdit.h>
#include <QApplication>
#include <QString>
#include "LexEditor.h"


namespace {

inline std::string getNodeAttribute(QDomNode &n, std::string_view name)
{
    return n.attributes().namedItem("name").nodeValue().toStdString();
}

inline std::optional<uint> getNodeAttributeUInt(QDomNode &n, std::string_view name, int radix=10)
{
    if (!n.hasAttributes()) {
        return {};
    }
    auto nameItem = n.attributes().namedItem(QString::fromStdString(std::string(name)));
    if (nameItem.isNull()) {
        return {};
    }
    auto value = nameItem.nodeValue();
    auto ok = false;
    auto i = value.toUInt(&ok, radix);
    if (!ok) {
        return {};
    }
    return i;
}

}

namespace ScintillaQt {

sptr_t LEX_COLOR(uchar r, uchar g, uchar b) {
    return r | g << 8 | b << 16;
}

Scintilla::NotepadPlusStyle loadXmlStyle(const std::filesystem::path fileName)
{
    QFile file(fileName);
    bool b;

    if (!file.open(QIODevice::ReadOnly))
    {
        // TODO: display debug message, saying that this is an invalid XML?
        return {};
    }

    QDomDocument doc("NotepadPlus");
    if (!doc.setContent(&file)) {
        // TODO: display debug message, saying that this is an invalid XML?
        return {};
    }

    Scintilla::NotepadPlusStyle notepadStyle;
    {
        auto globalStyles = doc.elementsByTagName("GlobalStyles");
        auto styles = globalStyles.at(0).toElement().elementsByTagName("WidgetStyle");
        for (auto l=0; l<styles.count(); l++) {
            auto styleNode = styles.at(l);
            auto widgetStyle = Scintilla::WidgetStyle{};

            widgetStyle.styleID = getNodeAttributeUInt(styleNode, "styleID").value_or(-1);
            widgetStyle.name = getNodeAttribute(styleNode, "name");
            widgetStyle.bgColor = getNodeAttributeUInt(styleNode, "bgColor", 16);
            widgetStyle.fgColor = getNodeAttributeUInt(styleNode, "fgColor", 16);
            widgetStyle.fontSize = getNodeAttributeUInt(styleNode, "fontSize");
            widgetStyle.fontStyle = getNodeAttributeUInt(styleNode, "fontStyle");
            widgetStyle.fontName = getNodeAttribute(styleNode, "fontName");
            notepadStyle.globalStyles.insert({ widgetStyle.styleID, widgetStyle });
        }
    }
    {
        auto lexerStylesNodes = doc.elementsByTagName("LexerStyles");
        auto stylesNodes = lexerStylesNodes.at(0).toElement().elementsByTagName("LexerType");
        for (auto l=0; l<stylesNodes.count(); l++) {
            auto lexerTypeNode = stylesNodes.at(l);
            auto wordsStylesNodes = lexerTypeNode.toElement().elementsByTagName("WordStyle");
            auto lexerStyle = Scintilla::LexerStyle{};
            lexerStyle.name = getNodeAttribute(lexerTypeNode, "name");
            lexerStyle.desc = getNodeAttribute(lexerTypeNode, "desc");
            lexerStyle.ext = getNodeAttribute(lexerTypeNode, "ext");
            for (auto k=0; k<wordsStylesNodes.count(); k++) {
                auto wordStyleNode = wordsStylesNodes.at(k);
                auto wordsStyle = Scintilla::WordsStyle{};
                wordsStyle.name = getNodeAttribute(wordStyleNode, "name");
                wordsStyle.styleID = getNodeAttributeUInt(wordStyleNode, "styleID").value();
                wordsStyle.bgColor = getNodeAttributeUInt(wordStyleNode, "bgColor", 16);
                wordsStyle.fgColor = getNodeAttributeUInt(wordStyleNode, "fgColor", 16);
                wordsStyle.fontSize = getNodeAttributeUInt(wordStyleNode, "fontSize");
                wordsStyle.fontStyle = getNodeAttributeUInt(wordStyleNode, "fontStyle");
                wordsStyle.fontName = getNodeAttribute(wordStyleNode, "fontName");
                wordsStyle.keywordClass = getNodeAttribute(wordStyleNode, "keywordClass");
                lexerStyle.wordsStyles[wordsStyle.styleID] = wordsStyle;
            }
            notepadStyle.lexerStyles[lexerStyle.name] = lexerStyle;
        }
    }

    return notepadStyle;
}

const int MARK_BOOKMARK = 24;
const int MARK_HIDELINESBEGIN = 23;
const int MARK_HIDELINESEND = 22;
const int MARK_HIDELINESUNDERLINE = 21;


void setStyle(ScintillaEdit *editor, const Scintilla::NotepadPlusStyle &style)
{
//    editor->styleSetFore(STYLE_DEFAULT,  style.findWidgetStyle());
    editor->styleResetDefault();
    editor->styleClearAll();

#if 0
    editor->styleSetBack(STYLE_DEFAULT, LEX_COLOR(0x00,0x00,0x80));
    editor->styleSetSize(STYLE_DEFAULT, qApp->font().pointSize());
    editor->styleSetFont(STYLE_DEFAULT, "Courier New");


    editor->styleSetFore(STYLE_LINENUMBER, LEX_COLOR(0xFF,0xFF,0xFF));
    editor->styleSetBack(STYLE_LINENUMBER, LEX_COLOR(0x30,0x30,0xA0));
    editor->styleSetBold(STYLE_LINENUMBER, false);

    editor->styleSetFore(STYLE_BRACELIGHT, 0x0000FF);
    editor->styleSetBack(STYLE_BRACELIGHT, 0xFFFFFF);

    editor->styleSetFore(STYLE_BRACEBAD, 0x000080);
    editor->styleSetBack(STYLE_BRACEBAD, 0xFFFFFF);

    editor->styleSetFore(STYLE_INDENTGUIDE, 0xC0C0C0);
    editor->styleSetBack(STYLE_INDENTGUIDE, 0xFFFFFF);


    editor->setElementColour(SC_ELEMENT_CARET_LINE_BACK, LEX_COLOR(0x00,0xA0,0xA0));
//    editor->setElementColour(SC_ELEMENT_WHITE_SPACE, LEX_COLOR(0x00,0xff,0x00));
#else
    int color;

    std::optional<Scintilla::WidgetStyle> widgetStyle;

//#define STYLE_CONTROLCHAR 36
//#define STYLE_INDENTGUIDE 37
//#define STYLE_CALLTIP 38
//#define STYLE_FOLDDISPLAYTEXT 39
//#define STYLE_LASTPREDEFINED 39

    widgetStyle = style.findWidgetStyle("Default Style", 32);
    if (widgetStyle) {
        editor->styleSetFore(STYLE_DEFAULT, widgetStyle.value().fgColor.value());
        editor->styleSetBack(STYLE_DEFAULT, widgetStyle.value().bgColor.value());
        editor->styleClearAll();
    };
    widgetStyle = style.findWidgetStyle("Global override", 0);
    if (widgetStyle) {
        // TODO - is this correct at all?
        editor->styleSetBack(STYLE_DEFAULT, widgetStyle.value().bgColor.value());
        editor->styleSetFore(STYLE_DEFAULT, widgetStyle.value().fgColor.value());
        editor->styleClearAll();
    };
    widgetStyle = style.findWidgetStyle("Indent guideline ", 37);
    if (widgetStyle) {
//        editor->setCaretStyle()
    };
    widgetStyle = style.findWidgetStyle("Brace highlight ", 34);
    if (widgetStyle) {
        editor->styleSetFore(STYLE_BRACELIGHT, widgetStyle.value().fgColor.value());
        editor->styleSetBack(STYLE_BRACELIGHT, widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("Bad brace colour", 35);
    if (widgetStyle) {
        editor->styleSetFore(STYLE_BRACEBAD, widgetStyle.value().fgColor.value());
        editor->styleSetBack(STYLE_BRACEBAD, widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("Current line background colour", 0);
    if (widgetStyle) {
        editor->setElementColour(SC_ELEMENT_CARET_LINE_BACK, widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("Selected text colour", 0);
    if (widgetStyle) {
        editor->setSelBack(true, widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("Caret colour", 2069);
    if (widgetStyle) {
        editor->setCaretFore(widgetStyle.value().fgColor.value());
//      TODO: "Hello Kitty" defines bgColor... wtf is it?
//      if (widgetStyle.value().bgColor.has_value())
//          editor->setCaretBack(widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("Edge colour", 0);
    if (widgetStyle) {
        editor->setEdgeColour(widgetStyle.value().fgColor.value());
//      TODO: ???
//      editor->setEdgeColour(widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("Line number margin", 33);
    if (widgetStyle) {
        editor->styleSetFore(STYLE_LINENUMBER, widgetStyle.value().fgColor.value());
        editor->styleSetBack(STYLE_LINENUMBER, widgetStyle.value().bgColor.value());
        editor->styleSetBold(STYLE_LINENUMBER, widgetStyle.value().fontStyle.value_or(0) > 1);
    };
    widgetStyle = style.findWidgetStyle("Bookmark margin", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Change History margin", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Fold", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Fold active", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Fold margin", 0);
    if (widgetStyle) {
        editor->setFoldMarginColour(true, widgetStyle.value().fgColor.value());
        editor->setFoldMarginHiColour(true, widgetStyle.value().bgColor.value());
    };
    widgetStyle = style.findWidgetStyle("White space symbol", 0);
    if (widgetStyle) {
        // TODO - what is this 16?
        editor->setWhitespaceFore(widgetStyle.value().fgColor.value(), 16);

    };
    widgetStyle = style.findWidgetStyle("Smart HighLighting", 29);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Find Mark Style", 31);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Mark Style 1", 25);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Mark Style 2", 24);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Mark Style 3", 23);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Mark Style 4", 22);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Mark Style 5", 21);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Incremental highlight all", 28);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Tags match highlight", 27);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Tags attribute", 26);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Active tab focused indicator", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Active tab unfocused indicator", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Active tab text", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("Inactive tabs", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("URL hovered", 0);
    if (widgetStyle) {

    };
    widgetStyle = style.findWidgetStyle("EOL custom color", 0);
    if (widgetStyle) {

    };
#endif

}

}
