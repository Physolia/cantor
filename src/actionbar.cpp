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

#include "actionbar.h"
#include "worksheetentry.h"

#include <QToolButton>
#include <QGraphicsProxyWidget>

ActionBar::ActionBar(WorksheetEntry* parent)
    : QGraphicsObject(parent)
{
    setPos(parent->size().width(), 0);
    m_pos = 0;
    m_height = 0;
}

ActionBar::~ActionBar()
{
}

void ActionBar::addButton(QToolButton* button)
{
    QGraphicsProxyWidget* widget = new QGraphicsProxyWidget(this);
    widget->setWidget(button);
    m_pos -= button->width();
    m_height = m_height > button->height() ? m_height : button->height();
    widget->setPos(m_pos, 0);
}

void ActionBar::addSpace()
{
    m_pos -= 8;
}

void ActionBar::updatePosition()
{
    setPos(parentEntry()->size().width(), 0);
}

WorksheetEntry* ActionBar::parentEntry()
{
    return qgraphicsitem_cast<WorksheetEntry*>(parentItem());
}

QRectF ActionBar::boundingRect() const
{
    return QRectF(m_pos, 0, -m_pos, m_height);
}

void ActionBar::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}

