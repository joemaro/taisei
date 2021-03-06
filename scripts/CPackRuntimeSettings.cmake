
 ###############################
#                               #
#  CCC  PPPP    A    CCC  K   K #
# C   C P   P  A A  C   C K  K  #
# C     PPPP  AAAAA C     KKK   #
# C   C P     A   A C   C K  K  #
#  CCC  P     A   A  CCC  K   K #
#                               #
#  SSSS U   U  CCC  K   K  SSSS #
# S     U   U C   C K  K  S     #
#  SSS  U   U C     KKK    SSS  #
#     S U   U C   C K  K      S #
# SSSS   UUU   CCC  K   K SSSS  #
#                               #
 ###############################

set(CLEANUP_COMMANDS "
    DeleteRegKey HKCR \".tsr\"
    DeleteRegKey HKCR \"TaiseiProject.tsr\"
    DeleteRegKey HKCR \"TaiseiProject.tsr\\DefaultIcon\"
    DeleteRegKey HKCR \"TaiseiProject.tsr\\Shell\"
    DeleteRegKey HKCR \"TaiseiProject.tsr\\Shell\\Open\"
    DeleteRegKey HKCR \"TaiseiProject.tsr\\Shell\\Open\\Command\"
    DeleteRegKey HKCR \"Applications\\taisei.exe\"
    DeleteRegKey HKCR \"Applications\\taisei.exe\\DefaultIcon\"
    DeleteRegKey HKCR \"Applications\\taisei.exe\\Shell\"
    DeleteRegKey HKCR \"Applications\\taisei.exe\\Shell\\Open\"
    DeleteRegKey HKCR \"Applications\\taisei.exe\\Shell\\Open\\Command\"
")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    ${CLEANUP_COMMANDS}
    WriteRegStr HKCR \".tsr\" \"\" \"TaiseiProject.tsr\"
    WriteRegStr HKCR \".tsr\" \"Content Type\" \"application/x-taisei-replay\"
    WriteRegStr HKCR \"TaiseiProject.tsr\" \"\" \"Taisei Project replay\"
    WriteRegStr HKCR \"TaiseiProject.tsr\" \"FriendlyTypeName\" \"@$INSTDIR\\taisei.exe,-2\"
    WriteRegStr HKCR \"TaiseiProject.tsr\\DefaultIcon\" \"\" \"$INSTDIR\\taisei.exe,1\"
    WriteRegStr HKCR \"TaiseiProject.tsr\\Shell\\Open\\Command\" \"\" \"$\\\"$INSTDIR\\taisei.exe$\\\" --replay $\\\"%1$\\\"\"
    WriteRegStr HKCR \"Applications\\taisei.exe\" \"\" \"Taisei Project\"
    WriteRegStr HKCR \"Applications\\taisei.exe\" \"FriendlyAppName\" \"@$INSTDIR\\taisei.exe,-1\"
    WriteRegStr HKCR \"Applications\\taisei.exe\\DefaultIcon\" \"\" \"$INSTDIR\\taisei.exe,1\"
    WriteRegStr HKCR \"Applications\\taisei.exe\\Shell\\Open\\Command\" \"\" \"$\\\"$INSTDIR\\taisei.exe$\\\" --replay $\\\"%1$\\\"\"
")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CLEANUP_COMMANDS}")

 ###############################
#                               #
#  CCC  M   M   A   K   K EEEEE #
# C   C MM MM  A A  K  K  E     #
# C     M M M AAAAA KKK   EEEE  #
# C   C M   M A   A K  K  E     #
#  CCC  M   M A   A K   K EEEEE #
#                               #
#  SSSS U   U  CCC  K   K  SSSS #
# S     U   U C   C K  K  S     #
#  SSS  U   U C     KKK    SSS  #
#     S U   U C   C K  K      S #
# SSSS   UUU   CCC  K   K SSSS  #
#                               #
 ###############################
