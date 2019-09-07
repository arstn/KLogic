set(klogic_SRCS
calcRules.cpp      deviceDFF.cpp      devLib.cpp      imageCache.cpp    propDev.cpp     ramw.cpp          xmlExport.cpp
calculator.cpp     deviceFactory.cpp  dlgEqu.cpp      InverterChooser.cpp propGlob.cpp   selection.cpp    xmlField.cpp
circuit.cpp        deviceJKFF.cpp     dlgLib.cpp      klogic4.cpp       propName.cpp    OscWidget.cpp     xmlImport.cpp
CircuitWidget.cpp  deviceRAM.cpp      dlgPattern.cpp  KlogicGlobal.cpp  propOsz.cpp     symbolicCalc.cpp  xmlSymbolMap.cpp
ClientWidget.cpp   deviceRSFF.cpp     fifo.cpp        main.cpp          propPwr.cpp     tool.cpp          xobject.cpp
deviceColored.cpp  deviceSimple.cpp   FileIO.cpp      oldHandler.cpp    propSwitch.cpp  wire.cpp          xwire.cpp
device.cpp         deviceTFF.cpp      grid.cpp        Operator.cpp      propText.cpp    xdevice.cpp       ScrollableCircuitWidget.cpp
ValueChooser.cpp   OscContentWidget.cpp OscChannel.cpp Simulator.cpp    SimulatorThread.cpp
)

kde4_add_ui_files(klogic_SRCS NetWidget.ui)

kde4_add_kcfg_files(klogic_SRCS settings.kcfgc )

kde4_add_executable(klogic ${klogic_SRCS})

target_link_libraries(klogic ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS klogic ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS klogic.desktop  DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR} )
install( FILES klogic.kcfg  DESTINATION  ${KDE4_KCFG_INSTALL_DIR} )
install( FILES klogic4ui.rc  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/klogic )
