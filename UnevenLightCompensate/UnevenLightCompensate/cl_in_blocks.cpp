#include "def.h"
using namespace std;
using namespace cv;

/*****************************************************************************
 �� �� ��  : CompensateLightInBlocks
 ��������  : �����ȹ��ղ���
 �������  : Mat &image,   �Ҷ�ͼ��
             INT blockSize ���С
 �������  : Mat* dstImg,  ���ͼ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��6��5��
    ��    ��   : ccy0739
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG CompensateLightInBlocks(Mat &image, Mat* dstImg, INT blockSize)
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