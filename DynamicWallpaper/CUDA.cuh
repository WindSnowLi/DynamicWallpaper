#pragma once
#include"cuda_runtime.h"
#include "device_launch_parameters.h"
//#include<iostream>
#include <Windows.h>
//using namespace std;
#include <stdio.h>
void ToCUDAWaveSpreadThreadStart(int* templpWave1, int* templpWave2, int m_iBmpWidth, int m_iBmpHeight);
void ToCUDACUDAWaveRenderThreadStart(int* m_pWaveBuf1, BYTE* m_pBmpRender, BYTE* m_pBmpSource, int m_iBytesPerWidth, int m_iBmpWidth, int m_iBmpHeight);