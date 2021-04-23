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
    Copyright (C) 2009 Alexander Rieder <alexanderrieder@gmail.com>
    Copyright (C) 2019 Alexander Semke <alexander.semke@web.de>
 */

#include "rbackend.h"
#include "rsession.h"
#include "rextensions.h"
#include "settings.h"
#include "rsettingswidget.h"

RBackend::RBackend(QObject* parent,const QList<QVariant>& args) : Cantor::Backend(parent, args)
{
    new RScriptExtension(this);
    new RPlotExtension(this);
    new RVariableManagementExtension(this);
}

RBackend::~RBackend()
{
    qDebug()<<"Destroying RBackend";
}

QString RBackend::id() const
{
    return QLatin1String("r");
}

QString RBackend::version() const
{
    return QLatin1String("Undefined");
}

Cantor::Session* RBackend::createSession()
{
    qDebug()<<"Spawning a new R session";

    return new RSession(this);
}

Cantor::Backend::Capabilities RBackend::capabilities() const
{
    qDebug()<<"Requesting capabilities of RSession";
    Cantor::Backend::Capabilities cap=
        SyntaxHighlighting|
        Completion |
        InteractiveMode;

    if (RServerSettings::variableManagement())
        cap |= VariableManagement;

    return cap;
}

bool RBackend::requirementsFullfilled(QString* const reason) const
{
#ifdef Q_OS_WIN
    const QString& path = QStandardPaths::findExecutable(QLatin1String("cantor_rserver.exe"));
#else
    const QString& path = QStandardPaths::findExecutable(QLatin1String("cantor_rserver"));
#endif
    return Cantor::Backend::checkExecutable(QLatin1String("Cantor RServer"), path, reason);
}

QWidget* RBackend::settingsWidget(QWidget* parent) const
{
    return new RSettingsWidget(parent);
}

KConfigSkeleton* RBackend::config() const
{
    return RServerSettings::self();
}

QUrl RBackend::helpUrl() const
{
    const QUrl& localDoc = RServerSettings::self()->localDoc();
    if (!localDoc.isEmpty())
        return localDoc;
    else
    return QUrl(i18nc("the url to the documentation of R, please check if there is a translated version and use the correct url",
                 "https://cran.r-project.org/manuals.html"));
}

QString RBackend::description() const
{
    return i18n("<b>R</b> is a language and environment for statistical computing and graphics, similar to the S language and environment. <br/>"\
                "It provides a wide variety of statistical (linear and nonlinear modelling, "\
                "classical statistical tests, time-series analysis, classification, clustering, ...) "\
                "and graphical techniques, and is highly extensible. The S language is often the "\
                "vehicle of choice for research in statistical methodology, "\
                "and R provides an Open Source route to participation in that activity.");
}

K_PLUGIN_FACTORY_WITH_JSON(rbackend, "rbackend.json", registerPlugin<RBackend>();)
#include "rbackend.moc"
