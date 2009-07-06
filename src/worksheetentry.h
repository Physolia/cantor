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

#ifndef _WORKSHEETENTRY_H
#define _WORKSHEETENTRY_H

#include <QObject>
#include <QTextTableCell>
#include "lib/expression.h"

class QTextDocument;
namespace MathematiK{
    class Expression;
    class TabCompletionObject;
}
class Worksheet;

/**
   An entry in the Worksheet. it contains:
     1 Row to take command from the user
     0+ Rows for addition questions/answers from the backend
     0/1 Row for contextual help like Tab Completion offers
     1 Row for the Result
 **/

class WorksheetEntry : public QObject
{
  Q_OBJECT
  public:
    static const QString Prompt;

    WorksheetEntry(QTextCursor position, Worksheet* parent);
    ~WorksheetEntry();

    QString command();
    void setExpression(MathematiK::Expression* expr);
    MathematiK::Expression* expression();

    //returns the line of the command cell, the textCursor is currently in
    QString currentLine(const QTextCursor& cursor);

    bool isEmpty();

    //only used for saving/loading. normally
    //you should create an expression, and set the result there
    void setResult(const QString& html);

    void setTabCompletion(MathematiK::TabCompletionObject* tc);

    QTextTableCell commandCell();
    QTextTableCell actualInformationCell();
    QTextTableCell resultCell();

    void addInformation();

    int firstPosition();
    int lastPosition();

    bool contains(const QTextCursor& cursor);
    bool isInCurrentInformationCell(const QTextCursor& cursor);
    bool isInCommandCell(const QTextCursor& cursor);
    bool isInPromptCell(const QTextCursor& cursor);

    //checks if this entry has still anything needed (aka the user didn't delete anything
    //like the prompt. Readd missing things
    void checkForSanity();

  public slots:
    void updateResult();
    void expressionChangedStatus(MathematiK::Expression::Status status);
    void showAdditionalInformationPrompt(const QString& question);
    void applyTabCompletion();
  private slots:
    void resultDeleted();
  private:
    QTextTable* m_table;
    QTextTableCell m_commandCell;
    QTextTableCell m_contextHelpCell;
    QList<QTextTableCell> m_informationCells;
    QTextTableCell m_errorCell;
    QTextTableCell m_resultCell;
    MathematiK::Expression* m_expression;
    Worksheet* m_worksheet;

    MathematiK::TabCompletionObject* m_tabCompletionObject;
};

#endif /* _WORKSHEETENTRY_H */
