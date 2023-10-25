#pragma once

#include "Smoother.h"

class SmootherWrapper
{
public:
	SmootherWrapper(double alphaRvec, double alphaTvec, size_t maxHistorySize) : rvecSmoother(alphaRvec, maxHistorySize), tvecSmoother(alphaTvec, maxHistorySize) {}
	
	void addValues(const cv::Vec3d& newRvec, const cv::Vec3d& newTvec);
	cv::Vec3d getSmoothedRvec() const;
	cv::Vec3d getSmoothedTvec() const;
	void resetAbsense();
	void increaseAbsense();
	bool shouldRemove() const;

private:
	Smoother rvecSmoother;
	Smoother tvecSmoother;
	int absenseCounter = 0;
	static const int max_absense = 2;
};

