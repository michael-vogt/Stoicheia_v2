#pragma once
#include <QColor>
#include <QSettings>

struct GridSettings {
    bool   visible     = true;
    double spacing     = 50.0;
    bool   snapEnabled = false;
    QColor axisColor   = QColor(160, 160, 160);
    QColor gridColor   = QColor(220, 220, 220);
    QColor labelColor  = QColor(120, 120, 120);
};

struct ColorScheme {
    QColor background   = QColor(245, 245, 245);
    QColor point        = QColor(0,   0,   0);
    QColor pointFill    = QColor(255, 255, 255);
    QColor line         = QColor(0,   0,   0);
    QColor circle       = QColor(0,   0,   0);
    QColor selected     = QColor(0,   0,   255);
    QColor highlighted  = QColor(255, 140, 0);
    QColor construction = QColor(100, 100, 200);
    QColor watermark    = QColor(0,   0,   0,  20);
};

struct General {
    struct RecentFiles {
        QStringList files;
        int maxCount = 5;
    } recentFiles;
    QString language = "en";
};

class AppSettings {
public:
    static AppSettings& instance();

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