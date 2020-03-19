#pragma once
#include"cuda_runtime.h"
#include "device_launch_parameters.h"
//#include<iostream>
#include <Windows.h>
//using namespace std;
#include <stdio.h>
void ToCUDAWaveSpreadThreadStart(int* AlpWave1, int* AlpWave2, int m_iBmpWidth, int m_iBmpHeight);
void ToCUDACUDAWaveRenderThreadStart(int* AlpWave1, BYTE* tempM_pBmpRender, BYTE* tempM_pBmpSource, int m_iBytesPerWidth, int m_iBmpWidth, int m_iBmpHeight);

void ToModifyCUDALpWaveThreadStart(int* AlpWave1, int m_iBmpWidth, int m_iBmpHeight, int x, int y, int stoneSize, int stoneWeight);