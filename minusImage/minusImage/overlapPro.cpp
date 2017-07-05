#include "overlapPro.h"

overlapPro::overlapPro()
{
	prePicBwMix.reserve(100);
	memset(bwMixTimeRaw,0,sizeof(bwMixTimeRaw));
	memset(bwMixIndex,0,sizeof(bwMixIndex)); 
	//������д���ʱ���ʱ���׼���������Ƕ�4������ݽ���ѹ��
	time_t syscTimee=time(NULL);	//����ִֻ��һ�Σ������Ͳ���Ҫ�ٲ�����
	processIndex=syscTimee+4;//��Ϊ��Ҫ�ͺ�������Ҫ������ı�־λ�ͺ󣬾�����ͺ�ʱ����������޸�
}

overlapPro::~overlapPro()
{
	outfile.close();
}
void overlapPro::fillHole(const Mat srcBw, Mat &dstBw)//�ο����ϵ���亯����������
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//��չͼ��
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));//�ǳ������������㷨�ķ�����

	Mat cutImg;//�ü���չ��ͼ��
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

//ѡ����ʷ�Χ����ͨ����
void overlapPro::selectArea(Mat &src,Mat &dst, int minNum,int maxNum)
{
	vector<vector<Point>> contours;  
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()<1)  //ȥ���߽�ֵ
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
		for (int i = 0; i < contours.size(); i++)
		{
			if (maxNum>0?(contourArea(contours[i])>maxNum||contourArea(contours[i])<minNum):(contourArea(contours[i])<minNum))
			{
				drawContours(dst,contours,i,0,CV_FILLED);
			} 
			else
			{
				drawContours(dst,contours,i,255,CV_FILLED);
			}
		}
	}
}

//��������ͨ������͹���ϲ�
void overlapPro::hullArea(Mat &src,Mat &dst) 
{
	vector<vector<Point>> contours;  //ͨ���÷���Ҳ�ǿ��Եģ������䵱��ԭ������vector<Point>��Ч������˫ͨ������
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> hull(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		convexHull(Mat(contours[i]),hull[i],false);
	}
	if(contours.size()<1)  //ȥ���߽�ֵ
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,hull,i,(255),CV_FILLED);
		}
	}
}

//�����ṹ��ĺ���
void overlapPro::getBwMix(Mat &src,vector<bwMix> &dst)
{
	vector<Mat> contours; //����ط��ú�˼����
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()>=1)  //ȥ���߽�ֵ
	{
		for (int i = 0; i < contours.size(); i++)
		{
			bwMix temp;
			temp.bwTime=time(NULL);//��ȡʱ��
			temp.markLabel=i;
			temp.bwEdge=contours[i];
			temp.bwRect=boundingRect(contours[i]);
			//���һ���������ĵĺ���
			//Moments m=moments(contours[i],false);
			//temp.center.x=m.m10/m.m00;
			//temp.center.y=m.m01/m.m00;
			dst.push_back(temp);
		}
	}
}

//���ݽṹ��ת���ɲ�ɫ����//��ɫ������ �����б����������ļ���������������˻���ָ��ѵĺۼ� �������ļ�������Ϊ10�ˣ�Ҳ��ͨ���ṹ��������ͼ����Ϣ
void overlapPro::bwMixToColorMat(vector<bwMix> &src,Mat &dst)
{
	Scalar globalColor[10]; //������ɫ��
	globalColor[0] = Scalar( 67, 128, 6 );
	globalColor[1] = Scalar( 84, 80, 153 );
	globalColor[2] = Scalar( 57, 60, 79 );
	globalColor[3] = Scalar( 255, 255, 0 );
	globalColor[4] = Scalar( 61, 63, 17 );
	globalColor[5] = Scalar( 155, 175, 131 );
	globalColor[6] = Scalar( 101, 67, 254 );
	globalColor[7] = Scalar( 154, 157, 252 );
	globalColor[8] = Scalar( 173, 205, 249 );
	globalColor[9] = Scalar( 169, 200, 200 );
	if (src.size()>=1&&src.size()<11)//�����ɫ�ı߽�ֵ����Ҫ��ֵ̫�࣬����������ʱ����ʾͼ��
	{
		for (int i=0;i<src.size();i++)
		{
			vector<Mat> tempMat;
			tempMat.push_back(src[i].bwEdge);//����ת�棬�����������һ������
			drawContours(dst,tempMat,0,globalColor[src[i].markLabel],CV_FILLED,8);//����Ӧ�ı�ǩ����ɫ���
		}
	}
}

