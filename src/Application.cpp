/*** 
 * @                                                    __----~~~~~~~~~~~------___
 * @                                   .  .   ~~//====......          __--~ ~~
 * @                   -.            \_|//     |||\\  ~~~~~~::::... /~
 * @                ___-==_       _-~o~  \/    |||  \\            _/~~-
 * @        __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 * @    _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 * @  .~       .~       |   \\ -_    /  /-   /   ||      \   /
 * @ /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 * @ |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 * @          '         ~-|      /|    |-~\~~       __--~~
 * @                      |-~~-_/ |    |   ~\_   _-~            /\
 * @                           /  \     \__   \/~                \__
 * @                       _--~ _/ | .-~~____--~-/                  ~~==.
 * @                      ((->/~   '.|||' -_|    ~~-/ ,              . _||
 * @                                 -_     ~\      ~~---l__i__i__i--~~_/
 * @                                 _-~-__   ~)  \--______________--~~
 * @                               //.-~~~-~_--~- |-------~~~~~~~~
 * @                                      //.-~~~--\
 * @                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @
 * @                              神兽保佑            永无BUG
 * @
 * @Author: vincent vincent_xjw@163.com
 * @Date: 2024-08-22 14:34:02
 * @LastEditors: vincent vincent_xjw@163.com
 * @LastEditTime: 2024-08-22 14:34:16
 * @FilePath: /620Q-Demo/src/Application.cpp
 * @Description: 
 * @
 * @Copyright (c) 2024 by vincent_xjw@163.com, All Rights Reserved. 
 */

#include "Application.hpp"
#include <iostream>
#include <string.h>
#include "ax_ivps_api.h"
#include "opencv2/opencv.hpp"
#include "ivps_hal.h"

Application::Application(int argc, char** argv)
{
    std::cout << __func__ << " [" << __LINE__ << "]" << std::endl;
}

Application::~Application()
{
    stopThread();
    std::cout << __func__ << " [" << __LINE__ << "]" << std::endl;
}

#if 1
void Application::threadFunction(Application *caller)
{
    AX_S32 ret = 0;
    AX_VIDEO_FRAME_T tSrcFrame = {0}, tDstFrame={0};
    AX_BLK BlkId = 0;
    AX_U32 nImgSize = 0;
    int ivpsGroupId = 1;
    int ivpsChannelId = 0;
    AX_IVPS_CROP_RESIZE_ATTR_T tCropResizeAttr;
    memset(&tCropResizeAttr, 0, sizeof(AX_IVPS_CROP_RESIZE_ATTR_T));

    ALOGI("IVPS Grp: %d, Chn: %d", ivpsGroupId, ivpsChannelId);
    tCropResizeAttr.tAspectRatio.eMode = AX_IVPS_ASPECT_RATIO_STRETCH;
    tCropResizeAttr.tAspectRatio.eAligns[0] = AX_IVPS_ASPECT_RATIO_HORIZONTAL_CENTER;
    tCropResizeAttr.tAspectRatio.eAligns[1] = AX_IVPS_ASPECT_RATIO_VERTICAL_CENTER;
    tCropResizeAttr.tAspectRatio.nBgColor = 0x0000FF;
    tDstFrame.u32PicStride[0] = 1920;
    tDstFrame.u32Width = 1920;
    tDstFrame.u32Height = 1080;

    while (caller->_isThreadRunning)
    {
        sleep(1);
        ret = AX_IVPS_GetChnFrame(ivpsGroupId, ivpsChannelId, &tSrcFrame, -1);
        if (ret)
        {
            /* reach EOF */
            printf("[WARN] CHN[%d] is empty ret:0x%x\n", ivpsChannelId, ret);
            continue;
        }

        printf("AX_IVPS_GetChnFrame(%lld): Chn:%d, (%d x %d) Stride:%d, Phy:%llx, UserData:%llx, PTS:%llx, BlockId:%x\n",
               tSrcFrame.u64SeqNum, ivpsChannelId, tSrcFrame.u32Width, tSrcFrame.u32Height, tSrcFrame.u32PicStride[0],
               tSrcFrame.u64PhyAddr[0], tSrcFrame.u64UserData, tSrcFrame.u64PTS, tSrcFrame.u32BlkId[0]);
        tSrcFrame.s16CropWidth = 1920;
        tSrcFrame.s16CropHeight = 1080;
        tDstFrame.enImgFormat = tSrcFrame.enImgFormat;

        printf("FBC:%d", tSrcFrame.stCompressInfo.enCompressMode);
        nImgSize = _calcImgSize(tDstFrame.u32PicStride[0], tDstFrame.u32Width,
                               tDstFrame.u32Height, tDstFrame.enImgFormat, 16);
        ret = _bufPoolBlockAddrGet(-1, nImgSize, &tDstFrame.u64PhyAddr[0], (AX_VOID **)(&tDstFrame.u64VirAddr[0]), &BlkId);
        if (ret)
        {
            ALOGE("Alloc BlkId fail, ret=0x%x", ret);
            goto ERR1;
        }
        AX_IVPS_CropResizeVpp(&tSrcFrame, &tDstFrame, &tCropResizeAttr);
        _toMat(&tDstFrame, ivpsGroupId, ivpsChannelId);

        ret = AX_POOL_ReleaseBlock(BlkId);
        if (ret)
        {
            ALOGE("Rls BlkId fail, ret=0x%x", ret);
        }
    ERR1:
        ret = AX_IVPS_ReleaseChnFrame(ivpsGroupId, ivpsChannelId, &tSrcFrame);
        if (ret)
        {
            ALOGE("AX_IVPS_ReleaseFrame fail, ret=0x%x", ret);
        }

        ALOGI("AX_IVPS_GetFrame");
    }
}
#endif

