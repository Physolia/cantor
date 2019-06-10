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
 */

#include "loadedexpression.h"

#include "lib/imageresult.h"
#include "lib/epsresult.h"
#include "lib/textresult.h"
#include "lib/latexresult.h"
#include "lib/animationresult.h"

#include <QDir>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

LoadedExpression::LoadedExpression( Cantor::Session* session ) : Cantor::Expression( session, false, -1)
{

}

void LoadedExpression::interrupt()
{
    //Do nothing
}

void LoadedExpression::evaluate()
{
    //Do nothing
}

void LoadedExpression::loadFromXml(const QDomElement& xml, const KZip& file)
{
    setCommand(xml.firstChildElement(QLatin1String("Command")).text());

    const QDomNodeList& results = xml.elementsByTagName(QLatin1String("Result"));
    for (int i = 0; i < results.size(); i++)
    {
        const QDomElement& resultElement = results.at(i).toElement();
        const QString& type = resultElement.attribute(QLatin1String("type"));
        if ( type == QLatin1String("text"))
        {
            addResult(new Cantor::TextResult(resultElement.text()));
        }
        else if (type == QLatin1String("image") || type == QLatin1String("latex") || type == QLatin1String("animation"))
        {
            const KArchiveEntry* imageEntry=file.directory()->entry(resultElement.attribute(QLatin1String("filename")));
            if (imageEntry&&imageEntry->isFile())
            {
                const KArchiveFile* imageFile=static_cast<const KArchiveFile*>(imageEntry);
                QString dir=QStandardPaths::writableLocation(QStandardPaths::TempLocation);
                imageFile->copyTo(dir);
                QUrl imageUrl = QUrl::fromLocalFile(QDir(dir).absoluteFilePath(imageFile->name()));
                if(type==QLatin1String("latex"))
                {
                    addResult(new Cantor::LatexResult(resultElement.text(), imageUrl));
                }else if(type==QLatin1String("animation"))
                {
                    addResult(new Cantor::AnimationResult(imageUrl));
                }else if(imageFile->name().endsWith(QLatin1String(".eps")))
                {
                    addResult(new Cantor::EpsResult(imageUrl));
                }else
                {
                    addResult(new Cantor::ImageResult(imageUrl));
                }
            }
        }
    }
}

void LoadedExpression::loadFromJupyter(const QJsonObject& cell)
{
    //Jupyter TODO: handle missing key, like 'source', check that array, and string data inside
    const QJsonArray& sources = cell.value(QLatin1String("source")).toArray();
    QString code;
    for (const QJsonValue& line : sources)
        code += line.toString();
    setCommand(code);
    const QJsonValue idObject = cell.value(QLatin1String("execution_count"));
    if (!idObject.isUndefined() && !idObject.isNull())
        setId(idObject.toInt());


    const QJsonArray& outputs = cell.value(QLatin1String("outputs")).toArray();
    //Jupyter TODO: load all outputs here
    for (QJsonArray::const_iterator iter = outputs.begin(); iter != outputs.end(); iter++)
    {
        bool isCellObject = iter->isObject();
        if (!isCellObject)
            continue;

        const QJsonObject& output = iter->toObject();
        const QString& outputType = output.value(QLatin1String("output_type")).toString();
        if (outputType == QLatin1String("stream"))
        {
            const QJsonArray& textLineArray = output.value(QLatin1String("text")).toArray();
            QString text;
            for (const QJsonValue& line : textLineArray)
                text += line.toString();

            addResult(new Cantor::TextResult(text));
        }
        else if (outputType == QLatin1String("error"))
        {
            //Jupyter TODO: there are colors in tracback, add support for them
            const QJsonArray& tracebackLineArray = output.value(QLatin1String("traceback")).toArray();
            QString traceback;
            for (const QJsonValue& line : tracebackLineArray)
                traceback += line.toString();

            setErrorMessage(traceback);
        }
    }
    if (errorMessage().isEmpty())
        setStatus(Done);
    else
        setStatus(Error);
}
