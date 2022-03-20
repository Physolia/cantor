/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2009 Alexander Rieder <alexanderrieder@gmail.com>
    SPDX-FileCopyrightText: 2018-2022 by Alexander Semke (alexander.semke@web.de)
*/

#include "testmaxima.h"

#include "session.h"
#include "backend.h"
#include "expression.h"
#include "result.h"
#include "textresult.h"
#include "imageresult.h"
#include "epsresult.h"
#include "syntaxhelpobject.h"
#include "completionobject.h"
#include "defaultvariablemodel.h"

#include <config-cantorlib.h>

QString TestMaxima::backendName()
{
    return QLatin1String("maxima");
}


void TestMaxima::testSimpleCommand()
{
    auto* e=evalExp( QLatin1String("2+2") );

    QVERIFY( e!=nullptr );
    QVERIFY( e->result()!=nullptr );

    QCOMPARE( cleanOutput( e->result()->data().toString() ), QLatin1String("4") );
}

void TestMaxima::testMultilineCommand()
{
    auto* e = evalExp( QLatin1String("2+2;3+3") );

    QVERIFY(e != nullptr);
    QVERIFY(e->results().size() == 2);

    QCOMPARE(e->results().at(0)->data().toString(), QLatin1String("4"));
    QCOMPARE(e->results().at(1)->data().toString(), QLatin1String("6"));
}

//WARNING: for this test to work, Integration of Plots must be enabled
//and CantorLib must be compiled with EPS-support
void TestMaxima::testPlot()
{
    if(QStandardPaths::findExecutable(QLatin1String("gnuplot")).isNull())
    {
        QSKIP("gnuplot not found, maxima needs it for plotting", SkipSingle);
    }

    auto* e=evalExp( QLatin1String("plot2d(sin(x), [x, -10,10])") );

    QVERIFY( e!=nullptr );
    QVERIFY( e->result()!=nullptr );

    if(!e->result())
    {
        waitForSignal(e, SIGNAL(gotResult()));
    }

    QCOMPARE( e->result()->type(), (int)Cantor::ImageResult::Type );
    QVERIFY( !e->result()->data().isNull() );
    QVERIFY( e->errorMessage().isNull() );
}

void TestMaxima::testPlotWithAnotherTextResults()
{
    if(QStandardPaths::findExecutable(QLatin1String("gnuplot")).isNull())
    {
        QSKIP("gnuplot not found, maxima needs it for plotting", SkipSingle);
    }

    auto* e=evalExp( QLatin1String(
        "2*2; \n"
        "plot2d(sin(x), [x, -10,10]); \n"
        "4*4;"
    ));

    if (e->results().at(1)->type() == Cantor::TextResult::Type)
        waitForSignal(e, SIGNAL(resultReplaced));

    QVERIFY( e!=nullptr );
    QVERIFY( e->errorMessage().isNull() );
    QCOMPARE(e->results().size(), 3);

    QCOMPARE(e->results().at(0)->data().toString(), QLatin1String("4"));

    QCOMPARE( e->results().at(1)->type(), (int)Cantor::ImageResult::Type );
    QVERIFY( !e->results().at(1)->data().isNull() );

    QCOMPARE(e->results().at(2)->data().toString(), QLatin1String("16"));
}

void TestMaxima::testInvalidSyntax()
{
    auto* e=evalExp( QLatin1String("2+2*(") );

    QVERIFY( e!=nullptr );
    QVERIFY( e->status()==Cantor::Expression::Error );
}

void TestMaxima::testWarning01()
{
    auto* e = evalExp( QLatin1String("rat(0.75*10)") );

    QVERIFY(e != nullptr);
    QVERIFY(e->results().size() == 2); //two results, the warning and the actual result of the calculation

    //the actual warning string "rat: replaced 7.5 by 15/2 = 7.5" which we don't checked since it's translated,
    //we just check it's existance.
    auto* result = dynamic_cast<Cantor::TextResult*>(e->results().at(0));
    QVERIFY(e != nullptr);
    QVERIFY(result->data().toString().isEmpty() == false);
    QVERIFY(result->isWarning() == true);

    //the result of the calculation
    QCOMPARE(e->results().at(1)->data().toString(), QLatin1String("15/2"));
}

/*!
 * test the output of the tex() function which is similarly formatted as other functions producing warning
 * but which shouldn't be treated as a warning.
 * */
void TestMaxima::testWarning02()
{
    auto* e = evalExp( QLatin1String("tex(\"sin(x)\")") );

    QVERIFY(e != nullptr);
    QVERIFY(e->results().size() == 2); //two results, the TeX output and an additional 'false'

    //the actual TeX string is of no interest for us, we just check it's existance.
    auto* result = dynamic_cast<Cantor::TextResult*>(e->results().at(0));
    QVERIFY(e != nullptr);
    QVERIFY(result->data().toString().isEmpty() == false);
    QVERIFY(result->isWarning() == false);
}

