#ifndef IMAGEHELPERS_H
#define IMAGEHELPERS_H

#include <QPixmap>
#include <QString>
#include <QSize>
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "ClickableLabel.h"

QPixmap loadPixmapOrPlaceholder(const QString& path);

QPixmap centerCropSquare(const QPixmap& src, const QSize& target);

QPixmap makeCircularPixmap(const QPixmap& src, int diameter);

ClickableLabel* createImageLabel(const std::string& imagePathStr,
                                 const QSize& size,
                                 bool isArtist,
                                 QWidget* parent);

#endif // IMAGEHELPERS_H
