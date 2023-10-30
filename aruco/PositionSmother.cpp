#include "PositionSmother.h"

PositionSmother::PositionSmother(float alphaRvec, float alphaTvec, size_t maxHistorySize, size_t maxAbsense)
	: _alphaRvec(alphaRvec), _alphaTvec(alphaTvec), _maxHistorySize(maxHistorySize), _maxAbsense(maxAbsense) {}

void PositionSmother::processMarkers(Markers& markers)
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
		_markersPosition[id]->absenseCounter = 0;
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
