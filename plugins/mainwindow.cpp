/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt for Python examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "pythonutils.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <QtGui/QAction>
#include <QtGui/QFontDatabase>
#include <QtGui/QIcon>

#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

static const char defaultScript[] = R"(
import AppLib
print("Hello, world")
mainWindow.testFunction1()
)";

MainWindow::MainWindow()
    : m_scriptEdit(new QPlainTextEdit(QString::fromLatin1(defaultScript).trimmed(), this))
{
    setWindowTitle(tr("Scriptable Application"));

    // Icons
    const QIcon runIcon = QIcon::fromTheme(QStringLiteral("system-run"));
    const QIcon quitIcon = QIcon::fromTheme(QStringLiteral("application-exit"));
    const QIcon clearIcon = QIcon::fromTheme(QStringLiteral("edit-clear"));
    const QIcon aboutIcon = QIcon::fromTheme(QStringLiteral("help-about"));

    // 'File' Menu with Actions
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *runAction = fileMenu->addAction(runIcon, tr("&Run..."), this, &MainWindow::slotRunScript);
    runAction->setShortcut(Qt::CTRL | Qt::Key_R);
    QAction *diagnosticAction = fileMenu->addAction(tr("&Print Diagnostics"), this, &MainWindow::slotPrintDiagnostics);
    diagnosticAction->setShortcut(Qt::CTRL | Qt::Key_D);
    fileMenu->addAction(tr("&Invoke testFunction1()"), this, &MainWindow::testFunction1);
    QAction *quitAction = fileMenu->addAction(quitIcon, tr("&Quit"), qApp, &QCoreApplication::quit);
    quitAction->setShortcut(Qt::CTRL | Qt::Key_Q);

    // 'Edit' Menu with Actions
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QAction *clearAction = editMenu->addAction(clearIcon, tr("&Clear"), m_scriptEdit, &QPlainTextEdit::clear);

    // 'Help' Menu with Actions
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(aboutIcon, tr("&About Qt"), qApp, &QApplication::aboutQt);

    m_toolBar = new QToolBar;
    addToolBar(m_toolBar);
    m_toolBar->addAction(quitAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(clearAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(runAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(aboutAction);

    m_scriptEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    if (!PythonUtils::bindAppObject("__main__", "mainWindow", PythonUtils::MainWindowType, this))
       statusBar()->showMessage(tr("Error loading the application module"));

    m_pluginMenu = new QListWidget();

    // Init plugins
    const QFileInfo outputDir("plugins");
    if (!outputDir.isDir()) {
        qWarning() << "No plugins detected";
    } else {
        QStringList l;
        QDir directory("plugins");
        QStringList plugins = directory.entryList(QStringList() << "*.py", QDir::Files);
        for(QString &filename: plugins) {
            QString filePath = QDir("plugins").filePath(filename);
            QFile f(filePath);
            if (!f.open(QFile::ReadOnly | QFile::Text)) break;
            QTextStream in(&f);
            for (auto line: in.readAll().split("\n"))
                l << line;
            QListWidgetItem *item = new QListWidgetItem(filename.replace("plugin_", "").replace(".py", ""));
            item->setSizeHint(QSize(item->sizeHint().width(), 30));
            m_pluginMenu->addItem(item);
        }

        if (PythonUtils::runScript(l)) {
            qDebug() << "Plugins loaded";
        } else {
            qDebug() << "Error while loading plugins";
        }
    }


    QWidget *centralWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("Plugins"));
    leftLayout->addWidget(m_pluginMenu);
    layout->addLayout(leftLayout, 1);
    layout->addWidget(m_scriptEdit, 3);
    centralWidget->setLayout(layout);

    this->setStyleSheet("font-size: 12px;");

    setCentralWidget(centralWidget);

}

void MainWindow::slotRunScript()
{
    const QStringList script = m_scriptEdit->toPlainText().trimmed().split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    if (!script.isEmpty())
        runScript(script);
}

void MainWindow::slotPrintDiagnostics()
{
    const QStringList script = QStringList()
            << "import sys" << "print('Path=', sys.path)" << "print('Executable=', sys.executable)";
    runScript(script);
}

void MainWindow::runScript(const QStringList &script)
{
    if (!::PythonUtils::runScript(script))
        statusBar()->showMessage(tr("Error running script"));
}

void MainWindow::testFunction1()
{
    static int n = 1;
    QString message;
    QTextStream(&message) << __FUNCTION__ << " called #" << n++;
    qDebug().noquote() << message;
    statusBar()->showMessage(message);
}

QToolBar* MainWindow::getToolBar()
{
    return m_toolBar;
}

QPlainTextEdit* MainWindow::getTextEdit()
{
    return m_scriptEdit;
}

QListWidget* MainWindow::getPluginMenu()
{
    return m_pluginMenu;
}
