#include "visionROI.hpp"

bool VisionROI::checkForBoundaries() {

  if (mPosCenter.x < 0 || mPosCenter.y < 0 || mPosCenter.x > mWidth || mPosCenter.y > mHeight) {
    // std::cout << "VisionROI::checkForBoundaries: window " << mId << ": " << mPosCenter.x << ", " << mPosCenter.y << std::endl;
    mPosCenter.x = mWidth/2;
    mPosCenter.y = mHeight/2;
    mPosBegin = cv::Point(mPosCenter.x-mSize/2, mPosCenter.y-mSize/2);
    mPosEnd = cv::Point(mPosCenter.x+mSize/2, mPosCenter.y+mSize/2);
    // std::cout << "VisionROI::checkForBoundaries: CORRECTED window " << mId << ": " << mPosCenter.x << ", " << mPosCenter.y << std::endl;
    return false;
  }

  mPosBegin.x = mPosCenter.x - mSize/2;
  mPosBegin.y = mPosCenter.y - mSize/2;
  mPosEnd.x = mPosCenter.x + mSize/2;
  mPosEnd.y = mPosCenter.y + mSize/2;

  if (mPosBegin.x < 0) {
    mPosBegin.x = 0;
    mPosCenter.x = mSize/2;
    mPosEnd.x = mSize;
  }
  if (mPosBegin.y < 0) {
    mPosBegin.y = 0;
    mPosCenter.y = mSize/2;
    mPosEnd.y = mSize;
  }
  if (mPosEnd.x > mWidth-1) {
    mPosCenter.x = mWidth-1 - mSize/2;
    mPosBegin.x = mWidth-1 - mSize;
    mPosEnd.x = mWidth-1;
  }
  if (mPosEnd.y > mHeight-1) {
    mPosBegin.y = mHeight-1 - mSize;
    mPosCenter.y = mHeight-1 - mSize/2;
    mPosEnd.y = mHeight-1;
  }

  if (abs(mPosBegin.x - mPosEnd.x) <= 0 || abs(mPosBegin.y - mPosEnd.y) <= 0) {
    std::cout << "VisionROI::checkForBoundaries: window " << mId << " has no valid area. (abs(mPosBegin.x - mPosEnd.x) <= 0 || abs(mPosBegin.y - mPosEnd.y) <= 0)" << std::endl;
    return false;
  }

  return true;
}

void VisionROI::setSize(cv::Size size) {
  mSize = size.width;
  mSize = size.height;
  mPosCenter = cv::Point(mPosBegin.x+mSize/2, mPosBegin.y+mSize/2);
  checkForBoundaries();
}

void VisionROI::setSize(int width, int height) {
  mSize = width;
  mSize = height;
  mPosCenter = cv::Point(mPosBegin.x+mSize/2, mPosBegin.y+mSize/2);
  checkForBoundaries();
}

void VisionROI::setId(int id) {
  mId = id;
}

void VisionROI::setPosition(cv::Point pos) {
  mPosBegin.x = pos.x;
  mPosBegin.y = pos.y;
  mPosCenter = cv::Point(mPosBegin.x+mSize/2, mPosBegin.y+mSize/2);
  checkForBoundaries();
}

void VisionROI::setPosition(int x, int y) {
  mPosBegin.x = x;
  mPosBegin.y = y;
  mPosCenter = cv::Point(mPosBegin.x+mSize/2, mPosBegin.y+mSize/2);
  checkForBoundaries();
}

void VisionROI::setCenter(cv::Point pos) {
  mPosCenter.x = pos.x;
  mPosCenter.y = pos.y;
  checkForBoundaries();
}

void VisionROI::setCenter(int x, int y) {
  mPosCenter = cv::Point(x, y);
  checkForBoundaries();
}


void VisionROI::setIsLost(bool lost) {
  mIsLost = lost;
}

int VisionROI::getSize() {
  return mSize;
}

int VisionROI::getId() {
  return mId;
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

cv::Point VisionROI::getCenter() {
  return mPosCenter;
}

int VisionROI::getCenterX() {
  return mPosCenter.x;
}

int VisionROI::getCenterY() {
  return mPosCenter.y;
}


VisionROI::VisionROI(int width, int height, int size, int id) :
mIsLost(true), mPosBegin(cv::Point(0,0)), mSize(size), mId(id),
mWidth(width), mHeight(height) {
  mPosCenter = cv::Point(mPosBegin.x+mSize/2, mPosBegin.y+mSize/2);
  checkForBoundaries();
}

VisionROI::~VisionROI() {
}