//��������ȽϺ���
float overlapPro::rectOverlap(const Rect & preBox,const Rect & nowBox) //ֱ��ʹ��Rect����
{ 
	Rect rect=preBox & nowBox;//ȡ��������Ľ���
	if (nowBox.area()!=0)
	{
		return float(rect.area())/float(nowBox.area());
	}else
	{
		return 0.0;
	}
}

//�þɵľ���ͼ���ţ������µı�ŵĺ��� ��������֡overlap���£�ͨ���ṹ������������ͼ�����Ϣ,��û�б��ʱ����취ȥ��ֵ�����ܳ������������ȼٶ���10��
//�����Ȳ������жϵ��������������жϵ������Ӧ��Ҫ��һ������ĸо���������ȡƽ�����о�����һ����ȽϺã�����˼·�Ƚ�����
void overlapPro::renewVec(vector<bwMix> &src, vector<bwMix> &dst,float thd)
{
	//����һ���� n*m��С
	int m=src.size();
	int n=dst.size();
	std::set<int> S;//ͨ������sɸѡ����
	//����һά���飬�����ά����
	Mat table(n,m,CV_32FC1,0.0);//��openCV��ʹ��mat��Ϊ2ά���������
	for (int i = 0; i < n; i++)
	{
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++)
		{
			data[j]=rectOverlap(src[j].bwRect,dst[i].bwRect);
			S.insert(src[j].markLabel);//�޳��ظ���Ԫ��
		}
	}
	//ͨ������в�ѯ����ɱ�ŵĸ��� �����к�n��Ӧ����Ҫ���µı�ţ�m�ǿ���ɸѡ�ı��
	for (int i = 0; i < n; i++)//��ʼ����
	{
		float maxNum=thd;
		int maxIndex=-1;
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++) //�ҳ�ÿ���б��У�����ֵ
		{
			if (maxNum<data[j])
			{
				maxNum=data[j];
				maxIndex=j;
			}
		}
		//���´��ڵı�� Ҫ�����¼���ͺ���û�е�����
		if (maxIndex!=-1)
		{
			dst[i].markLabel=src[maxIndex].markLabel;
		}else//����µı��
		{
			for (int k=0;k<30;k++)//ֻ���0-19�ڵ�Ԫ��
			{
				if(S.find(k)==S.end())//��Ӳ����ڵı�ţ������Ų�����ʱ
				{
					dst[i].markLabel=k;
					S.insert(k);
					break;
				}
			}
		}
	}
}

//��õõ����ݵĺ���λ
int overlapPro::charTimeGetSecond(char ttt[14])
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}

//�켣���㹫ʽ �������ڵ�����������켣 ��Ҫ���ǵ��жϵ����⣬��Ϊ�켣������жϣ����ж�ʱҪ����켣���Ժ��ٿ���,�ڹ켣��Ӧ�ü����˲��Ĺ��̣���Ϊͼ�����˶�������ͼ��ᷢ���仯
//�켣���ܻ������©�ĳ��������ھͲ���Ҫ�����ж�
//���ڷ�ɢ����Ӧ����һ���ϲ��Ĺ��̣�����ͬ��ŵ�����ϲ���ͬһ���塣
//������Ҫ��ͬһ��ŵĺϲ���Ȼ��ͬһ��ŵ���Ϣȡʱ��ƽ��
void overlapPro::bwMixToOrbit(vector<bwMix> &src)  //���ﻹ���ܴ�������ͻȻ��ⲻ���Ŀ���
{ 
	//���ҳ����ж����ظ��ı��
	std::set<int> S;//ͨ������sɸѡ����
	for (int i = 0; i < src.size(); i++)
	{
		S.insert(src[i].markLabel);
	}
	//���ظ���ŵ����ºϲ�Ϊһ�����󣬼���������,�������ϵ�ͳһ        ���������ͨ������������룬���ŷֲ�Ӧ����������ɣ�������������������
	for (auto i = S.begin(); i != S.end(); i++)//set����ͨ���±���������ʣ����ͨ������������
	{
		vector<Point> tempMat;//ÿ��������һ������
		for (int j = 0; j < src.size(); j++)
		{
			if (*i==src[j].markLabel)
			{ 
				//cout<<*i<<endl;
				int rowNumber=src[j].bwEdge.rows;
				//int colNumber=src[j].bwEdge.cols*src[j].bwEdge.channels();
				for (int ii=0;ii<rowNumber;ii++)
				{
					int* data=src[j].bwEdge.ptr<int>(ii);
					Point pointt;
					pointt.x=data[0];
					pointt.y=data[1];
					tempMat.push_back(pointt);//����ת�棬�����������һ������
				}
			}
		}
		//����Ϣ����ṹ���ڣ�Ϊ��һ��ƽ����׼��
		char timeFix[16];
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&src[0].bwTime));//ʱ���޸�
		int second=charTimeGetSecond(timeFix);
		//��ȡ�������ģ�������ڱ궨��̫����
