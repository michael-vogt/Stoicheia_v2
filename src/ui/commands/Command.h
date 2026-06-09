#pragma once
#include <qstring.h>
#include <QObject>

class Command : public QObject {
public:
    ~Command() override = default;

    virtual void execute() = 0;
    virtual void undo() = 0;

    [[nodiscard]] virtual auto description() const -> QString = 0;

};
