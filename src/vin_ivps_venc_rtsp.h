#ifndef _VIN_IVPS_VENC_RTSP_H_
#define _VIN_IVPS_VENC_RTSP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "ax_venc_api.h"
#include "ax_ivps_api.h"
#include "ax_isp_api.h"
#include "common_sys.h"
#include "common_venc.h"
#include "common_vin.h"
#include "common_cam.h"
#include "common_nt.h"
#include "common_isp.h"
#include "AXRtspWrapper.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ALIGN_UP_16(value) ((value + 0xF) & (~0xF))
#define ALIGN_UP_64(value) ((value + 0x3F) & (~0x3F))
#define SAMPLE_VENC_CHN_NUM_MAX   (3)
/* #define SAMPLE_IVPS_CROPRESIZE_ENABLE */
/* #define SAMPLE_IVPS_CROPRESIZE_MANUAL_MODE */

typedef struct _stVencGetStreamParam
{
    AX_S32 VeChn;
    AX_BOOL bThreadStart;
    AX_PAYLOAD_TYPE_E ePayloadType;
} VENC_GETSTREAM_PARAM_T;
typedef struct
{
    AX_S32 chnNum;
    AX_BOOL bGetStrmStart;
    pthread_t getStrmPid;
} SAMPLE_VENC_SELECT_PARA_T;

typedef struct _stRCInfo
{
    SAMPLE_VENC_RC_E eRCType;
    AX_U32 nMinQp;
    AX_U32 nMaxQp;
    AX_U32 nMinIQp;
    AX_U32 nMaxIQp;
    AX_S32 nIntraQpDelta;
} RC_INFO_T;

typedef struct _stVideoConfig
{
    AX_PAYLOAD_TYPE_E ePayloadType;
    AX_U32 nGOP;
    AX_U32 nSrcFrameRate;
    AX_U32 nDstFrameRate;
    AX_U32 nStride;
    AX_S32 nInWidth;
    AX_S32 nInHeight;
    AX_S32 nOutWidth;
    AX_S32 nOutHeight;
    AX_IMG_FORMAT_E eImgFormat;
    RC_INFO_T stRCInfo;
    AX_S32 nBitrate;
} VIDEO_CONFIG_T;

typedef enum {
    SAMPLE_VIN_NONE  = -1,
    SAMPLE_VIN_SINGLE_DUMMY  = 0,
    SAMPLE_VIN_SINGLE_OS04A10 = 1,
    SAMPLE_VIN_SINGLE_OS08A20  = 2,
    SAMPLE_VIN_SINGLE_SC200AI  = 3,
    SAMPLE_VIN_BUTT
} SAMPLE_VIN_CASE_E;

typedef struct {
    AX_U32 nStride;
    AX_S32 nWidth;
    AX_S32 nHeight;
    AX_IMG_FORMAT_E eImgFormat;
    AX_COMPRESS_MODE_E eFbcMode;
} SAMPLE_CHN_ATTR_T;

typedef struct {
    SAMPLE_VIN_CASE_E eSysCase;
    COMMON_VIN_MODE_E eSysMode;
    AX_SNS_HDR_MODE_E eHdrMode;
    SAMPLE_LOAD_RAW_NODE_E eLoadRawNode;
    AX_BOOL bAiispEnable;
    AX_S32 nDumpFrameNum;
    AX_S32 nPipeId; /* For VIN */
    AX_S32 nGrpId;  /* For IVPS */
    AX_S32 nOutChnNum;
    char *pFrameInfo;
    AX_VIN_IVPS_MODE_E eMode;
    AX_IVPS_ROTATION_E eRotAngle;
    AX_U32 statDeltaPtsFrmNum;
    AX_U32 bVencSelect;
} SAMPLE_VIN_PARAM_T;

typedef struct {
    AX_BOOL bEnable;
    AX_RTSP_HANDLE pRtspHandle;
} SAMPLE_RTSP_PARAM_T;


AX_VOID __cal_dump_pool(COMMON_SYS_POOL_CFG_T pool[], AX_SNS_HDR_MODE_E eHdrMode, AX_S32 nFrameNum);
AX_VOID __set_pipe_hdr_mode(AX_U32 *pHdrSel, AX_SNS_HDR_MODE_E eHdrMode);
AX_VOID __set_vin_attr(AX_CAMERA_T *pCam, SAMPLE_SNS_TYPE_E eSnsType, AX_SNS_HDR_MODE_E eHdrMode,
                              COMMON_VIN_MODE_E eSysMode, AX_BOOL bAiispEnable);
AX_U32 __sample_case_single_dummy(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs);
AX_U32 __sample_case_single_os04a10(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs);
AX_U32 __sample_case_single_sc200ai(AX_CAMERA_T *pCamList, SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs);
AX_U32 __sample_case_config(SAMPLE_VIN_PARAM_T *pVinParam, COMMON_SYS_ARGS_T *pCommonArgs,
                                   COMMON_SYS_ARGS_T *pPrivArgs);
AX_VOID SAMPLE_DeltaPtsStatistic(AX_S32 Chn, AX_VENC_STREAM_T *pstStream);
AX_VOID *SAMPLE_VencSelectGetStreamProc(AX_VOID *arg);
/* venc get stream task 从编码器中获取编码后的流并推RTSP流*/
void *VencGetStreamProc(void *arg);
AX_S32 SAMPLE_VencStartSelectGetStream(SAMPLE_VENC_SELECT_PARA_T *pstArg);
AX_S32 SAMPLE_VENC_Init(SAMPLE_VIN_PARAM_T *pVinParam, AX_S32 nChnNum, AX_IVPS_ROTATION_E eRotAngle, AX_U32 bVencSelect);
AX_S32 SAMPLE_VencStopSelect();
AX_S32 SAMPLE_VENC_DeInit(AX_S32 nChnNum, AX_BOOL bVencSelect);
int SAMPLE_IVPS_Init(SAMPLE_VIN_PARAM_T *pVinParam, AX_S32 nGrpId, AX_S32 nChnNum, AX_IVPS_ROTATION_E eRotAngle);
AX_S32 SAMPLE_IVPS_DeInit(AX_S32 nGrpId, AX_S32 nChnNum);
AX_S32 SAMPLE_SYS_LinkInit(AX_S32 nGrpId, AX_U8 nChnNum);
AX_S32 SampleLinkDeInit(AX_S32 nGrpId, AX_U8 nChnNum);
AX_S32 SampleRtspInit(AX_S32 nChnNum);
AX_S32 SampleRtspDeInit(AX_VOID);
long get_time_in_microseconds();
AX_VOID *GetFrameThreadForAI(AX_VOID *pArg);
AX_S32 TestGetFrameThreadStart(AX_S32 nIvpsGrp, AX_S32 nIvpsChn);
AX_S32 SAMPLE_VIN_IVPS_VENC_RTSP(SAMPLE_VIN_PARAM_T *pVinParam);
int SampleExec();

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */