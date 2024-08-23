/*** 
 * @                       ::
 * @                      :;J7, :,                        ::;7:
 * @                      ,ivYi, ,                       ;LLLFS:
 * @                      :iv7Yi                       :7ri;j5PL
 * @                     ,:ivYLvr                    ,ivrrirrY2X,
 * @                     :;r@Wwz.7r:                :ivu@kexianli.
 * @                    :iL7::,:::iiirii:ii;::::,,irvF7rvvLujL7ur
 * @                   ri::,:,::i:iiiiiii:i:irrv177JX7rYXqZEkvv17
 * @                ;i:, , ::::iirrririi:i:::iiir2XXvii;L8OGJr71i
 * @              :,, ,,:   ,::ir@mingyi.irii:i:::j1jri7ZBOS7ivv,
 * @                 ,::,    ::rv77iiiriii:iii:i::,rvLq@huhao.Li
 * @             ,,      ,, ,:ir7ir::,:::i;ir:::i:i::rSGGYri712:
 * @           :::  ,v7r:: ::rrv77:, ,, ,:i7rrii:::::, ir7ri7Lri
 * @          ,     2OBBOi,iiir;r::        ,irriiii::,, ,iv7Luur:
 * @        ,,     i78MBBi,:,:::,:,  :7FSL: ,iriii:::i::,,:rLqXv::
 * @        :      iuMMP: :,:::,:ii;2GY7OBB0viiii:i:iii:i:::iJqL;::
 * @       ,     ::::i   ,,,,, ::LuBBu BBBBBErii:i:i:i:i:i:i:r77ii
 * @      ,       :       , ,,:::rruBZ1MBBqi, :,,,:::,::::::iiriri:
 * @     ,               ,,,,::::i:  @arqiao.       ,:,, ,:::ii;i7:
 * @    :,       rjujLYLi   ,,:::::,:::::::::,,   ,:i,:,,,,,::i:iii
 * @    ::      BBBBBBBBB0,    ,,::: , ,:::::: ,      ,,,, ,,:::::::
 * @    i,  ,  ,8BMMBBBBBBi     ,,:,,     ,,, , ,   , , , :,::ii::i::
 * @    :      iZMOMOMBBM2::::::::::,,,,     ,,,,,,:,,,::::i:irr:i:::,
 * @    i   ,,:;u0MBMOG1L:::i::::::  ,,,::,   ,,, ::::::i:i:iirii:i:i:
 * @    :    ,iuUuuXUkFu7i:iii:i:::, :,:,: ::::::::i:i:::::iirr7iiri::
 * @    :     :rk@Yizero.i:::::, ,:ii:::::::i:::::i::,::::iirrriiiri::,
 * @     :      5BMBBBBBBSr:,::rv2kuii:::iii::,:i:,, , ,,:,:i@petermu.,
 * @          , :r50EZ8MBBBBGOBBBZP7::::i::,:::::,: :,:,::i;rrririiii::
 * @              :jujYY7LS0ujJL7r::,::i::,::::::::::::::iirirrrrrrr:ii:
 * @           ,:  :@kevensun.:,:,,,::::i:i:::::,,::::::iir;ii;7v77;ii;i,
 * @           ,,,     ,,:,::::::i:iiiii:i::::,, ::::iiiir@xingjief.r;7:i,
 * @        , , ,,,:,,::::::::iiiiiiiiii:,:,:::::::::iiir;ri7vL77rrirri::
 * @         :,, , ::::::::i:::i:::i:i::,,,,,:,::i:i:::iir;@Secbone.ii:::
 * @
 * @Author: vincent vincent_xjw@163.com
 * @Date: 2024-08-22 14:33:53
 * @LastEditors: vincent vincent_xjw@163.com
 * @LastEditTime: 2024-08-22 14:35:44
 * @FilePath: /620Q-Demo/src/Application.hpp
 * @Description: 
 * @
 * @Copyright (c) 2024 by vincent_xjw@163.com, All Rights Reserved. 
 */

#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "ax_global_type.h"

class Application
{
public:
    Application(int argc, char** argv);
    ~Application();

    void startThread()
    {
        if (!_isThreadRunning) {
            std::lock_guard<std::mutex> lock(_isThreadRunningMutex);
            _isThreadRunning = true;
            _thread = std::thread(&Application::threadFunction, this, this);
            _thread.detach();
        }
    }

    void stopThread()
    {
        if (_isThreadRunning) {
            {
                std::lock_guard<std::mutex> lock(_isThreadRunningMutex);
                _isThreadRunning = false;
            }
            if (_thread.joinable()) {
                _thread.join();
            }
        }
    }

    bool isRuning() const { return _isThreadRunning; }

private:
    std::thread _thread;
    void threadFunction(Application *caller);
    bool _isThreadRunning = false;
    std::mutex _isThreadRunningMutex;
    std::condition_variable cv;

    uint32_t _calcImgSize(uint32_t nStride, uint32_t nW, uint32_t nH, AX_IMG_FORMAT_E eType, uint32_t nAlign);
    int _bufPoolBlockAddrGet(uint32_t PoolId, uint32_t BlkSize, AX_U64 *nPhyAddr, void **pVirAddr, uint32_t *BlkId);
    void _toMat(AX_VIDEO_FRAME_T *tDstFrame, int nGrpIdx, int nChnIdx);

    int picNameIndex = 0;
};

#endif // __APPLICATION_H_