#if 0
void Application::threadFunction(Application *caller)
{
    int picNameIndex = 0;
    AX_BLK BlkId;
    AX_U32 nImgSize;
    AX_U32 u32FrameSize;
    AX_VOID *pVirAddr;
    int ivpsGroupId = 1;
    int ivpsChannelId = 0;
    AX_VIDEO_FRAME_T tVideoFrame;

    AX_CHAR OutFile[128] = {0};
    FILE *pstFile = NULL;
    sprintf(OutFile, "group%d_chn%d_%d_%d.yuv", ivpsGroupId, ivpsChannelId, tVideoFrame.u32Width, tVideoFrame.u32Height);

    memset(&tVideoFrame, 0, sizeof(AX_VIDEO_FRAME_T));
    
    while(caller->_isThreadRunning) {
        std::cout << "Application::threadFunction isRunning:" << caller->_isThreadRunning << " " << _isThreadRunning << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        continue;
        
        int ret = AX_IVPS_GetChnFrame(ivpsGroupId, ivpsChannelId, &tVideoFrame, -1);

        if (0 != ret) {
            if (AX_ERR_IVPS_BUF_EMPTY == ret) {
                printf("GRP[%d]CHN[%d] read empty", ivpsGroupId, ivpsChannelId);
                std::this_thread::sleep_for(std::chrono::microseconds(1000));
                continue;
            }
            printf("GRP[%d]CHN[%d] AX_IVPS_GetChnFrame failed", ivpsGroupId, ivpsChannelId);
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            continue;
        }

        if (tVideoFrame.enImgFormat == AX_FORMAT_YUV420_SEMIPLANAR)
            u32FrameSize = tVideoFrame.u32PicStride[0] * tVideoFrame.u32Height * 3 / 2;

        // pVirAddr = AX_SYS_Mmap(tVideoFrame.u64PhyAddr[0], u32FrameSize);
        pVirAddr = AX_POOL_GetBlockVirAddr(tVideoFrame.u32BlkId[0]);
        if (pVirAddr) {
            ALOGI("pVirAddr: 0x%px, u64PhyAddr: 0x%llx, u32FrameSize: 0x%x\n", pVirAddr, tVideoFrame.u64PhyAddr[0], u32FrameSize);
            sprintf(OutFile, "%d.yuv", picNameIndex++);
            if (!pstFile) pstFile = fopen(OutFile, "wb");
            if (pstFile) {
                fwrite(pVirAddr, u32FrameSize, 1, pstFile);
                fclose(pstFile);
                ALOGI("write %s done\n", OutFile);
            }

            AX_S32 s32Ret = AX_SYS_Munmap(pVirAddr, u32FrameSize);
            if (s32Ret)
                ALOGE("AX_SYS_Munmap failed, s32Ret=0x%x\n", s32Ret);
        } else {
            ALOGE("AX_SYS_Mmap failed\n");
        }

        ret = AX_IVPS_ReleaseChnFrame(ivpsGroupId, ivpsChannelId, &tVideoFrame);
    }

    if (pstFile) fclose(pstFile);
}
#endif
#if 0
void Application::threadFunction(Application *caller)
{
    AX_S32 ret = 0;
    AX_VIDEO_FRAME_T tSrcFrame, tDstFrame;
    AX_BLK BlkId;
    AX_U32 nImgSize;
    int ivpsGroupId = 1;
    int ivpsChannelId = 0;

    ALOGI("IVPS Grp: %d, Chn: %d", ivpsGroupId, ivpsChannelId);

    tDstFrame.u32PicStride[0] = 1920;
    tDstFrame.u32Width = 1920;
    tDstFrame.u32Height = 1080;

    while (caller->_isThreadRunning)
    {
        sleep(1);
        ret = AX_IVPS_GetChnFrame(ivpsGroupId, ivpsChannelId, &tSrcFrame, -1);
        if (ret)
        {
            /* reach EOF */
            printf("[WARN] CHN[%d] is empty ret:0x%x\n", ivpsChannelId, ret);
            continue;
        }

        printf("AX_IVPS_GetChnFrame(%lld): Chn:%d, (%d x %d) Stride:%d, Phy:%llx, UserData:%llx, PTS:%llx, BlockId:%x\n",
               tSrcFrame.u64SeqNum, ivpsChannelId, tSrcFrame.u32Width, tSrcFrame.u32Height, tSrcFrame.u32PicStride[0],
               tSrcFrame.u64PhyAddr[0], tSrcFrame.u64UserData, tSrcFrame.u64PTS, tSrcFrame.u32BlkId[0]);
        tSrcFrame.s16CropWidth = 1920;
        tSrcFrame.s16CropHeight = 1080;
        tDstFrame.enImgFormat = tSrcFrame.enImgFormat;

        printf("FBC:%d", tSrcFrame.stCompressInfo.enCompressMode);
        nImgSize = _calcImgSize(tDstFrame.u32PicStride[0], tDstFrame.u32Width,
                               tDstFrame.u32Height, tDstFrame.enImgFormat, 16);
        ret = _bufPoolBlockAddrGet(-1, nImgSize, &tDstFrame.u64PhyAddr[0], (AX_VOID **)(&tDstFrame.u64VirAddr[0]), &BlkId);
        if (ret)
        {
            ALOGE("Alloc BlkId fail, ret=0x%x", ret);
            goto ERR1;
        }
        
        _toMat(&tDstFrame, ivpsGroupId, ivpsChannelId);

        ret = AX_POOL_ReleaseBlock(BlkId);
        if (ret)
        {
            ALOGE("Rls BlkId fail, ret=0x%x", ret);
        }
    ERR1:
        ret = AX_IVPS_ReleaseChnFrame(ivpsGroupId, ivpsChannelId, &tSrcFrame);
        if (ret)
        {
            ALOGE("AX_IVPS_ReleaseFrame fail, ret=0x%x", ret);
        }

        ALOGI("AX_IVPS_GetFrame");
    }
}
#endif
#if 0
void Application::threadFunction(Application *caller)
{
    int picNameIndex = 0;
    AX_BLK BlkId;
    AX_U32 nImgSize;
    AX_U64 u64VirAddr;
    while(caller->_isThreadRunning) {
        std::cout << "Application::threadFunction isRunning:" << caller->_isThreadRunning << " " << _isThreadRunning << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // continue;
        int ivpsGroupId = 1;
        int ivpsChannelId = 0;
        AX_VIDEO_FRAME_T tVideoFrame;
        memset(&tVideoFrame, 0, sizeof(AX_VIDEO_FRAME_T));
        int ret = AX_IVPS_GetChnFrame(ivpsGroupId, ivpsChannelId, &tVideoFrame, -1);

        if (0 != ret) {
            if (AX_ERR_IVPS_BUF_EMPTY == ret) {
                printf("GRP[%d]CHN[%d] read empty", ivpsGroupId, ivpsChannelId);
                std::this_thread::sleep_for(std::chrono::microseconds(1000));
                continue;
            }
            printf("GRP[%d]CHN[%d] AX_IVPS_GetChnFrame failed", ivpsGroupId, ivpsChannelId);
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            continue;
        }

        tVideoFrame.u64VirAddr[0] = (AX_ULONG)AX_POOL_GetBlockVirAddr(tVideoFrame.u32BlkId[0]);
        tVideoFrame.u64PhyAddr[0]  = AX_POOL_Handle2PhysAddr(tVideoFrame.u32BlkId[0]);
        tVideoFrame.u32FrameSize = tVideoFrame.u32PicStride[0] * tVideoFrame.u32Height * 3 / 2;

        AX_U32 nPixelSize = (AX_U32)tVideoFrame.u32PicStride[0] * tVideoFrame.u32Height;

        printf("\nApplication::threadFunction frameSize=%d width=%d height=%d add=%u enImgFormat=%d.\n", tVideoFrame.u32FrameSize, tVideoFrame.u32Width, tVideoFrame.u32Height, tVideoFrame.u64VirAddr[0], tVideoFrame.enImgFormat);           
        printf("\nSaveFile nPixelSize: %d u32FrameSize: %d Height:%d Width:%d PhyAddrY:%llx PhyAddrUV:%llx Format:%d\n",
           nPixelSize, tVideoFrame.u32FrameSize, tVideoFrame.u32Height, tVideoFrame.u32Width,
           tVideoFrame.u64PhyAddr[0], tVideoFrame.u64PhyAddr[1], tVideoFrame.enImgFormat);
        
        uchar *pp = (uchar *)(uintptr_t)(tVideoFrame.u64PhyAddr[0]);

        cv::Mat yuvimg(tVideoFrame.u32Height * 3 / 2, tVideoFrame.u32Width, CV_8UC1, pp);
        cv::Mat rgbimg(tVideoFrame.u32Height, tVideoFrame.u32Width, CV_8UC3);
        cv::cvtColor(yuvimg, rgbimg, cv::COLOR_YUV2BGR_NV21);

        std::string fileName = std::to_string(picNameIndex++) + ".jpg";
        cv::imwrite(fileName, rgbimg);

        ret = AX_IVPS_ReleaseChnFrame(ivpsGroupId, ivpsChannelId, &tVideoFrame);
    }
}
#endif

