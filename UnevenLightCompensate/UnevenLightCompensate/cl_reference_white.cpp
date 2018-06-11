#include "def.h"
using namespace std;
using namespace cv;

/*****************************************************************************
 �� �� ��  : CompensateLightReferenceWhite
 ��������  : ���ڲο��׵Ĳ����ȹ��ղ���
 �������  : Mat &image,   �Ҷ�ͼ��
             INT blockSize ���С
 �������  : Mat* dstImg,  ���ͼ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��6��11��
    ��    ��   : ccy0739
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG CompensateLightReferenceWhite(Mat &srcImg, Mat* dstImg)
{

    if (!srcImg.data || CV_8UC1 != srcImg.type())
    {
        return HCOM_ERR;
    }

    const DOUBLE whiteLevel = 0.05;
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