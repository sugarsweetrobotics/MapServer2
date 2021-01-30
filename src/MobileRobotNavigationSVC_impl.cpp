// -*-C++-*-
/*!
 * @file  MobileRobotNavigationSVC_impl.cpp
 * @brief Service implementation code of MobileRobotNavigation.idl
 *
 */

#include "MobileRobotNavigationSVC_impl.h"
#include "MapServer.h"
/*
 * Example implementational code for IDL interface NAVIGATION::OccupancyGridMapServer
 */
NAVIGATION_OccupancyGridMapServerSVC_impl::NAVIGATION_OccupancyGridMapServerSVC_impl()
{
  // Please add extra constructor code here.
}


NAVIGATION_OccupancyGridMapServerSVC_impl::~NAVIGATION_OccupancyGridMapServerSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
NAVIGATION::MAP_RETURN_STATUS NAVIGATION_OccupancyGridMapServerSVC_impl::requestLocalMap(const NAVIGATION::OccupancyGridMapRequestParam& param, NAVIGATION::OccupancyGridMap_out map)
{
  std::cout << "requestLocalMap" << std::endl;
  NAVIGATION::OccupancyGridMap_var retmap(new NAVIGATION::OccupancyGridMap());
  auto& img = m_pRTC->m_mapImg; // cv::Mat
  auto& cnf = m_pRTC->m_mapConfig;

  auto retMapCnf = cnf;
  cv::Mat retMap;

  if (param.sizeOfMap.w <= 0 || param.sizeOfMap.l <= 0) {
    img.copyTo(retMap);
  } else {
    // if (param.sizeOfMap.w > 0 || param.sizeOfMap.l > 0) {
    
    int retMapWidthPx = param.sizeOfMap.w / param.sizeOfGrid.w;
    int retMapHeightPx = param.sizeOfMap.l / param.sizeOfGrid.l;
    
    int originXOfWholeMapPx = -cnf.config.origin_x / cnf.config.xScale;
    int originYOfWholeMapPx = +cnf.config.origin_y / cnf.config.yScale;
    int retMapCenterXOnWholeMapPx = param.globalPositionOfCenter.position.x / cnf.config.xScale + originXOfWholeMapPx;
    int retMapCenterYOnWholeMapPx = -param.globalPositionOfCenter.position.y / cnf.config.yScale + originYOfWholeMapPx;
    int retMapWidthOnWholeMapPx = param.sizeOfMap.w / cnf.config.xScale;
    int retMapHeightOnWholeMapPx = param.sizeOfMap.l / cnf.config.yScale;
    auto tmpMap = cv::Mat(img, cv::Rect((retMapCenterXOnWholeMapPx - retMapWidthOnWholeMapPx/2),
					(retMapCenterYOnWholeMapPx - retMapHeightOnWholeMapPx/2),
					retMapWidthOnWholeMapPx, retMapHeightOnWholeMapPx));
    
    tmpMap.copyTo(retMap);
    std::cout << "rect:" << (retMapCenterXOnWholeMapPx - retMapWidthOnWholeMapPx/2)
	      << ", " << (retMapCenterYOnWholeMapPx - retMapHeightOnWholeMapPx/2)
	      << ", " << retMapWidthOnWholeMapPx << ", " <<  retMapHeightOnWholeMapPx
	      << std::endl;
    retMapCnf.config.origin_x = (param.globalPositionOfCenter.position.x - param.sizeOfMap.w/2);
    retMapCnf.config.origin_y = (param.globalPositionOfCenter.position.y - param.sizeOfMap.l/2);
  }
  
  retmap->config.globalPositionOfTopLeft.position.x = retMapCnf.config.origin_x;
  retmap->config.globalPositionOfTopLeft.position.y = retMapCnf.config.origin_y;
  retmap->config.globalPositionOfTopLeft.heading = 0;
  retmap->config.sizeOfGrid.l = retMapCnf.config.yScale;
  retmap->config.sizeOfGrid.w = retMapCnf.config.xScale;
  retmap->config.sizeOfMap.l = retMapCnf.config.yScale * retMap.rows;
  retmap->config.sizeOfMap.w = retMapCnf.config.xScale * retMap.cols;
  /*
  retmap->config.globalPositionOfTopLeft.position.x = retMapCnf.config.origin_x;
  retmap->config.globalPositionOfTopLeft.position.y = retMapCnf.config.origin_y;
  retmap->config.globalPositionOfTopLeft.heading = 0;
  retmap->config.sizeOfGrid.l = retMapCnf.config.yScale;
  retmap->config.sizeOfGrid.w = retMapCnf.config.xScale;
  retmap->config.sizeOfMap.l = retMapCnf.config.yScale * img.rows;
  retmap->config.sizeOfMap.w = retMapCnf.config.xScale * img.cols;
  */
  std::cout << "map(" << retMap.cols << " x " << retMap.rows << ")" << std::endl;
  std::cout << "config.sizeOfMap.l: " << retmap->config.sizeOfMap.l << std::endl;
  std::cout << "config.sizeOfMap.w: " << retmap->config.sizeOfMap.w << std::endl;
  std::cout << "config.sizeOfGrid.l: " << retmap->config.sizeOfGrid.l << std::endl;
  std::cout << "config.sizeOfGrid.w: " << retmap->config.sizeOfGrid.w << std::endl;
  retmap->cells.length(retMap.cols * retMap.rows);
  for(int h = 0;h < retMap.rows;h++) {
    for(int w = 0;w < retMap.cols;w++) {
      retmap->cells[h * retMap.cols + w] = retMap.data[h * retMap.cols + w];
    }
  }
  map = retmap._retn();
  
  /*
  retmap->config.globalPositionOfTopLeft.position.x = 0;
  retmap->config.globalPositionOfTopLeft.position.y = 0;
  retmap->config.globalPositionOfTopLeft.heading = 0;
  retmap->config.sizeOfGrid.l = 0.5; // height
  retmap->config.sizeOfGrid.w = 0.5;
  retmap->config.sizeOfMap.l = 240*0.5; // height
  retmap->config.sizeOfMap.w = 320*0.5;

  retmap->cells.length(320*240);
  for(int h = 0;h < 240;h++) {
    for(int w = 0;w < 320;w++) {
      if ( ((h/20) % 2 == 0 && (w/20) % 2 == 0) ||
	   ((h/20) % 2 == 1 && (w/20) % 2 == 1)) {
	retmap->cells[h*240 + w] = 255;
      } else {
	retmap->cells[h*240 + w] = 0;
      }
    }
  }
  map = retmap._retn();
  */
  return NAVIGATION::MAP_RETVAL_OK;
}



