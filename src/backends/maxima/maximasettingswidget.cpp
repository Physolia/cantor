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
    Copyright (C) 2020 Alexander Semke <alexander.semke@web.de>
    Copyright (C) 2020 Shubham <aryan100jangid@gmail.com>
 */

#include "maximasettingswidget.h"
#include "maximabackend.h"
#include "../qthelpconfig.h"

MaximaSettingsWidget::MaximaSettingsWidget(QWidget* parent, const QString& id) : QWidget(parent)
{
    setupUi(this);

    // Add QtHelp widget
    QtHelpConfig* docWidget = new QtHelpConfig(id);
    auto hboxLayout = new QHBoxLayout(tabDocumentation);
    hboxLayout->addWidget(docWidget);
}
