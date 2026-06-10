#pragma once
#include <QColor>
#include <QSettings>
#include <qcolor.h>
#include "../../Constants.h"
#include "GuiConstants.h"


using namespace Constants;
using namespace GuiConstants;

struct GridSettings {
    bool   visible     = true;
    double spacing     = GridConstants::SPACING;
    bool   snapEnabled = false;
    QColor axisColor   = Colors::AXISCOLOR;
    QColor gridColor   = Colors::GRIDCOLOR;
    QColor labelColor  = Colors::LABELCOLOR;
};



struct ColorScheme {
    QColor background   = Colors::BACKGROUND;
    QColor point        = Colors::POINT;
    QColor pointFill    = Colors::POINTFILL;
    QColor line         = Colors::LINE;
    QColor circle       = Colors::CIRCLE;
    QColor selected     = Colors::SELECT;
    QColor highlighted  = Colors::HIGHLIGHT;
    QColor construction = Colors::CONSTRUCTION;
    QColor watermark    = Colors::WATRMARK;
};

struct General {
    struct RecentFiles {
        QStringList files;
        int maxCount = GeneralConstants::RECENT_MAXCOUNT;
    } recentFiles;
    QString language = "en";
};

class AppSettings {
public:
    static auto instance() -> AppSettings&;

    void addRecentFile(const QString& fileName);
    void load();
    void save();
    void resetToDefaults();

    GridSettings grid;
    ColorScheme  colors;
    General general;

private:
    AppSettings();
    QSettings m_settings;
};