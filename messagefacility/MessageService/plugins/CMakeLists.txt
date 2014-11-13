# ======================================================================
#
# MessageService/plugins/CMakeLists.txt
#
# ======================================================================

include( BasicPlugin )

basic_plugin( cout   mfplugin MF_MessageLogger )
basic_plugin( cerr   mfplugin MF_MessageLogger )
basic_plugin( file   mfplugin MF_MessageLogger )
basic_plugin( syslog mfplugin MF_MessageLogger )

