#pragma once

#include "Smoother.h"
#include "Markers.h"
struct rectPosition
{
	Smoother rvecSmoother;
	Smoother tvecSmoother;
	std::vector<cv::Point2f> corners = std::vector<cv::Point2f>(4);
	int absenseCounter = 0;
};

class PositionSmother
{
public:
	PositionSmother(float alphaRvec, float alphaTvec, size_t maxHistorySize, size_t maxAbsense);
	
	void processMarkers(Markers& markers);

private:
	// Constants to config
	float _alphaRvec = 0.5f;
	float _alphaTvec = 0.5f;
	size_t _maxHistorySize = 10;
	size_t _maxAbsense = 2;

	std::unordered_map<int, std::shared_ptr<rectPosition>> _markersPosition;
};

