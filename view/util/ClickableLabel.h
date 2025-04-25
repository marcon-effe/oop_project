// clickablelabel.h
#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

// Estensione di QLabel per renderlo cliccabile
class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = nullptr);

signals:
    void clicked();

protected:
    bool event(QEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
};

#endif // CLICKABLELABEL_H