#pragma once
#include <QColor>
#include <QSettings>
#include <qcolor.h>

constexpr double DEFAULT_GRID_SPACING = 50.0;
constexpr QColor DEFAULT_GRID_AXISCOLOR(160, 160, 160);
constexpr QColor DEFAULT_GRID_GRIDCOLOR(220, 220, 220);
constexpr QColor DEFAULT_GRID_LABELCOLOR(120, 120, 120);

struct GridSettings {
    bool   visible     = true;
    double spacing     = DEFAULT_GRID_SPACING;
    bool   snapEnabled = false;
    QColor axisColor   = DEFAULT_GRID_AXISCOLOR;
    QColor gridColor   = DEFAULT_GRID_GRIDCOLOR;
    QColor labelColor  = DEFAULT_GRID_LABELCOLOR;
};

constexpr QColor DEFAULT_COLORS_BG(245, 245, 245);
constexpr QColor DEFAULT_COLORS_PF(255, 255, 255);
constexpr QColor DEFAULT_COLOR_SELECT(0,0,255);
constexpr QColor DEFAULT_COLOR_HIGHLIGHT(255, 140, 0);
constexpr QColor DEFAULT_COLOR_CONSTRUCTION(100, 100, 200);
constexpr QColor DEFAULT_COLOR_WATRMARK(0, 0, 0, 20);

struct ColorScheme {
    QColor background   = DEFAULT_COLORS_BG;
    QColor point        = QColor(0,   0,   0);
    QColor pointFill    = DEFAULT_COLORS_PF;
    QColor line         = QColor(0,   0,   0);
    QColor circle       = QColor(0,   0,   0);
    QColor selected     = DEFAULT_COLOR_SELECT;
    QColor highlighted  = DEFAULT_COLOR_HIGHLIGHT;
    QColor construction = DEFAULT_COLOR_CONSTRUCTION;
    QColor watermark    = DEFAULT_COLOR_WATRMARK;
};

constexpr int DEFAULT_GENERAL_RECENT_MAXCOUNT = 5;

struct General {
    struct RecentFiles {
        QStringList files;
        int maxCount = DEFAULT_GENERAL_RECENT_MAXCOUNT;
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