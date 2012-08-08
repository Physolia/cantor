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

#ifndef ACTIONBAR_H
#define ACTIONBAR_H

#include <QGraphicsObject>

class QToolButton;

class WorksheetEntry;

class ActionBar : public QGraphicsObject
{
  Q_OBJECT
  public:
    ActionBar(WorksheetEntry* parent);
    ~ActionBar();

    void addButton(QToolButton* button);
    void addSpace();

    void updatePosition();
    
    WorksheetEntry* parentEntry();

    QRectF boundingRect() const;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  private:
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity);
    qreal m_pos;
    qreal m_height;
};

#endif // ACTIONBAR_H
