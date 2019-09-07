set(klogic_ICONS
AND.xpm       IEC_XOR.xpm   NET.xpm         pxLED0green.xpm   pxOUT.xpm    pxSSs12.xpm  pxSSs6.xpm     Subnet.xpm
BotBlank.xpm  INV.xpm       OR.xpm          pxLED0red.xpm     pxPOS.xpm    pxSSs13.xpm  pxSSs7.xpm     TopBlank.xpm
BotHuge.xpm   klogicf.xpm   OSC.xpm         pxLED0yellow.xpm  pxRSCMF.xpm  pxSSs14.xpm  pxSSs8.xpm     TopHuge.xpm
BotLarge.xpm  klogic.xpm    pxDM.xpm        pxLED1blue.xpm    pxRSCM.xpm   pxSSs15.xpm  pxSSs9.xpm     TopLarge.xpm
Bot.xpm       MidBlank.xpm  pxD.xpm         pxLED1green.xpm   pxRSC.xpm    pxSSs1.xpm   pxSSsoff.xpm   Top.xpm
cursor.xpm    MidHuge.xpm   pxIN.xpm        pxLED1red.xpm     pxRS.xpm     pxSSs2.xpm   pxSS.xpm       XOR.xpm
IEC_AND.xpm   MidLarge.xpm  pxJKF.xpm       pxLED1yellow.xpm  pxSSs0.xpm   pxSSs3.xpm   pxSwitch0.xpm
IEC_NOT.xpm   Mid.xpm       pxJK.xpm        pxMatrix.xpm      pxSSs10.xpm  pxSSs4.xpm   pxSwitch1.xpm
IEC_OR.xpm    MONOFLOP.xpm  pxLED0blue.xpm  pxNEG.xpm         pxSSs11.xpm  pxSSs5.xpm   pxTFF.xpm
)

install(FILES ${klogic_ICONS} DESTINATION ${KDE4_DATA_INSTALL_DIR}/klogic/deviceIcons)
