!include "MUI.nsh"


Name "Wadoku Notify"
OutFile "wadoku_notify_installer.exe"
InstallDir $PROGRAMFILES\WadokuNotify

!define MUI_HEADERIMAGE 
!define MUI_HEADERIMAGE_BITMAP "data\wadoku.png"  
!define MUI_ICON "data\wadoku.ico"

;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES
  
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Japanese"


  !insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES


;!insertmacro MUI_LANGUAGE "German"

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

Section "Install"
 SetOutPath $INSTDIR
 File "bin\wadoku.exe"
 File "data\wadoku.sqlite3"
 File "data\wadoku.png"
 ;CreateDirectory "$INSTDIR\recources"
 ;CopyFiles "$EXEDIR\recources\*" "$INSTDIR\recources\" 1000
 WriteUninstaller "uninstall.exe"
 CreateDirectory "$SMPROGRAMS\WadokuNotify"
 CreateShortCut "$SMPROGRAMS\WadokuNotify\WadokuNotify.lnk" "$INSTDIR\wadoku.exe" "" "$INSTDIR\wadoku.exe" 0
 CreateShortCut "$SMPROGRAMS\WadokuNotify\WadokuNotify Uninstaller.lnk" "$INSTDIR\uninstall.exe" "" "$SMPROGRAMS\WadokuNotify\WadokuNotify.lnk" 0
 ; write uninstall strings
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BigNSISTest" "DisplayName" "WadokuNotify (remove only)"
 WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BigNSISTest" "UninstallString" '"$INSTDIR\uninstall.exe"'
 ;working directory not working with regkey WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "WadokuNotify" "$INSTDIR\wadoku.exe"
 CreateShortCut "$SMSTARTUP\WadokuNotify.lnk" "$INSTDIR\wadoku.exe" 
SectionEnd


Section "Uninstall"
 Delete "$INSTDIR\wadoku.exe"
 RMDir /r "$INSTDIR"
 Delete "$SMPROGRAMS\WadokuNotify\WadokuNotify.lnk"
 Delete "$SMPROGRAMS\WadokuNotify\uninstall.lnk"
 Delete "$SMSTARTUP\WadokuNotify.lnk"
 RMDir "$SMPROGRAMS\WadokuNotify"
 ;DeleteRegValue HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "WadokuNotify"
SectionEnd

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd
