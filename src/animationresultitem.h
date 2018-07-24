/*
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301, USA.

    ---
    Copyright (C) 2012 Martin Kuettler <martin.kuettler@gmail.com>
 */

#ifndef ANIMATIONRESULTITEM_H
#define ANIMATIONRESULTITEM_H

#include "imageresultitem.h"

#include <QMovie>

class WorksheetEntry;

class AnimationResultItem : public WorksheetImageItem, public ResultItem
{
  Q_OBJECT

  public:
    AnimationResultItem(QGraphicsObject* parent);
    ~AnimationResultItem() override;

    using WorksheetImageItem::setGeometry;
    double setGeometry(double x, double y, double w) Q_DECL_OVERRIDE;
    void populateMenu(QMenu* menu, QPointF pos) Q_DECL_OVERRIDE;

    ResultItem* updateFromResult(Cantor::Result*) Q_DECL_OVERRIDE;

    void deleteLater() Q_DECL_OVERRIDE;

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    double width() const Q_DECL_OVERRIDE;
    double height() const Q_DECL_OVERRIDE;
    CommandEntry* parentEntry();
    Cantor::Result* result() Q_DECL_OVERRIDE;

  Q_SIGNALS:
    void removeResult();

  protected Q_SLOTS:
    void saveResult();
    void stopMovie();
    void pauseMovie();

  private:
    void setMovie(QMovie* movie);

  private Q_SLOTS:
    void updateFrame();
    void updateSize(const QSize& size);

  private:
    double m_height;
    QMovie* m_movie;
};

#endif //ANIMATIONRESULTITEM_H

