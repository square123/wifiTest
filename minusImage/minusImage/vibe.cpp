#include "ViBe.h"

using namespace std;
using namespace cv;

int c_xoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};  //x的邻居点
int c_yoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};  //y的邻居点

ViBeHue::ViBeHue(Mat &src) //在构造函数中应该加入是否进行hue的选项。 
{
	if (src.channels()!=3)
	{
		cout<<"输入图像必须是彩色图像！"<<endl;
	}
	src.copyTo(oriImg);
	GaussianBlur(oriImg,oriImgGauss,Size(3,3),0,0);
	cvtColor(oriImgGauss,oriImgGray,CV_RGB2GRAY);
	cvtColor(oriImgGauss,oriImgCovert,CV_BGR2HLS_FULL);
	split(oriImgCovert,orisplit);
	init(oriImgGray);
	processFirstFrame(oriImgGray);	
}

ViBeHue::~ViBeHue(void)
{

}

/**************** Assign space and init ***************************/
void ViBeHue::init(const Mat& _image)
{
	m_samples.create(_image.size(),CV_8UC(NUM_SAMPLES));
	m_samples.setTo(0);
	m_mask = Mat::zeros(_image.size(),CV_8UC1);
	m_foregroundMatchCount = Mat::zeros(_image.size(),CV_8UC1);
}

/**************** Init model from first frame ********************/
void ViBeHue::processFirstFrame(const Mat& _image)
{
	RNG rng;
	int rstep, cstep;

	int stepsize = _image.step;
	for (int i = 1;i<_image.rows-1;i++)
	{
		uchar* sam_ptr = m_samples.ptr<uchar>(i);
		const uchar* img_ptr = _image.ptr<uchar>(i);
		for (int j=1;j<_image.cols-1;j++)
		{
			int cindex = j*m_samples.elemSize();

			for (int k=0;k<NUM_SAMPLES;k++)
			{
				int random = rng.uniform(0, 9);
				rstep = c_yoff[random];
				random = rng.uniform(0, 9);
				cstep = c_xoff[random];
				sam_ptr[cindex+k] = *(img_ptr+rstep*stepsize+j+cstep);

			}
		}

	}

}

/**************** Test a new frame and update model ********************/
void ViBeHue::testAndUpdate(const Mat& _image)
{
	RNG rng;
	int stepsize = _image.step;

	for(int i = 1; i < _image.rows-1; i++)
	{
		uchar* sam_ptr = m_samples.ptr<uchar>(i);
		const uchar* ptr_img = _image.ptr<uchar>(i);
		uchar* fg_ptr = m_foregroundMatchCount.ptr<uchar>(i);
		uchar* mask_ptr = m_mask.ptr<uchar>(i);

		for(int j = 1; j < _image.cols-1; j++)
		{
			int matches(0), count(0);
			float dist;

			int cindex = j*m_samples.elemSize();

			while(matches < MIN_MATCHES && count < NUM_SAMPLES)
			{
				dist = abs(sam_ptr[cindex + count] - ptr_img[j]);
				if (dist < RADIUS)
					matches++;
				count++;
			}

			if (matches >= MIN_MATCHES)
			{
				// It is a background pixel
				fg_ptr[j] = 0;

				// Set background pixel to 0
				mask_ptr[j] = 0;

				// 如果一个像素是背景点，那么它有 1 / defaultSubsamplingFactor 的概率去更新自己的模型样本值
				int random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					random = rng.uniform(0, NUM_SAMPLES);
					sam_ptr[cindex+random] = ptr_img[j];
				}

				// 同时也有 1 / defaultSubsamplingFactor 的概率去更新它的邻居点的模型样本值
				random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					int row, col;
					random = rng.uniform(0, 9);
					row = c_yoff[random];
					random = rng.uniform(0, 9);
					col = c_xoff[random];
					cindex = cindex + col*m_samples.elemSize();
					random = rng.uniform(0, NUM_SAMPLES);
					*( sam_ptr+row*m_samples.step+cindex+random) = ptr_img[j];

				}
			}
			else
			{
				// It is a foreground pixel
				fg_ptr[j] ++;

				// Set background pixel to 255
				mask_ptr[j] = 255;

				//如果某个像素点连续N次被检测为前景，则认为一块静止区域被误判为运动，将其更新为背景点
				if (fg_ptr[j] > 15)//原来才参数为50，做了修改
				{
					int random = rng.uniform(0, NUM_SAMPLES);
					if (random == 0)
					{
						random = rng.uniform(0, NUM_SAMPLES);
						sam_ptr[cindex+random] = ptr_img[j];
					}
				}
			}
		}
	}
}

void ViBeHue::operator()(Mat &image, Mat &fgmask)//要求输入的矩阵是彩色的矩阵
{
	if (image.channels()!=3)
	{
		cout<<"输入图像必须是彩色三通道图像"<<endl;
	}

	//获得基本矩阵
	Mat imageGauss,imageCovt,imageGray;
	vector<Mat> imageSplit;
	GaussianBlur(image,imageGauss,Size(3,3),0,0);
	cvtColor(imageGauss,imageGray,CV_RGB2GRAY);
	cvtColor(imageGauss,imageCovt,CV_BGR2HLS_FULL);
	split(imageCovt,imageSplit);

	//获得vibe的基本矩阵
	Mat tempMat;
	testAndUpdate(imageGray);
	tempMat = getMask();
	imshow("ooo",tempMat);
	//预处理部分(还需要再完善)
 	Mat smoothMat;
 	deNoise(tempMat,fgmask);

/* 	imshow("ori",fgmask);*/

//     //hue处理
// 	Mat diffHue,hueSmooth,diffGray,graySmooth;
// 	absdiff(imageSplit.at(0),orisplit.at(0),diffHue);
// 	absdiff(imageGray,oriImgGray,diffGray);
	//threshold(diffHue,hueSmooth,20,255,THRESH_BINARY);//将hue的差截断处理
	//threshold(diffGray&smoothMat,graySmooth,20,255,THRESH_BINARY);//将hue的差截断处理
	/*Mat normMat;*/
	//normalize(diffGray&smoothMat,normMat,1.0,0.1,NORM_MINMAX); //归一化函数
	

	//将hue与vibe合并	
// 	Mat hueAndVibe,hueAndVibeEd;
// 	addWeighted(diffHue&smoothMat,0.4,diffGray&smoothMat,0.6,0,hueAndVibe);
// 	threshold(hueAndVibe,fgmask,0,255,THRESH_OTSU);
}