// 		Moments m=moments(tempMat,false);
// 		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.x=m.m10/m.m00;
// 		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.y=m.m01/m.m00;

		//ѡȡ�����Ӿ��εĵױ����ĵ� ���ܻ���Ҫ���о���Ĳ���
		Rect tempRect=boundingRect(Mat(tempMat));
		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.x = (tempRect.br().x)-(tempRect.width*0.5);
		bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center.y = tempRect.br().y;

		memcpy(bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].Timestamp,timeFix,sizeof(char)*14);
		//cout<<"���Ϊ"<<*i<<"��������"<<bwMixTimeRaw[second][*i][bwMixIndex[second][*i]].center<<endl;
		bwMixIndex[second][*i]++;
	}
}

//���ṹ��ĺ���ȡƽ��������֮ǰ��������գ�����ѡ��������������ʱ���ϵ�ͳһ
//������Ϣѹ����1��
void overlapPro::timeRawProcess()
{
	char timeFixed[16];
	time_t processIndexInit=processIndex-4;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processIndexInit));
	int second=charTimeGetSecond(timeFixed);
	
	for (int i=0;i<count_num;i++)//ÿ��count
	{
		double xSum=0.0,ySum=0.0;
		if (bwMixIndex[second][i]!=0)//����ղ�����
		{
			for (int j=0;j<bwMixIndex[second][i];j++)//�������
			{
				xSum+=bwMixTimeRaw[second][i][j].center.x;
				ySum+=bwMixTimeRaw[second][i][j].center.y;
			}
			int xAve=xSum/bwMixIndex[second][i]; //������ͼ���ϵľ��󣬾ͻ���ȡ���ȽϺ�
			int yAve=ySum/bwMixIndex[second][i];

			//��ʱ��������
// 			int r1=sqrt((xAve-342)*(xAve-342)+(yAve-209)*(yAve-209));
// 			int r2=sqrt((xAve-113)*(xAve-113)+(yAve-188)*(yAve-188));
// 			int r3=sqrt((xAve-293)*(xAve-293)+(yAve-112)*(yAve-112));
// 			int r4=sqrt((xAve-180)*(xAve-180)+(yAve-99)*(yAve-99));
// 
// 			//�ļ��������(��ʱ�ĺ��������Ժ�������Kalman���и���)
// 			cout<<timeFixed<<","<<i<<","<<r1<<","<<r2<<","<<r3<<","<<r4<<endl;
// 			outfile<<timeFixed<<","<<i<<","<<r1<<","<<r2<<","<<r3<<","<<r4<<endl;
		}
	}
	//��ղ���
	memset(bwMixTimeRaw[second],0,sizeof(bwMixRawData)*count_num*max_num);
	memset(bwMixIndex[second],0,sizeof(int)*count_num);
}

