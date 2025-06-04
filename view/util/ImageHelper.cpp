#include "ImageHelper.h"

QPixmap loadPixmapOrPlaceholder(const QString& path) {
    QPixmap pm;
    if (!path.isEmpty() && QFile::exists(path) && pm.load(path)) {
        return pm;
    }
    pm.load(":/icons/placeholder.png");
    return pm;
}

QPixmap centerCropSquare(const QPixmap& src, const QSize& target) {
    if (src.isNull()) {
        return QPixmap(target);
    }
    QPixmap expanded = src.scaled(target,
                                  Qt::KeepAspectRatioByExpanding,
                                  Qt::SmoothTransformation);
    int offsetX = (expanded.width()  - target.width())  / 2;
    int offsetY = (expanded.height() - target.height()) / 2;
    return expanded.copy(offsetX, offsetY, target.width(), target.height());
}

QPixmap makeCircularPixmap(const QPixmap& src, int diameter) {
    QPixmap circle(diameter, diameter);
    circle.fill(Qt::transparent);

    QPainter painter(&circle);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath clipPath;
    clipPath.addEllipse(circle.rect());
    painter.setClipPath(clipPath);

    if (!src.isNull()) {
        QPixmap scaled = src.scaled(diameter, diameter,
                                    Qt::KeepAspectRatioByExpanding,
                                    Qt::SmoothTransformation);
        painter.drawPixmap(circle.rect(), scaled);
    }
    return circle;
}

ClickableLabel* createImageLabel(const std::string& imagePathStr,
                                 const QSize& size,
                                 bool isArtist,
                                 QWidget* parent)
{
    QString path = QString::fromStdString(imagePathStr);
    QPixmap original = loadPixmapOrPlaceholder(path);

    QPixmap finalPixmap;
    if (isArtist) {
        finalPixmap = makeCircularPixmap(original, size.width());
    } else {
        finalPixmap = centerCropSquare(original, size);
    }

    ClickableLabel* lbl = new ClickableLabel(parent);
    lbl->setFixedSize(size);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setPixmap(finalPixmap);

    QObject::connect(lbl, &ClickableLabel::clicked, [original, parent]() {
        // Mostra un QDialog con anteprima ingrandita
        QDialog* dlg = new QDialog(parent);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setWindowTitle("Anteprima");

        QVBoxLayout* layout = new QVBoxLayout(dlg);
        QLabel* big = new QLabel(dlg);
        big->setPixmap(original.scaled(600, 600,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        layout->addWidget(big);

        dlg->exec();
    });

    return lbl;
}