uint32_t Application::_calcImgSize(uint32_t nStride, uint32_t nW, uint32_t nH, AX_IMG_FORMAT_E eType, uint32_t nAlign)
{
   uint32_t nBpp = 0;
    if (nW == 0 || nH == 0)
    {
        ALOGE("Invalid width %d or height %d!", nW, nH);
        return 0;
    }
    if (0 == nStride)
    {
        nStride = (0 == nAlign) ? nW : ALIGN_UP(nW, nAlign);
    }
    else
    {
        if (nAlign > 0)
        {
            if (nStride % nAlign)
            {
                ALOGE("stride: %u not %u aligned.!", nStride, nAlign);
                return 0;
            }
        }
    }
    switch (eType)
    {
    case AX_FORMAT_YUV400:
        nBpp = 8;
        break;
    case AX_FORMAT_YUV420_PLANAR:
    case AX_FORMAT_YUV420_SEMIPLANAR:
    case AX_FORMAT_YUV420_SEMIPLANAR_VU:
        nBpp = 12;
        break;
    case AX_FORMAT_YUV422_INTERLEAVED_YUYV:
    case AX_FORMAT_YUV422_INTERLEAVED_UYVY:
    case AX_FORMAT_RGB565:
    case AX_FORMAT_BGR565:
        nBpp = 16;
        break;
    case AX_FORMAT_YUV444_PACKED:
    case AX_FORMAT_RGB888:
    case AX_FORMAT_BGR888:
        nBpp = 24;
        break;
    case AX_FORMAT_RGBA8888:
    case AX_FORMAT_ARGB8888:
        nBpp = 32;
        break;
    default:
        nBpp = 0;
        break;
    }
    return nStride * nH * nBpp / 8;
}

