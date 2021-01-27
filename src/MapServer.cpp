// -*- C++ -*-
/*!
 * @file  MapServer.cpp
 * @brief Map Server RT component
 * @date $Date$
 *
 * $Id$
 */

#include "MapServer.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

// Module specification
// <rtc-template block="module_spec">
static const char* mapserver_spec[] =
  {
    "implementation_id", "MapServer",
    "type_name",         "MapServer",
    "description",       "Map Server RT component",
    "version",           "1.0.0",
    "vendor",            "Sugar Sweet Robotics",
    "category",          "Navigation",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.map_file_name", "../success_map",

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
MapServer::MapServer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_mapServerPort("mapServer")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
MapServer::~MapServer()
{
}



RTC::ReturnCode_t MapServer::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers

  // Set OutPort buffer

  // Set service provider to Ports
  m_mapServerPort.registerProvider("NAVIGATION_OccupancyGridMapServer", "NAVIGATION::OccupancyGridMapServer", m_NAVIGATION_OccupancyGridMapServer);

  // Set service consumers to Ports

  // Set CORBA Service Ports
  addPort(m_mapServerPort);

  m_NAVIGATION_OccupancyGridMapServer.setRTC(this);
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("map_file_name", m_map_file_name, "map_file_name");
  // </rtc-template>

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t MapServer::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


namespace {
  inline std::vector<std::string> stringSplit(const std::string& str, const char sep=' ') {
    std::vector<std::string> result;
    int i = 0;
    do {
      const int begin = i;
      ///const char *begin = pstr;
      while(str[i] != sep && str[i] != 0)
	i++;
      
      result.push_back(str.substr(begin, i-begin));//std::string(begin, pstr));
    } while (0 != str[i++]);
    return result;
  }

  inline void ltrim_(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
      return !std::isspace(ch);
    }));
  }
  
  inline void rtrim_(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
      return !std::isspace(ch);
    }).base(), s.end());
  }
  
  // trim from both ends (in place)
  inline void trim_(std::string &s) {
    ltrim_(s);
    rtrim_(s);
  }
  
// trim from start (copying)
  inline std::string ltrim(std::string s) {
    ltrim_(s);
    return s;
  }

// trim from end (copying)
  inline std::string rtrim(std::string s) {
    rtrim_(s);
    return s;
  }

  // trim from both ends (copying)
  inline std::string trim(std::string s) {
    trim_(s);
    return s;
  }
  
}

RTC::ReturnCode_t MapServer::onActivated(RTC::UniqueId ec_id)
{
  
  auto img = cv::imread( m_map_file_name + ".png", 1);
  cv::imwrite("hoge.png", img);
  //if (!m_mapImg.data ) {
  //RTC_ERROR(("Reading Map Data (filename = %s) failed.", m_map_file_name.c_str()));
  //return RTC::RTC_ERROR;
  //}

  auto typ = img.type();
  auto depth = typ & CV_MAT_DEPTH_MASK;
  auto chans = 1 + (typ >> CV_CN_SHIFT);
  if (chans != 1) {
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    m_mapImg = gray;
  } else {
    m_mapImg = img;
  }

  std::ifstream fin(m_map_file_name + ".yaml");
  if (fin.fail()) {
    std::cout << "[MapServer] Error reading yaml file" << std::endl;
    return RTC::RTC_ERROR;
  }
  std::string line;
  int lineNumber = 0;
  bool configContext = false;
  while (std::getline(fin, line)) {
    //    std::cout << "line is " << line << std::endl;
    lineNumber++;
    if (line[0] == '#') continue; // comment line
    
    if (configContext && (line[0] != ' ' && line[0] != '\t')) {
      //std::cout << "Now config exit context" << std::endl;      
      configContext = false;
    }

    auto tokens = stringSplit(line, ':');
    auto key = trim(tokens[0]);
    auto value = trim(tokens[1]);
    //    std::cout << "key is   '" << key << "'" << std::endl;
    //    std::cout << "value is '" << value << "'" << std::endl;
    if (!configContext) {
      if (key == "image") {
	m_mapConfig.image = value;
	continue;
      } else if (key == "config") {
	configContext = true;
	//std::cout << "Now config context" << std::endl;
	continue;
      } else {
	std::cout << "E1:Invalid Config File (filename is " << m_map_file_name + ".yaml)" << std::endl;
	std::cout << "Line "<< lineNumber << " detect invalid key : " << key << std::endl;
	return RTC::RTC_ERROR;
      }
      continue;
    } 

    //    std::cout << "config: " << std::endl;
    if (key == "xScale") {
      m_mapConfig.config.xScale = atof(value.c_str());
    } else if (key == "yScale") {
      m_mapConfig.config.yScale = atof(value.c_str());
    } else if (key == "origin_x") {
      m_mapConfig.config.origin_x = atof(value.c_str());
    } else if (key == "origin_y") {
      m_mapConfig.config.origin_y = atof(value.c_str());
    } else if (key == "origin_th") {
      m_mapConfig.config.origin_th = atof(value.c_str());
    } else if (key == "row") {
      m_mapConfig.config.row = atoi(value.c_str());
    } else if (key == "column") {
      m_mapConfig.config.column = atoi(value.c_str());
    } else {
      std::cout << "E2: Invalid Config File (filename is " << m_map_file_name + ".yaml)" << std::endl;
      std::cout << "Line "<< lineNumber << " detect invalid key : " << key << std::endl;
      return RTC::RTC_ERROR;
    }
  }
  return RTC::RTC_OK;
}


RTC::ReturnCode_t MapServer::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t MapServer::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t MapServer::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{

  void MapServerInit(RTC::Manager* manager)
  {
    coil::Properties profile(mapserver_spec);
    manager->registerFactory(profile,
                             RTC::Create<MapServer>,
                             RTC::Delete<MapServer>);
  }

};


