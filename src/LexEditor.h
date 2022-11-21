#pragma once

#include <filesystem>
#include <string>
#include <map>
#include <optional>
#include <QStringView>

class ScintillaEdit;

namespace Scintilla {

struct WidgetStyle {
    uint styleID;
    std::string name;
    std::optional<uint> bgColor;
    std::optional<uint> fgColor;
    std::optional<uint> fontSize;
    std::optional<uint> fontStyle;
    std::optional<std::string> fontName;
};

struct WordsStyle {
    int styleID;
    std::string name;
    std::optional<uint> bgColor;
    std::optional<uint> fgColor;
    std::optional<uint> fontSize;
    std::optional<uint> fontStyle;
    std::optional<std::string> fontName;
    std::optional<std::string> keywordClass;
};

struct LexerStyle
{
    std::string name;
    std::string desc;
    std::string ext;
    std::map<uint,WordsStyle> wordsStyles;
};

struct NotepadPlusStyle {
    std::multimap<int,WidgetStyle> globalStyles;
    std::map<std::string,LexerStyle> lexerStyles;

    // Ideally this would be a `std::optional<const WidgetStyle>`
    inline std::optional<WidgetStyle> findWidgetStyle(std::string_view name, uint styleID) const {
        for (auto i = globalStyles.find(styleID); i != globalStyles.end(); i++)
        {
            if (i->second.name == name) {
                return i->second;
            }
        }
        return {};
    }

};

}

namespace ScintillaQt {

Scintilla::NotepadPlusStyle loadXmlStyle(const std::filesystem::path fileName);
void setStyle(ScintillaEdit *editor, const Scintilla::NotepadPlusStyle &style);

}


