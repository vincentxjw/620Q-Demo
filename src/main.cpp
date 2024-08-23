/*** 
 * @                       _oo0oo_
 * @                      o8888888o
 * @                      88" . "88
 * @                      (| -_- |)
 * @                      0\  =  /0
 * @                    ___/`---'\___
 * @                  .' \\|     |// '.
 * @                 / \\|||  :  |||// \
 * @                / _||||| -:- |||||- \
 * @               |   | \\\  - /// |   |
 * @               | \_|  ''\---/''  |_/ |
 * @               \  .-\__  '-'  ___/-. /
 * @             ___'. .'  /--.--\  `. .'___
 * @          ."" '<  `.___\_<|>_/___.' >' "".
 * @         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 * @         \  \ `_.   \_ __\ /__ _/   .-` /  /
 * @     =====`-.____`.___ \_____/___.-`___.-'=====
 * @                       `=---='
 * @
 * @
 * @     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @
 * @           佛祖保佑     永不宕机     永无BUG
 * @
 * @Author: vincent vincent_xjw@163.com
 * @Date: 2024-08-21 14:04:57
 * @LastEditors: vincent vincent_xjw@163.com
 * @LastEditTime: 2024-08-22 14:14:25
 * @FilePath: /620Q-Demo/src/main.c
 * @Description: 
 * @
 * @Copyright (c) 2024 by vincent_xjw@163.com, All Rights Reserved. 
 */

#include "vin_ivps_venc_rtsp.h"
#include "ivps_hal.h"
#include <iostream>
#include <Application.hpp>

static AX_VOID SigInt(AX_S32 signo)
{
    ALOGI2("SigInt Catch signal %d", signo);
    ThreadLoopStateSet(AX_TRUE);
}

static AX_VOID SigStop(AX_S32 signo)
{
    ALOGI2("SigStop Catch signal %d", signo);
    ThreadLoopStateSet(AX_TRUE);
}

int main(int argc, char *argv[])
{
    std::cout << "hello world." << std::endl;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SigInt);
    signal(SIGTSTP, SigStop);

    Application app(argc, argv);
    app.startThread();
    SampleExec();

    return 0;
}
