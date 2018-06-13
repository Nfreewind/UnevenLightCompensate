/*****************************************************************************
 �� �� ��  : HSEG_GetBindImgWithLocalThreshold
 ��������  : �ֲ���ֵ *ʹ�þ�ֵ�ͱ�׼����Ϊ�ж����� *����������������С����ֵϵ�����Լ���׼��ϵ��
 �������  : 
 �������  : 
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��6��5��
    ��    ��   : ccy0739
    �޸�����   : �����ɺ���

*****************************************************************************/
ULONG HSEG_GetBindImgWithLocalThreshold(Mat& srcImg, Mat* bindImg, INT windownSize, DOUBLE meanParam, DOUBLE stadevPara)
{

    if (!srcImg.data)
    {
        PRINT_ERROR(HSEG_ERROR_NO_IMAGE);
        return HSEG_ERROR_NO_IMAGE;
    }
    
    *bindImg = cv::Mat::zeros(srcImg.size(), srcImg.type());

    UCHAR* srcImgData = srcImg.data;
    UCHAR* bindImgData = bindImg->data;
    DOUBLE deta = 0.0;
    DOUBLE mean = 0.0;
    DOUBLE data = 0.0;

    /* ������β���� */
    for (INT col = windownSize / 2; col < (srcImg.cols - windownSize / 2); col++)
    {
        for (INT row = windownSize / 2; row < (srcImg.rows - windownSize / 2); row++)
        {
            deta = 0.0;
            mean = 0.0; 
            data = (DOUBLE)(srcImgData[col + row * srcImg.cols]);

            /* �������ھ�ֵ�ͱ�׼�� */
            for (INT i = -(windownSize / 2); i < windownSize / 2 + 1; i++)
            {
                for (INT j = -(windownSize / 2); j < windownSize / 2 + 1; j++)
                {
                    mean += srcImgData[(row + i) * srcImg.cols + col + j];
                }
            }
            mean /= (DOUBLE)(windownSize * windownSize);
            
            for (INT i = -(windownSize / 2); i <= windownSize / 2; i++)
            {
                for (INT j = -(windownSize / 2); j <= windownSize / 2; j++)
                {
                    deta += (srcImgData[(row + i) * srcImg.cols + col + j] - mean)
                        * (srcImgData[(row + i) * srcImg.cols + col + j] - mean);
                }
            }

            deta /= (DOUBLE)(windownSize * windownSize);
            deta = sqrt(deta);

            if (data> meanParam * mean && data > stadevPara * deta)
            {

                bindImgData[col + row * srcImg.cols] = 255;
            }
        
        }
    }

    return HCOM_OK;
}