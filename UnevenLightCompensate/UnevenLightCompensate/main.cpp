#include"common_def.h"

ULONG compensateLightInBlocks(Mat &image, Mat* dstImg, INT blockSize);
ULONG compensateLightReferenceWhite(Mat &srcImg, Mat* dstImg);

int main()
{
	
	Mat srcImg;
	Mat grayImg;
	Mat blocksImg;
	Mat referenceWhite;

	srcImg = imread("timg.jpg");
	cvtColor(srcImg, grayImg, CV_RGB2GRAY);

	compensateLightInBlocks(grayImg, &blocksImg, 10);
	imshow("blocksImg", blocksImg);

	compensateLightReferenceWhite(grayImg, &referenceWhite);
	imshow("referenceWhite", referenceWhite);

	while (true)
	{
		waitKey(0);
	}

	return 0;
}

/*****************************************************************************
 �� �� ��  : compensateLightInBlocks
 ��������  : �����ȹ��ղ�������������
 �������  : Mat &image,   �Ҷ�ͼ��
             INT blockSize ���С
 �������  : Mat* dstImg,  ���ͼ��

 �޸���ʷ      :
  1.��    ��   : 2018��6��5��
    ��    ��   : chengcy
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG compensateLightInBlocks(Mat &image, Mat* dstImg, INT blockSize)
{
    if (!image.data)
    {
        return HCOM_ERR;
    }

    /* ͼ������ҶȾ�ֵ */
    DOUBLE average = mean(image)[0];

    /* �ֿ� */
    INT newRows = (INT)ceil(DOUBLE(image.rows) / DOUBLE(blockSize));
    INT newCols = (INT)ceil(DOUBLE(image.cols) / DOUBLE(blockSize));
    Mat blockImage;
    blockImage = Mat::zeros(newRows, newCols, CV_64FC1);

    INT minRow;
    INT maxRow;
    INT minCol;
    INT maxCol;
    DOUBLE temaver;
    Mat imageROI;

    /* ��ÿ�����ڵľ�ֵ */
    for (INT i = 0; i < newRows; i++)
    {
        for (INT j = 0; j < newCols; j++)
        {
            minRow = i * blockSize;
            maxRow = (i + 1) * blockSize;
            if (maxRow > image.rows)
            {
                maxRow = image.rows;
            }

            minCol = j * blockSize;
            maxCol = (j + 1) * blockSize;
            if (maxCol > image.cols)
            {
                maxCol = image.cols;
            }

            imageROI = image(Range(minRow, maxRow), Range(minCol, maxCol));
            temaver = mean(imageROI)[0];
            blockImage.at<DOUBLE>(i, j) = temaver;
        }
    }

    blockImage = blockImage - average;

    Mat resizeBlockImage;
    resize(blockImage, resizeBlockImage, image.size(), INTER_CUBIC);

    Mat doubleImage;
    image.convertTo(doubleImage, CV_64FC1);

    *dstImg = doubleImage - resizeBlockImage;
    (*dstImg).convertTo(*dstImg, CV_8UC1);  

    return HCOM_OK;
}

/*****************************************************************************
 �� �� ��  : CompensateLightReferenceWhite
 ��������  : ���ڲο��׵Ĳ����ȹ��ղ���
 �������  : Mat &image,   �Ҷ�ͼ��
             INT blockSize ���С
 �������  : Mat* dstImg,  ���ͼ��

 �޸���ʷ      :
  1.��    ��   : 2018��6��11��
    ��    ��   : chengcy
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG compensateLightReferenceWhite(Mat &srcImg, Mat* dstImg)
{

    if (!srcImg.data || CV_8UC1 != srcImg.type())
    {
        return HCOM_ERR;
    }

    const DOUBLE whiteLevel = 0.01;
    ULONG cols = srcImg.cols;
    ULONG rows = srcImg.rows;

    /* ͳ��ֱ��ͼ */
    INT histogram[256] = {0};
    UCHAR* grayData = srcImg.data;
    for(ULONG i = 0; i < cols; i++)
    {     
        for(ULONG j = 0;j < rows; j++)
        { 
            histogram[grayData[i + j * cols]] ++;
        }
    }

    ULONG refNum =0;
    ULONG totalNum = rows * cols ;
    ULONG grayVal;

    //ǰwhiteLevel�ĸ߻Ҷ�����
    for(ULONG i = 0;i < 256; i++)
    {
        if((DOUBLE)(refNum / totalNum) < whiteLevel) 
        {
            refNum += histogram[255-i]; 
            grayVal = i;
        }
        else
            break;
    }

    //�ο�����ƽ��ֵ
    ULONG aveGray = 0;
    refNum =0;
    for(ULONG i = 255;i >= 255 - grayVal; i--)
    {
        aveGray += histogram[i] * i;  //�ܵ����صĸ���*�Ҷ�ֵ
        refNum += histogram[i];     //�ܵ�������
    }
    aveGray /=refNum;

    //���߲�����ϵ��
    DOUBLE coe = 255.0 / (DOUBLE )aveGray;

    *dstImg = srcImg.clone();
    UCHAR* dstData = dstImg->data;
    for(ULONG i = 0; i < cols; i++)
    {     
        for(ULONG j = 0;j < rows; j++)
        {
            dstData[i + j * cols] = saturate_cast<UCHAR>(coe * dstData[i + j * cols] + 0.5);
        }
    }

    return HCOM_OK;
}