#pragma once
#include <QColor>
#include <QSettings>
#include <qcolor.h>
#include "../../Constants.h"




struct GridSettings {
    bool   visible     = true;
    double spacing     = Constants::Grid::SPACING;
    bool   snapEnabled = false;
    QColor axisColor   = Constants::ColorScheme::AXISCOLOR;
    QColor gridColor   = Constants::ColorScheme::GRIDCOLOR;
    QColor labelColor  = Constants::ColorScheme::LABELCOLOR;
};



struct ColorScheme {
    QColor background   = Constants::ColorScheme::BACKGROUND;
    QColor point        = Constants::ColorScheme::POINT;
    QColor pointFill    = Constants::ColorScheme::POINTFILL;
    QColor line         = Constants::ColorScheme::LINE;
    QColor circle       = Constants::ColorScheme::CIRCLE;
    QColor selected     = Constants::ColorScheme::SELECT;
    QColor highlighted  = Constants::ColorScheme::HIGHLIGHT;
    QColor construction = Constants::ColorScheme::CONSTRUCTION;
    QColor watermark    = Constants::ColorScheme::WATRMARK;
};

struct General {
    struct RecentFiles {
        QStringList files;
        int maxCount = Constants::General::RECENT_MAXCOUNT;
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