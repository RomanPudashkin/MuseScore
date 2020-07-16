#ifndef SCORESTATEOBSERVER_H
#define SCORESTATEOBSERVER_H

#include <QObject>

#include "../scorestypes.h"

namespace Ms {
class ScoreStateObserver : public QObject
{
    Q_OBJECT

public:
    static ScoreStateObserver* instance() {
        static ScoreStateObserver obs;
        return &obs;
    }

    ScoreState currentState() const;

public slots:
    void setCurrentState(ScoreState currentState);

signals:
    void currentStateChanged(ScoreState currentState);

private:
    explicit ScoreStateObserver(QObject* parent = nullptr);

    ScoreState m_currentState = ScoreState::STATE_ALL;
};
}

Q_DECLARE_METATYPE(Ms::ScoreState)

#endif // SCORESTATEOBSERVER_H