int Application::_bufPoolBlockAddrGet(uint32_t PoolId, uint32_t BlkSize, AX_U64 *nPhyAddr, void **pVirAddr, uint32_t *BlkId)
{
    *BlkId = AX_POOL_GetBlock(PoolId, BlkSize, NULL);
    *nPhyAddr = AX_POOL_Handle2PhysAddr(*BlkId);
    if (!(*nPhyAddr))
    {
        ALOGE("fail!");
        return -1;
    }

    ALOGI("success (Blockid:0x%X --> PhyAddr=0x%llx)", *BlkId, *nPhyAddr);
    *pVirAddr = AX_POOL_GetBlockVirAddr(*BlkId);

    if (!(*pVirAddr))
    {
        ALOGE("fail!");
        return -2;
    }

    ALOGI("success blockVirAddr=0x%p", *pVirAddr);
    return 0;
}

void Application::_toMat(AX_VIDEO_FRAME_T *tDstFrame, int nGrpIdx, int nChnIdx)
{
    AX_U32 nPixelSize;
    AX_S32 s32Ret1 = 0;
    AX_S32 s32Ret2 = 0;
    AX_S32 s32Ret3 = 0;
    AX_S32 bit_num = 0;
    AX_U8 nStoragePlanarNum = 0;
    uint8_t *buf = nullptr;
    uint32_t bufSize = 0;

    nPixelSize = (AX_U32)tDstFrame->u32PicStride[0] * tDstFrame->u32Height;

    switch (tDstFrame->enImgFormat)
    {
    case AX_FORMAT_YUV420_PLANAR:
    case AX_FORMAT_YUV420_SEMIPLANAR:
    case AX_FORMAT_YUV420_SEMIPLANAR_VU:
    case AX_FORMAT_YUV422_SEMIPLANAR: /* NV16 */
    case AX_FORMAT_YUV422_SEMIPLANAR_VU:
    case AX_FORMAT_YUV422_INTERLEAVED_YUYV:
        bit_num = 8;
        nStoragePlanarNum = 2;
        break;
    case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P101010:
    case AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P101010:
        bit_num = 10;
        nStoragePlanarNum = 2;
        break;
    case AX_FORMAT_YUV420_SEMIPLANAR_10BIT_P010:
    case AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010:
        bit_num = 16;
        nStoragePlanarNum = 2;
        break;
    case AX_FORMAT_YUV444_PACKED:
    case AX_FORMAT_RGB888:
    case AX_FORMAT_BGR888:
    case AX_FORMAT_RGB565:
    case AX_FORMAT_BGR565:
    case AX_FORMAT_RGBA8888:
    case AX_FORMAT_ARGB8888:
    case AX_FORMAT_ARGB4444:
    case AX_FORMAT_ARGB1555:
    case AX_FORMAT_ARGB8565:
    case AX_FORMAT_RGBA5551:
    case AX_FORMAT_RGBA4444:
    case AX_FORMAT_RGBA5658:
    case AX_FORMAT_ABGR4444:
    case AX_FORMAT_ABGR1555:
    case AX_FORMAT_ABGR8888:
    case AX_FORMAT_ABGR8565:
    case AX_FORMAT_BGRA8888:
    case AX_FORMAT_BGRA5551:
    case AX_FORMAT_BGRA4444:
    case AX_FORMAT_BGRA5658:
    case AX_FORMAT_YUV400:
        nStoragePlanarNum = 1;
        break;
    default:
        ALOGE("Not support fromat %d", tDstFrame->enImgFormat);
        return;
        break;
    }

    printf("toMat nPixelSize: %d u32FrameSize: %d Height:%d Width:%d PhyAddrY:%llx PhyAddrUV:%llx Format:%d\n",
           nPixelSize, tDstFrame->u32FrameSize, tDstFrame->u32Height, tDstFrame->u32Width,
           tDstFrame->u64PhyAddr[0], tDstFrame->u64PhyAddr[1], tDstFrame->enImgFormat);

    switch (nStoragePlanarNum)
    {
    case 2:
        if (!tDstFrame->u64PhyAddr[1])
        {
            if (tDstFrame->stCompressInfo.enCompressMode)
                tDstFrame->u64PhyAddr[1] = tDstFrame->u64PhyAddr[0] + tDstFrame->u32PicStride[0] * tDstFrame->u32Height * tDstFrame->stCompressInfo.u32CompressLevel / 8;
            else
                tDstFrame->u64PhyAddr[1] = tDstFrame->u64PhyAddr[0] + tDstFrame->u32PicStride[0] * tDstFrame->u32Height;
        }
        if (tDstFrame->stCompressInfo.enCompressMode)
            nPixelSize = nPixelSize * bit_num / 8 * tDstFrame->stCompressInfo.u32CompressLevel / 8;
        else
            nPixelSize = nPixelSize * bit_num / 8;

        if (AX_FORMAT_YUV422_SEMIPLANAR == tDstFrame->enImgFormat || AX_FORMAT_YUV422_SEMIPLANAR_VU == tDstFrame->enImgFormat || AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P101010 == tDstFrame->enImgFormat || AX_FORMAT_YUV422_SEMIPLANAR_10BIT_P010 == tDstFrame->enImgFormat)
        {
            tDstFrame->u64VirAddr[0] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[0], nPixelSize);
            tDstFrame->u64VirAddr[1] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[1], nPixelSize);
            // fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), 1, nPixelSize, fp);
            // fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[1]), 1, nPixelSize, fp);
            bufSize = nPixelSize*2;
            buf = new uint8_t[nPixelSize*2];
            memcpy(buf, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), nPixelSize);
            memcpy(buf+nPixelSize, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[1]), nPixelSize);

            s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[0], nPixelSize);
            s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[1], nPixelSize);
        }
        else
        {
            printf("------------------\n");
            tDstFrame->u64VirAddr[0] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[0], nPixelSize);
            tDstFrame->u64VirAddr[1] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[1], nPixelSize / 2);
            //fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), 1, nPixelSize, fp);
            //fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[1]), 1, nPixelSize / 2, fp);
            bufSize = nPixelSize + nPixelSize/2;
            buf = new uint8_t[nPixelSize + nPixelSize/2];
            memcpy(buf, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), nPixelSize);
            memcpy(buf+nPixelSize, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[1]), nPixelSize/2);

            s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[0], nPixelSize);
            s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[1], nPixelSize / 2);
        }
        break;
    case 3:
        tDstFrame->u64VirAddr[0] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[0], nPixelSize);
        tDstFrame->u64VirAddr[1] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[1], nPixelSize / 2);
        tDstFrame->u64VirAddr[2] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[2], nPixelSize / 2);
        //fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), 1, nPixelSize, fp);
        //fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[1]), 1, nPixelSize / 2, fp);
        //fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[2]), 1, nPixelSize / 2, fp);
        bufSize = nPixelSize + nPixelSize / 2 + nPixelSize / 2;
        buf = new uint8_t[nPixelSize + nPixelSize / 2 + nPixelSize / 2];
        memcpy(buf, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), nPixelSize);
        memcpy(buf+nPixelSize, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[1]), nPixelSize/2);
        memcpy(buf+nPixelSize+nPixelSize/2, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[2]), nPixelSize/2);

        s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[0], nPixelSize);
        s32Ret2 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[1], nPixelSize / 2);
        s32Ret3 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[2], nPixelSize / 2);
        break;
    default:
        if (tDstFrame->u32FrameSize)
        {
            tDstFrame->u64VirAddr[0] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[0], tDstFrame->u32FrameSize);
            //fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), 1, tDstFrame->u32FrameSize, fp);
            bufSize = tDstFrame->u32FrameSize;
            buf = new uint8_t[tDstFrame->u32FrameSize];
            memcpy(buf, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), tDstFrame->u32FrameSize);

            s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[0], tDstFrame->u32FrameSize);
        }
        else
        {
            tDstFrame->u64VirAddr[0] = (AX_ULONG)AX_SYS_Mmap(tDstFrame->u64PhyAddr[0], nPixelSize * 3);
            // fwrite((AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), 1, nPixelSize * 3, fp);
            bufSize = nPixelSize * 3;
            buf = new uint8_t[nPixelSize * 3];
            memcpy(buf, (AX_VOID *)((AX_ULONG)tDstFrame->u64VirAddr[0]), nPixelSize * 3);
            s32Ret1 = AX_SYS_Munmap((AX_VOID *)(AX_ULONG)tDstFrame->u64VirAddr[0], nPixelSize * 3);
        }
        break;
    }

    if (buf != nullptr) {
        cv::Mat yuvimg(tDstFrame->u32Height * 3 / 2, tDstFrame->u32Width, CV_8UC1, buf);
        cv::Mat rgbimg(tDstFrame->u32Height, tDstFrame->u32Width, CV_8UC3);
        cv::cvtColor(yuvimg, rgbimg, cv::COLOR_YUV2BGR_NV12);
        std::string fileName = std::to_string(picNameIndex++) + ".jpg";
        cv::imwrite(fileName, rgbimg);
        printf("Save success!\n");
    }

    if (s32Ret1 || s32Ret2 || s32Ret3)
    {
        printf("AX_SYS_Munmap s32Ret1=0x%x ,s32Ret2=0x%x ,s32Ret2=0x%x", s32Ret1, s32Ret2, s32Ret3);
    }
}