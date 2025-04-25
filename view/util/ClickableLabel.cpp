#include "ClickableLabel.h"
#include <QVBoxLayout>

ClickableLabel::ClickableLabel(QWidget* parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
}

bool ClickableLabel::event(QEvent* e) {
    // Entrata del mouse nell'area della QLabel
    if (e->type() == QEvent::Enter) {
        setStyleSheet("border:2px solid #3366FF; border-radius:6px;");
    }   // uscita dall'area della QLabel
    else if (e->type() == QEvent::Leave) {
        setStyleSheet("");
    }
    return QLabel::event(e);    // evita di fermare la normale gestione degli eventi
}

void ClickableLabel::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        emit clicked();
    }
    QLabel::mousePressEvent(e);
}
