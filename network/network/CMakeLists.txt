set(HAVE_SLP ${SLP_FOUND})
configure_file( config-slp.h.cmake  ${CMAKE_CURRENT_BINARY_DIR}/config-slp.h )

include_directories(
  builder
)

set( netsystemdriver_SRCS
  builder/simpleitemfactory.cpp
)

# TODO: find best-practice to do such dependant inclusion of files to compile, this is unreadable
if(SLP_FOUND)
set( slpnetworkbuilder_SRCS
  builder/slp/slpservicebrowser.cpp
  builder/slp/slpnetworkbuilder.cpp
)
endif()

set( dnssdnetworkbuilder_SRCS
  builder/dnssd/dnssdnetworkbuilder.cpp
)

set( upnpnetworkbuilder_SRCS
  builder/upnp/cagibidevice.cpp
  builder/upnp/cagibidbuscodec.cpp
  builder/upnp/upnpnetworkbuilder.cpp
)

set( networkbuilder_SRCS
  builder/abstractnetworkbuilder.cpp
  builder/abstractnetsystemfactory.cpp
)

set( networkdbus_LIB_SRCS
  networkdbus.cpp
)

set( molletnetwork5_LIB_SRCS
  ${netsystemdriver_SRCS}
#   ${slpnetworkbuilder_SRCS}
  ${upnpnetworkbuilder_SRCS}
  ${dnssdnetworkbuilder_SRCS}
  ${networkbuilder_SRCS}
  ${networkdbus_LIB_SRCS}
  network_p.cpp
  network.cpp
  netdevice_p.cpp
  netdevice.cpp
  netservice_p.cpp
  netservice.cpp
)

add_library( molletnetwork5  SHARED ${molletnetwork5_LIB_SRCS} )
target_link_libraries( molletnetwork5
    PUBLIC
        KF5::DNSSD
        Qt5::Network
        Qt5::Gui
        KF5::KDELibs4Support # KSharedPtr
)
if(SLP_FOUND)
    target_link_libraries( molletnetwork5  PRIVATE ${SLP_LIBRARIES} )
endif()
set_target_properties( molletnetwork5 PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR} )

install( TARGETS molletnetwork5 ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP )
