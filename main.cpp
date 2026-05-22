#include <iostream>
#include <QApplication>
#include <qevent.h>
#include <QTranslator>

#include "geometry/geometry.h"
#include "constructions/constructions.h"
#include "ui/MainWindow.h"
#include "ui/dialogs/AppSettings.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    AppSettings::instance().load();
    QString languageCode = AppSettings::instance().ui.language;

    QTranslator translator;
    if (translator.load(QString(":/i18n/app_%1.qm").arg(languageCode))) {
        app.installTranslator(&translator);
    } else {
        std::cerr << "Error loading translator" << std::endl;
    }

    MainWindow window("Στοιχεῖα", &translator);
    window.drawingBoard()->applySettings();

    if (argc > 1) {
        const QString filename(argv[1]);
        window.fileManager()->openFile(filename);
    }

    window.showMaximized();

    return QApplication::exec();
}
