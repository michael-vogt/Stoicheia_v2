#pragma once
#include <qstring.h>
#include <QObject>

class Command : public QObject {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;

    virtual QString description() const = 0;

};