void TestMaxima::testExprNumbering()
{
    auto* e=evalExp( QLatin1String("kill(labels)") ); //first reset the labels

    e=evalExp( QLatin1String("2+2") );
    QVERIFY( e!=nullptr );
    int id=e->id();
    QCOMPARE( id, 1 );

    e=evalExp( QString::fromLatin1("%o%1+1" ).arg( id ) );
    QVERIFY( e != nullptr );
    QVERIFY( e->result()!=nullptr );
    QCOMPARE( cleanOutput( e->result()->data().toString() ), QLatin1String( "5" ) );
}

void TestMaxima::testCommandQueue()
{
    //only wait for the last Expression to return, so the queue gets
    //actually filled

    auto* e1=session()->evaluateExpression(QLatin1String("0+1"));
    auto* e2=session()->evaluateExpression(QLatin1String("1+1"));
    auto* e3=evalExp(QLatin1String("1+2"));

    QVERIFY(e1!=nullptr);
    QVERIFY(e2!=nullptr);
    QVERIFY(e3!=nullptr);

    QVERIFY(e1->result());
    QVERIFY(e2->result());
    QVERIFY(e3->result());

    QCOMPARE(cleanOutput(e1->result()->data().toString()), QLatin1String("1"));
    QCOMPARE(cleanOutput(e2->result()->data().toString()), QLatin1String("2"));
    QCOMPARE(cleanOutput(e3->result()->data().toString()), QLatin1String("3"));
}

void TestMaxima::testSimpleExpressionWithComment()
{
    auto* e=evalExp(QLatin1String("/*this is a comment*/2+2"));
    QVERIFY(e!=nullptr);
    QVERIFY(e->result()!=nullptr);

    QCOMPARE(cleanOutput(e->result()->data().toString()), QLatin1String("4"));
}

void TestMaxima::testCommentExpression()
{
    auto* e=evalExp(QLatin1String("/*this is a comment*/"));
    QVERIFY(e!=nullptr);
    QVERIFY(e->result()==nullptr||e->result()->data().toString().isEmpty());
}

void TestMaxima::testNestedComment()
{
    auto* e=evalExp(QLatin1String("/*/*this is still a comment*/*/2+2/*still/*a*/comment*//**/"));
    QVERIFY(e!=nullptr);
    QVERIFY(e->result()!=nullptr);

    QCOMPARE(cleanOutput(e->result()->data().toString()), QLatin1String("4"));
}

void TestMaxima::testUnmatchedComment()
{
    auto* e=evalExp(QLatin1String("/*this comment doesn't end here!"));
    QVERIFY(e!=nullptr);
    QVERIFY(e->result()==nullptr);
    QVERIFY(e->status()==Cantor::Expression::Error);
}

void TestMaxima::testInvalidAssignment()
{
    auto* e=evalExp(QLatin1String("0:a"));
    QVERIFY(e!=nullptr);
    //QVERIFY(e->result()==0);
    //QVERIFY(e->status()==Cantor::Expression::Error);

    if(session()->status()==Cantor::Session::Running)
        waitForSignal(session(), SIGNAL(statusChanged(Cantor::Session::Status)));

    //make sure we didn't screw up the session
    auto* e2=evalExp(QLatin1String("2+2"));
    QVERIFY(e2!=nullptr);
    QVERIFY(e2->result()!=nullptr);

    QCOMPARE(cleanOutput(e2->result()->data().toString()), QLatin1String("4"));
}

void TestMaxima::testInformationRequest()
{
    auto* e=session()->evaluateExpression(QLatin1String("integrate(x^n,x)"));
    QVERIFY(e!=nullptr);
    waitForSignal(e, SIGNAL(needsAdditionalInformation(QString)));
    e->addInformation(QLatin1String("N"));

    waitForSignal(e, SIGNAL(statusChanged(Cantor::Expression::Status)));
    QVERIFY(e->result()!=nullptr);

    QCOMPARE(cleanOutput(e->result()->data().toString()), QLatin1String("x^(n+1)/(n+1)"));
}

void TestMaxima::testSyntaxHelp()
{
    Cantor::SyntaxHelpObject* help = session()->syntaxHelpFor(QLatin1String("simplify_sum"));
    help->fetchSyntaxHelp();
    waitForSignal(help, SIGNAL(done()));

    bool trueHelpMessage= help->toHtml().contains(QLatin1String("simplify_sum"));
    bool problemsWithMaximaDocs = help->toHtml().contains(QLatin1String("INTERNAL-SIMPLE-FILE-ERROR"));
    QVERIFY(trueHelpMessage || problemsWithMaximaDocs);
}

