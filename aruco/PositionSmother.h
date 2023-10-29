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
	PositionSmother(float alphaRvec, float alphaTvec, size_t maxHistorySize, size_t maxAbsense)
		: _alphaRvec(alphaRvec), _alphaTvec(alphaTvec), _maxHistorySize(maxHistorySize), _maxAbsense(maxAbsense) {}
	
	void processMarkers(Markers& markers)
	{
		// increace absenseCounter for all cashed markers
		for (auto& pair : _markersPosition) {
			pair.second->absenseCounter++;
		}

		// update a set of markers
		for (int index{ 0 }; index < markers.ids.size(); index++) {
			int id = markers.ids.at(index);
			if (_markersPosition.find(id) == _markersPosition.end()) {
				_markersPosition[id] = std::make_shared<rectPosition>(Smoother(_alphaRvec, _maxHistorySize), Smoother(_alphaTvec, _maxHistorySize));
			}
			_markersPosition[id]->corners = markers.corners.at(index);
			_markersPosition[id]->rvecSmoother.addValueAndAverage(markers.rvecs.at(index));
			_markersPosition[id]->tvecSmoother.addValueAndAverage(markers.tvecs.at(index));
			_markersPosition[id]->absenseCounter=0;
		}

		// erase old markers
		for (auto it = _markersPosition.begin(); it != _markersPosition.end(); ) {
			if (it->second->absenseCounter > _maxAbsense) {
				it = _markersPosition.erase(it);
			}
			else {
				++it;
			}
		}

		// clear markers, rejected markers dont touch
		size_t  nMarkers = _markersPosition.size();

		markers.ids.clear();
		markers.rvecs.clear();
		markers.tvecs.clear();
		markers.corners.clear();
		markers.viewMatrixes.resize(nMarkers);

		for (auto& marker : _markersPosition) {
			markers.ids.push_back(marker.first);
			markers.rvecs.push_back(marker.second->rvecSmoother.getSmoothedValue());
			markers.tvecs.push_back(marker.second->tvecSmoother.getSmoothedValue());
			markers.corners.push_back(marker.second->corners);
		}
	}

private:
	// Constants to config
	float _alphaRvec = 0.5f;
	float _alphaTvec = 0.5f;
	size_t _maxHistorySize = 10;
	size_t _maxAbsense = 2;

	std::unordered_map<int, std::shared_ptr<rectPosition>> _markersPosition;
};