// End of example implementational code

/*
 * Example implementational code for IDL interface NAVIGATION::MonteCarloLocalization
 */
NAVIGATION_MonteCarloLocalizationSVC_impl::NAVIGATION_MonteCarloLocalizationSVC_impl()
{
  // Please add extra constructor code here.
}


NAVIGATION_MonteCarloLocalizationSVC_impl::~NAVIGATION_MonteCarloLocalizationSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
NAVIGATION::MCL_RETURN_STATUS NAVIGATION_MonteCarloLocalizationSVC_impl::resetParticles(const NAVIGATION::ParticleResetParam& param)
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <NAVIGATION::MCL_RETURN_STATUS NAVIGATION_MonteCarloLocalizationSVC_impl::resetParticles(const NAVIGATION::ParticleResetParam& param)>"
#endif
  return NAVIGATION::MCL_RETVAL_NOT_IMPL;  
}

NAVIGATION::MCL_RETURN_STATUS NAVIGATION_MonteCarloLocalizationSVC_impl::requestParticles(NAVIGATION::MCLInfo_out particles)
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <NAVIGATION::MCL_RETURN_STATUS NAVIGATION_MonteCarloLocalizationSVC_impl::requestParticles(NAVIGATION::MCLInfo_out particles)>"
#endif
  return NAVIGATION::MCL_RETVAL_NOT_IMPL;    
}

NAVIGATION::MCL_RETURN_STATUS NAVIGATION_MonteCarloLocalizationSVC_impl::setParticles(const NAVIGATION::MCLInfo& particles)
{
  // Please insert your code here and remove the following warning pragma
#ifndef WIN32
  #warning "Code missing in function <NAVIGATION::MCL_RETURN_STATUS NAVIGATION_MonteCarloLocalizationSVC_impl::setParticles(const NAVIGATION::MCLInfo& particles)>"
#endif
  return NAVIGATION::MCL_RETVAL_NOT_IMPL;      
}



// End of example implementational code



