// -*-C++-*-
/*!
 * @file  MapServerSVC_impl.cpp
 * @brief Service implementation code of MapServer.idl
 *
 */

#include <iostream>

#include "MapServerSVC_impl.h"
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
NAVIGATION::MAP_RETURN_STATUS NAVIGATION_OccupancyGridMapServerSVC_impl::updateLocalMap(const NAVIGATION::OccupancyGridMapRequestParam& param, const NAVIGATION::OccupancyGridMap& map)
{
  // Please insert your code here and remove the following warning pragma

  return NAVIGATION::MAP_NG_NOTIMPL;
}

NAVIGATION::MAP_RETURN_STATUS NAVIGATION_OccupancyGridMapServerSVC_impl::requestLocalMap(const NAVIGATION::OccupancyGridMapRequestParam& param, NAVIGATION::OccupancyGridMap_out map)
{
  std::cout << "[MapServer] NAVIGATION_OccupancyGridMapServerSVC_impl::requestLocalMap called." << std::endl;
  NAVIGATION::OccupancyGridMap_var retmap(new NAVIGATION::OccupancyGridMap());
  auto& img = m_pRTC->m_mapImg; // cv::Mat
  auto& cnf = m_pRTC->m_mapConfig;

  auto retMapCnf = cnf;
  cv::Mat retMap;
  if (param.sizeOfMap.width <= 0 || param.sizeOfMap.height <= 0) {
    img.copyTo(retMap);
  } else {
    // if (param.sizeOfMap.w > 0 || param.sizeOfMap.l > 0) {
    
    int retMapWidthPx = param.sizeOfMap.width / param.sizeOfGrid.width;
    int retMapHeightPx = param.sizeOfMap.height / param.sizeOfGrid.height;
    std::cout << "[MapServer] Requested Map grid size is " << retMapWidthPx << "x" << retMapHeightPx << std::endl;    
    int originXOfWholeMapPx = -cnf.config.origin_x / cnf.config.xScale;
    int originYOfWholeMapPx = +cnf.config.origin_y / cnf.config.yScale;
    std::cout << "[MapServer] Requested Map topLeft position of whole map is " << originXOfWholeMapPx << ", " << originYOfWholeMapPx << std::endl;
    int retMapCenterXOnWholeMapPx = param.globalPositionOfCenter.position.x / cnf.config.xScale + originXOfWholeMapPx;
    int retMapCenterYOnWholeMapPx = -param.globalPositionOfCenter.position.y / cnf.config.yScale + originYOfWholeMapPx;
    std::cout << "[MapServer] Requested Map center is " << retMapCenterXOnWholeMapPx << ", " << retMapCenterYOnWholeMapPx << std::endl;
    int retMapWidthOnWholeMapPx = param.sizeOfMap.width / cnf.config.xScale;
    int retMapHeightOnWholeMapPx = param.sizeOfMap.height / cnf.config.yScale;
    std::cout << "[MapServer] Requested Map size is " << retMapWidthOnWholeMapPx << "x" << retMapHeightOnWholeMapPx << std::endl;

    auto tmpMap = cv::Mat(img, cv::Rect((retMapCenterXOnWholeMapPx - retMapWidthOnWholeMapPx/2),
					(retMapCenterYOnWholeMapPx - retMapHeightOnWholeMapPx/2),
					retMapWidthOnWholeMapPx, retMapHeightOnWholeMapPx));
    cv::imwrite("tmpmap.png", tmpMap);
    
    tmpMap.copyTo(retMap);
    std::cout << "[MapServer] rect:" << (retMapCenterXOnWholeMapPx - retMapWidthOnWholeMapPx/2)
	      << ", " << (retMapCenterYOnWholeMapPx - retMapHeightOnWholeMapPx/2)
	      << ", " << retMapWidthOnWholeMapPx << ", " <<  retMapHeightOnWholeMapPx
	      << std::endl;
    retMapCnf.config.origin_x = (param.globalPositionOfCenter.position.x - param.sizeOfMap.width/2);
    retMapCnf.config.origin_y = (param.globalPositionOfCenter.position.y - param.sizeOfMap.height/2);
  }
  
  retmap->config.globalPositionOfTopLeft.position.x = retMapCnf.config.origin_x;
  retmap->config.globalPositionOfTopLeft.position.y = retMapCnf.config.origin_y;
  retmap->config.globalPositionOfTopLeft.heading = 0;
  retmap->config.sizeOfGrid.height = retMapCnf.config.yScale;
  retmap->config.sizeOfGrid.width = retMapCnf.config.xScale;
  retmap->config.sizeOfGridMap.height = retMap.rows;
  retmap->config.sizeOfGridMap.width = retMap.cols;
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
  std::cout << "config.sizeOfMap.height: " << retmap->config.sizeOfGridMap.height << std::endl;
  std::cout << "config.sizeOfMap.width: " << retmap->config.sizeOfGridMap.width << std::endl;
  std::cout << "config.sizeOfGrid.height: " << retmap->config.sizeOfGrid.height << std::endl;
  std::cout << "config.sizeOfGrid.width: " << retmap->config.sizeOfGrid.width << std::endl;
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
  return NAVIGATION::MAP_OK;
}

NAVIGATION::MAP_RETURN_STATUS NAVIGATION_OccupancyGridMapServerSVC_impl::getWholeMapConig(NAVIGATION::OccupancyGridMapConfig& config)
{

  return NAVIGATION::MAP_NG_NOTIMPL;
}



// End of example implementational code

/*
 * Example implementational code for IDL interface NAVIGATION::OccupancyGridMapClient
 */
NAVIGATION_OccupancyGridMapClientSVC_impl::NAVIGATION_OccupancyGridMapClientSVC_impl()
{
  // Please add extra constructor code here.
}


NAVIGATION_OccupancyGridMapClientSVC_impl::~NAVIGATION_OccupancyGridMapClientSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
NAVIGATION::MAP_RETURN_STATUS NAVIGATION_OccupancyGridMapClientSVC_impl::notifyUpdateMap(const NAVIGATION::OccupancyGridMapConfig& config)
{
  return NAVIGATION::MAP_NG_NOTIMPL;
}



// End of example implementational code



