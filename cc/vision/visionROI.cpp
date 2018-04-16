#include "visionROI.hpp"

bool VisionROI::checkForBoundaries() {

	if (mPosCenter.x < 0 || mPosCenter.y < 0 || mPosCenter.x > mWidth || mPosCenter.y > mHeight) {
		mPosCenter.x = mWidth / 2;
		mPosCenter.y = mHeight / 2;
		mPosBegin = cv::Point(mPosCenter.x - mSize / 2, mPosCenter.y - mSize / 2);
		mPosEnd = cv::Point(mPosCenter.x + mSize / 2, mPosCenter.y + mSize / 2);
		return false;
	}

	mPosBegin.x = mPosCenter.x - mSize / 2;
	mPosBegin.y = mPosCenter.y - mSize / 2;
	mPosEnd.x = mPosCenter.x + mSize / 2;
	mPosEnd.y = mPosCenter.y + mSize / 2;

	if (mPosBegin.x < 0) {
		mPosBegin.x = 0;
		mPosCenter.x = mSize / 2;
		mPosEnd.x = mSize;
	}
	if (mPosBegin.y < 0) {
		mPosBegin.y = 0;
		mPosCenter.y = mSize / 2;
		mPosEnd.y = mSize;
	}
	if (mPosEnd.x > mWidth - 1) {
		mPosCenter.x = mWidth - 1 - mSize / 2;
		mPosBegin.x = mWidth - 1 - mSize;
		mPosEnd.x = mWidth - 1;
	}
	if (mPosEnd.y > mHeight - 1) {
		mPosBegin.y = mHeight - 1 - mSize;
		mPosCenter.y = mHeight - 1 - mSize / 2;
		mPosEnd.y = mHeight - 1;
	}

	if (abs(mPosBegin.x - mPosEnd.x) <= 0 || abs(mPosBegin.y - mPosEnd.y) <= 0) {
		std::cout << "VisionROI::checkForBoundaries: window " << mId
				  << " has no valid area. (abs(mPosBegin.x - mPosEnd.x) <= 0 || abs(mPosBegin.y - mPosEnd.y) <= 0)"
				  << std::endl;
		return false;
	}

	return true;
}

void VisionROI::setPosition(int x, int y) {
	mPosBegin.x = x;
	mPosBegin.y = y;
	mPosCenter = cv::Point(mPosBegin.x + mSize / 2, mPosBegin.y + mSize / 2);
	checkForBoundaries();
}

void VisionROI::setCenter(cv::Point pos) {
	mPosCenter.x = pos.x;
	mPosCenter.y = pos.y;
	checkForBoundaries();
}

void VisionROI::setIsLost(bool lost) {
	mIsLost = lost;
}

int VisionROI::getSize() {
	return mSize;
}

cv::Point VisionROI::getEnd() {
	return mPosEnd;
}

cv::Point VisionROI::getPosition() {
	return mPosBegin;
}

bool VisionROI::getIsLost() {
	return mIsLost;
}

int VisionROI::getX() {
	return mPosBegin.x;
}

int VisionROI::getY() {
	return mPosBegin.y;
}

VisionROI::VisionROI(int width, int height, int size, int id) :
		mIsLost(true), mPosBegin(cv::Point(0, 0)), mSize(size), mId(id),
		mWidth(width), mHeight(height) {
	mPosCenter = cv::Point(mPosBegin.x + mSize / 2, mPosBegin.y + mSize / 2);
	checkForBoundaries();
}

VisionROI::~VisionROI() = default;
