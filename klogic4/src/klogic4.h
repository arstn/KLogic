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

#ifndef KLOGIC4_H
#define KLOGIC4_H

#include <list>

#include <KXmlGuiWindow>
#include <qlabel.h>
#include <qcombobox.h>

#include "ScrollableCircuitWidget.h"
#include "klogicList.h"
#include "devLib.h"

using namespace std;

class MainWidget;
class KToggleAction;
class CircuitWidget;
class ClientWidget;
class OscWidget;

/**
 * This class serves as the main window for klogic4.  It handles the
 * menus, toolbars and status bars.
 *
 * @short Main window class
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class klogic4 : public KXmlGuiWindow
{
    Q_OBJECT
public:
    klogic4();
    virtual ~klogic4();

    static klogic4 *getInstance();

    static const int simModePERMANENT = 995;
    static const int simModeSINGLE_STEP = 996;
    
    static const int modeCONNECT = 997;
    static const int modeSELECT = 998;
    static const int modeDROP = 999;
    
    static const int MIN_SIMSTEPTIME = 1;
    static const int MAX_SIMSTEPTIME = 400;
    static const int MIN_SIMBURST = 1;
    static const int MAX_SIMBURST = 99999;

    int getFunction();
    int getMode();
    int getSimulationMode();
    
    int getSimStepTime();
    int getSimBurstCount();

	void setFunctionfIN();
	void setFunctionfOUT();

	DevLibrary *getActiveLibrary();

	virtual void keyPressEvent(QKeyEvent * event);
	virtual void closeEvent(QCloseEvent *);

	void circuitHasShortcut();

public slots:
	void setSimBurstCount(int);
	void setSimStepTime(int);
	void circuitDeleted(Circuit *);
	void openClientWidget(Circuit *);
	void clientWidgetClosed(ClientWidget *);
	void repaintClientWidgets(Circuit *);
	void repaintClientWidgets();

	void oscilloscopeClosed();
	void oscilloscopeAnotherCircuit(Circuit *circuit);
	void oscilloscopeCircuitChanged(Circuit *circuit);

	void addDeviceToLibrary(XDevice *device);
	void libraryChanged(int libidx);

private slots:
	void newCircuit();
    void openFile();
    void saveFile();
    void saveFileAs();
    
    void openLibraryMaintenance(XDevice *new_dev = NULL);
    void libraryDeviceSelected();
    
    void selectionMode();
    void connectionMode();
    // drop modes
    void fEQUATION();
    void fAND();
    void fNAND();
    void fOR();
    void fNOR();
    void fXOR();
    void fNOT();
    void fRS();
    void fJK();
    void fDFF();
    void fTFF();
    void fRAM();
    void fTRI();
    void fBUS();
    void fOSC();
    void fSWI();
    void fSS();
    void fLED();
    void fPWR();
    void fTXT();
    void fSUB();
    void fFromLib();

    // simulation modes
    void setSimPermanent();
    void setSimStepByStep();

    void openOscilloscope();
    void closeOscilloscope();
    void quit();
    void enableShortcutWarning();

private:
    void setupActions();
    void performInitialSettings();
    void adjustToolbarsNotHandledByXml();
    void kde4ActionFanCatcher(const int);
	void openOscilloscope(Circuit *circuit);

private:
	static klogic4 *instance;			// The one and only instance

    int function;						// Either a device ID (DROP mode) or the DRAW/SELECT mode constant
    int simulationMode;					// simModePERMANENT or simModeSINGLE_STEP
    int simulationStepTime;				// This is how fast simModePERMANENT runs
    int simulationBurstCount;			// Reduce/eliminate device delays by doing multiple calulations steps within one step

    ClientWidget *clientWidget;			// The client widget embedded in the main window
    list<ClientWidget *> clientWidgets;	// All client widgets including the main
    OscWidget *oscWidget;				// Oscilloscope widget or 0

    KlogicList<DevLibrary> libList;
	DevLibrary *activeLib;           	// active library, or 0
    QAction *devFromLibAction;
    QAction *devANDAction;

    QPrinter   *m_printer;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;

    QAction *fINAction;
    QAction *fOUTAction;

    // Toolbar widgets
    QLabel *libraryChooserLabel;
    QComboBox *libraryChooser;
    QLabel *libraryDeviceChooserLabel;
    QComboBox *libraryDeviceChooser;

    QLabel *simTimeSliderLabel;
    QSlider *simTimeSlider;

    QAction *simStepByStepAction;
    QAction *simAutoAction;

    QAction *openOscilloscopeAction;

    QTimer shortcutWarningTimer;
    bool runtimeShortcutWarning;
};

#endif // _KLOGIC4_H_