void TestMaxima::testCompletion()
{
    Cantor::CompletionObject* help = session()->completionFor(QLatin1String("ask"), 3);
    waitForSignal(help, SIGNAL(fetchingDone()));

    // Checks all completions for this request
    // This correct for Maxima 5.41.0
    const QStringList& completions = help->completions();
    QVERIFY(completions.contains(QLatin1String("asksign")));
    QVERIFY(completions.contains(QLatin1String("askinteger")));
    QVERIFY(completions.contains(QLatin1String("askexp")));
}

void TestMaxima::testHelpRequest()
{
    //execute "??print"
    auto* e = session()->evaluateExpression(QLatin1String("??print"));
    QVERIFY(e != nullptr);

    //help result will be shown, but maxima still expects further input
    waitForSignal(e, SIGNAL(needsAdditionalInformation(QString)));
    if (e->status() == Cantor::Expression::Computing)
        waitForSignal(e, SIGNAL(statusChanged(Cantor::Expression::Status)));
    QVERIFY(e->status() != Cantor::Expression::Done);
    QVERIFY(e->results().size() == 1); //two results, the warning and the actual result of the calculation

    //ask for help for the first flag of the print command
    e->addInformation(QLatin1String("0"));

    //no further input is required, we're done
    waitForSignal(session(), SIGNAL(statusChanged(Cantor::Session::Status)));
    if (e->status() == Cantor::Expression::Computing)
        waitForSignal(e, SIGNAL(statusChanged(Cantor::Expression::Status)));
    QVERIFY(e->status() == Cantor::Expression::Done);
    QVERIFY(e->results().size() == 2); //two results, the warning and the actual result of the calculation
}

void TestMaxima::testVariableModel()
{
    QAbstractItemModel* model = session()->variableModel();
    QVERIFY(model != nullptr);

    auto* e1=evalExp(QLatin1String("a: 15"));
    auto* e2=evalExp(QLatin1String("a: 15; b: \"Hello, world!\""));
    auto* e3=evalExp(QLatin1String("l: [1,2,3]"));
    QVERIFY(e1!=nullptr);
    QVERIFY(e2!=nullptr);
    QVERIFY(e3!=nullptr);

    if(session()->status()==Cantor::Session::Running)
        waitForSignal(session(), SIGNAL(statusChanged(Cantor::Session::Status)));

    QCOMPARE(3, model->rowCount());

    QVariant name = model->index(0,0).data();
    QCOMPARE(name.toString(),QLatin1String("a"));

    QVariant value = model->index(0,1).data();
    QCOMPARE(value.toString(),QLatin1String("15"));

    QVariant name1 = model->index(1,0).data();
    QCOMPARE(name1.toString(),QLatin1String("b"));

    QVariant value1 = model->index(1,1).data();
    QCOMPARE(value1.toString(),QLatin1String("\"Hello, world!\""));

    QVariant name2 = model->index(2,0).data();
    QCOMPARE(name2.toString(),QLatin1String("l"));

    QVariant value2 = model->index(2,1).data();
    QCOMPARE(value2.toString(),QLatin1String("[1,2,3]"));
}

void TestMaxima::testLispMode01()
{
    //switch to the Lisp-mode
    auto* e1 = evalExp(QLatin1String("to_lisp();"));
    QVERIFY(e1 != nullptr);

    //evaluate a Lisp command and check the result
    auto* e2 = evalExp(QLatin1String("(cons 'a 'b)"));
    QVERIFY(e2 != nullptr);
    QVERIFY(e2->result() != nullptr);
    QCOMPARE(cleanOutput(e2->result()->data().toString()), QLatin1String("(A . B)"));

    //switch back to Maxima mode
    auto* e3 = evalExp(QLatin1String("(to-maxima)"));
    QVERIFY(e3 != nullptr);

    //evaluate a simple Maxima command
    auto* e4 = evalExp(QLatin1String("5+5"));
    QVERIFY(e4 != nullptr);

    //TODO: doesn't work in the test, works in Cantor though...
//     QVERIFY(e4->result() != nullptr);
//     QCOMPARE(cleanOutput(e4->result()->data().toString()), QLatin1String("10"));
}

void TestMaxima::testLoginLogout()
{
    // Logout from session twice and all must works fine
    session()->logout();
    session()->logout();

    // Login in session twice and all must works fine
    session()->login();
    session()->login();
}

void TestMaxima::testRestartWhileRunning()
{
    auto* e1=session()->evaluateExpression(QLatin1String(":lisp (sleep 5)"));

    session()->logout();
    QCOMPARE(e1->status(), Cantor::Expression::Interrupted);
    session()->login();

    auto* e2=evalExp( QLatin1String("2+2") );

    QVERIFY(e2 != nullptr);
    QVERIFY(e2->result() != nullptr);

    QCOMPARE(cleanOutput(e2->result()->data().toString() ), QLatin1String("4"));
}

QTEST_MAIN( TestMaxima )

