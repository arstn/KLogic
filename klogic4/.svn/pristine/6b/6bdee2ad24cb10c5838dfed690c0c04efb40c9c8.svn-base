/***************************************************************************
 *   Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "klogic4.h"
#include "KlogicGlobal.h"

#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("KLogic is an application for creating and simulating digital circuits.");

int main(int argc, char **argv)
{
    KAboutData about("klogic", 0, ki18n("KLogic"), KlogicGlobal::Klogic::VERSION, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2011 Andreas Rostin"), KLocalizedString(), 0, "klogic@a-rostin.de");
    about.addAuthor(ki18n("Andreas Rostin"), KLocalizedString(), "klogic@a-rostin.de" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    klogic4 *widget = new klogic4;

    if (app.isSessionRestored()) {
        RESTORE(klogic4);
    } else {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0) {
            widget->show();
        } else {
            int i = 0;
            for (; i < args->count(); i++) {
                widget->show();
            }
        }
        args->clear();
    }

    return app.exec();
}
