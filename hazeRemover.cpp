#include "hazeRemover.h"

void getDarkPrior(cv::Mat src, cv::Mat& prior, int patchSize)
{
	CHECK(patchSize % 2 == 1) << "Patch size must be odd."; 
	cv::Mat pad_src;
	int border = (patchSize - 1) / 2;
	cv::copyMakeBorder(src, pad_src, border, border, border, border, cv::BORDER_CONSTANT, cv::Vec3b(255, 255, 255));
	prior = cv::Mat::zeros(src.size(), CV_8UC1);
	for(int i = border; i < src.rows + border; i++) {
		for(int j = border; j < src.cols + border; j++) {
			cv::Rect roi;
			roi.x 		= j - border;
			roi.y 		= i - border;
			roi.width 	= patchSize;
			roi.height 	= patchSize;
			cv::Mat crop 	= pad_src(roi);
			cv::Mat bgr[3];
			cv::Mat bgr32F;
			std::vector<float> bgr_vec3;
			cv::split(crop, bgr);
			for(int c = 0; c < 3; c++)
			{
				bgr[c].convertTo(bgr32F, CV_32F);
				std::vector<float> bgr_vec((float*)bgr32F.data, (float*)bgr32F.data + bgr32F.rows * bgr32F.cols);
				bgr_vec3.insert(bgr_vec3.end(), bgr_vec.begin(), bgr_vec.end());
			}
			auto val = std::min_element(std::begin(bgr_vec3), std::end(bgr_vec3));
			prior.at<char>(i - border, j - border) = *val;
		}
	}
}

void getAtmoLight(cv::Mat src, cv::Scalar& atmoLight, int patchSize, float percentAtmo)
{
	cv::Mat prior;

	// Calculate the dark prior of the source image.
	getDarkPrior(src, prior, patchSize);
	
	// Get a percentage of the brightest pixels of the dark prior.
	// Get the dark prior histogram.
	cv::Mat hist;
	int channels[] 		= {0};
	int histSize 		= 256;
	float range[] 		= {0, 256};
	const float* histRange 	= {range};
	cv::calcHist(&prior, 1, channels, cv::Mat(), hist, 1, &histSize, &histRange, true);

	// Iterating through the histogram, sum up the number of pixels until the sum is greater or equal to 0.1% of the image pixels. 
	float targetAtmo 	= src.rows * src.cols * percentAtmo;
	float sumAtmo 		= 0;
	int thresholdAtmo 	= 0;

	for(int i = 255; i >= 0; i--)
	{
		sumAtmo += hist.at<float>(i);
		if(sumAtmo >= targetAtmo)
		{
			thresholdAtmo = i - 1;
			break;
		}
	}

	// Apply a threshold to get a mask of the 0.1% brightest pixels.
	cv::Mat dst;
	cv::threshold(prior, dst, thresholdAtmo, 255, cv::THRESH_BINARY);

	atmoLight = cv::mean(src, dst);
}

void getTransmissionMap(cv::Mat src, cv::Mat& transmissionMap, cv::Scalar atmoLight, int patchSize, float omega)
{
	CHECK(patchSize % 2 == 1) << "Patch size must be odd.";
	
	cv::Mat pad_src;
        int border = (patchSize - 1) / 2;
        cv::copyMakeBorder(src, pad_src, border, border, border, border, cv::BORDER_CONSTANT, cv::Vec3b(255, 255, 255));
        
	transmissionMap = cv::Mat::zeros(src.size(), CV_32FC1);
        
	for(int i = border; i < src.rows + border; i++) {
                 for(int j = border; j < src.cols + border; j++) {
                         cv::Rect roi;
                         roi.x           = j - border;
                         roi.y           = i - border;
                         roi.width       = patchSize;
                         roi.height      = patchSize;
                         cv::Mat crop    = pad_src(roi);
                         cv::Mat bgr[3];
                         cv::Mat bgr32F;
                         std::vector<float> bgr_vec3;
                         cv::split(crop, bgr);
                         for(int c = 0; c < 3; c++)
                         {
                                 bgr[c].convertTo(bgr32F, CV_32F);
				 bgr32F /= atmoLight[c];
                                 std::vector<float> bgr_vec((float*)bgr32F.data, (float*)bgr32F.data + bgr32F.rows * bgr32F.cols);
                         	 auto min_patch = std::min_element(std::begin(bgr_vec), std::end(bgr_vec));
                                 bgr_vec3.push_back(*min_patch);
                         }
                         auto val = std::min_element(std::begin(bgr_vec3), std::end(bgr_vec3));
                         transmissionMap.at<float>(i - border, j - border) = 1 - omega * (*val);
                 }
        }
}


void recoverSceneRadiance(cv::Mat src, cv::Mat& radiance, int patchSize, float t0)
{
	cv::Scalar atmoLight;
	getAtmoLight(src, atmoLight);

	cv::Vec3f atmoLight_32F;
	atmoLight_32F[0] = (float)atmoLight[0];
	atmoLight_32F[1] = (float)atmoLight[1];
	atmoLight_32F[2] = (float)atmoLight[2];

        cv::Mat transmissionMap, refinedTransMap; 
        getTransmissionMap(src, transmissionMap, atmoLight, patchSize);
	cv::ximgproc::guidedFilter(src, transmissionMap, refinedTransMap, 20, 0.001);

	cv::Mat src_32FC3;
	src.convertTo(src_32FC3, CV_32F);
	
	cv::Mat radiance_32FC3 = cv::Mat::zeros(src.size(), CV_32FC3);

	for(int i = 0; i < src.rows; i++)
	{
		for(int j = 0; j < src.cols; j++)
		{
			radiance_32FC3.at<cv::Vec3f>(i, j) = (src_32FC3.at<cv::Vec3f>(i, j) - atmoLight_32F) / std::max(refinedTransMap.at<float>(i, j), t0) + atmoLight_32F;
		}
	}

	radiance_32FC3.convertTo(radiance, CV_8U);
}
