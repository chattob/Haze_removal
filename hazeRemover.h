#include <glog/logging.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/edge_filter.hpp>

void getDarkPrior(cv::Mat src, cv::Mat& prior, int patchSize = 15);
void getAtmoLight(cv::Mat src, cv::Scalar& atmoLight, int patchSize = 15, float percentAtmo = 0.001);
void getTransmissionMap(cv::Mat src, cv::Mat& transmissionMap, cv::Scalar atmoLight, int patchSize = 15, float omega = 0.95);
void recoverSceneRadiance(cv::Mat src, cv::Mat& radiance, int patchSize = 15, float t0 = 0.1);