void overlapPro::deHue(Mat &src,Mat &colorSrc,Mat &oriImgHue,Mat &dst) //��ͨ��ͼ�����У���ֵͼ�����ͼ��
{
	//�����hue��������ȡ
	Mat ImgCovert; //ɫ��ת�����ͼƬ
	vector<Mat> imgSplit;//��Ҫ�����hls����
	cvtColor(colorSrc,ImgCovert,CV_BGR2HLS_FULL);
	split(ImgCovert,imgSplit);
	Mat diffHue,hueSmooth;
	absdiff(imgSplit.at(0),oriImgHue,diffHue);
	threshold(diffHue,hueSmooth,20,255,THRESH_BINARY); //ѡ���hue����
	//�ϲ�
	vector<vector<Point>> contours;  //ͨ���÷���Ҳ�ǿ��Եģ������䵱��ԭ������vector<Point>��Ч������˫ͨ������
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> deHueEdge(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		Mat temp=Mat::zeros(src.size(),CV_8UC1);
		drawContours(temp,contours,i,(255),CV_FILLED);//����һ���ݴ�����ڸþ����н��в���
		approxPolyDP(temp&hueSmooth,deHueEdge[i],3,true); //ȡ����Ľ�����Ȼ��ѡȡ�����Hue�����������Χ����
	}
	if(contours.size()<1)  //ȥ���߽�ֵ
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,deHueEdge,i,(255),CV_FILLED);
		}
	}
}

void overlapPro::process(Mat &src,Mat & colorSrc, Mat &oriSrcHue)     //��ʦ����˿��Բ���������ɫ��ʵ�ָ��٣�������ĳЩ�����Ҳ��һ�ֱȽϺõķ������ڴ����м����ɫ����
{
	vector<bwMix> picBwMix;
	picBwMix.reserve(100);
	selectArea(src,selectedMat,180,-1);//ͨ��������һЩ��ͨ��С�Ľ���ɸѡ  �Ժ���Ը���ʵ���ж�ɸѡ ���ϡ������Ŀ��ĸ���,���߳��Բ��������ص�ʱ�ķָ�
	//imshow("dd",selectedMat); 
	Mat ImgGauss; //��˹�˲����ͼƬ
	GaussianBlur(colorSrc,ImgGauss,Size(3,3),0,0); //��������õ�����ȥ����һ�β���
	//���г�����ж� ����Ͳ������ˣ�������������ٿ���
	Mat ll;
	//����hueȥ��һЩ����ķ����������� �÷�������Ի����Ŀ�ѡ����
	deHue(selectedMat,ImgGauss,oriSrcHue,ll);
	imshow("xx",ll);
	//����ͨ������͹����α�ʾ�����
	hullArea(ll,hullMat);
	/*imshow("hh",hullMat);*/

	//�ٽ���ͨ����ϲ� ��ֹ��������ȱʧ  ����ʱ�����ǣ�Ĭ�����ܱȽϺã�
	//��ȡ�ṹ�壨���庯���ṹ�壩
	time_t systime=time(NULL);//����ϵͳʱ��
	getBwMix(hullMat,picBwMix);//�õ��ṹ��    �ڵõ��ṹ���м����ɫ���ʷ���������������������

	//�Խṹ����и���
	//�·�ͼ���������Ϸ�ͼ��ı�� ��������ǰһ֡�ͺ�һ֡��
	//���û�б�������¸�ֵһ���µı��    
	//������Ѿ�������ʱ��Ҫ��취ȥ�أ���ô����ʧ���⣬���Ҳ����Ҫ�������Ȳ����ǣ���5�½�����д�������ٿ��Ǹ����飬�Ӵ��㷨�ĸ��Ӷȣ�
	if (prePicBwMix.size()!=0)  //kalman�˲�Ӧ�ÿ�������������Ŀ�궪ʧ��Ĳ�������Ŀ���Ԥ������ٴ�Ԥ�⡣������������
	{
		renewVec(prePicBwMix,picBwMix,0.0);//�ڽ���ƥ��ʱӦ�ÿ��ǲ�ɫ������ƥ����еĸ��£�������������
	}
	prePicBwMix=picBwMix;//�����ݽ��и���
	showColor=Mat::zeros(hullMat.size(),CV_8UC3);
	bwMixToColorMat(picBwMix,showColor);
	imshow("color",showColor);
	//����켣
	bwMixToOrbit(picBwMix);   //kalman�˲�Ӧ�ÿ�������������Ŀ�궪ʧ��Ĳ�������Ŀ���Ԥ������ٴ�Ԥ�⡣����������
	if (systime>=processIndex)//���в�ղ��� ���մﵽѭ������,�ĳɴ��ڵ�������Ϊ�����п��ܴ�������
	{
		timeRawProcess();
		processIndex++;
	}
}
