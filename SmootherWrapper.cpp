#include "SmootherWrapper.h"

void SmootherWrapper::addValues(const cv::Vec3d& newRvec, const cv::Vec3d& newTvec) {

	rvecSmoother.addValueAndAverage(newRvec);
	tvecSmoother.addValueAndAverage(newTvec);
}

cv::Vec3d SmootherWrapper::getSmoothedRvec() const {

	return rvecSmoother.getSmoothedValue();
}

cv::Vec3d SmootherWrapper::getSmoothedTvec() const {

	return tvecSmoother.getSmoothedValue();
}

void SmootherWrapper::resetAbsense() {

	absenseCounter = 0;
}

void SmootherWrapper::increaseAbsense() {

	absenseCounter++;
}

bool SmootherWrapper::shouldRemove() const {

	return absenseCounter >= max_absense;
}