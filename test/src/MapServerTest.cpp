// -*- C++ -*-
/*!
 * @file  MapServerTest.cpp
 * @brief Map Server RT component
 * @date $Date$
 *
 * $Id$
 */

#include "MapServerTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* mapserver_spec[] =
  {
    "implementation_id", "MapServerTest",
    "type_name",         "MapServerTest",
    "description",       "Map Server RT component",
    "version",           "1.1.0",
    "vendor",            "Sugar Sweet Robotics",
    "category",          "Navigation",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.map_file_name", "map_file_name",

    // Widget
    "conf.__widget__.map_file_name", "text",
    // Constraints

    "conf.__type__.map_file_name", "string",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
MapServerTest::MapServerTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_mapServerPort("mapServer")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
MapServerTest::~MapServerTest()
{
}



RTC::ReturnCode_t MapServerTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers

  // Set OutPort buffer

  // Set service provider to Ports

  // Set service consumers to Ports
  m_mapServerPort.registerConsumer("NAVIGATION_OccupancyGridMapServer", "NAVIGATION::OccupancyGridMapServer", m_NAVIGATION_OccupancyGridMapServer);

  // Set CORBA Service Ports
  addPort(m_mapServerPort);

  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("map_file_name", m_map_file_name, "map_file_name");
  // </rtc-template>

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t MapServerTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t MapServerTest::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MapServerTest::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t MapServerTest::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServerTest::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{

  void MapServerTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(mapserver_spec);
    manager->registerFactory(profile,
                             RTC::Create<MapServerTest>,
                             RTC::Delete<MapServerTest>);
  }

};


