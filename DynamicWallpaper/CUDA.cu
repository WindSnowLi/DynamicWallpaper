#include "CUDA.cuh"
#include <stdio.h>
#include <Windows.h>
__global__ void CUDAWaveSpread(int* AlpWave1, int*AlpWave2, int m_iBmpWidth, int m_iBmpHeight) {
	int Bid = blockIdx.x;
	int Tid = threadIdx.x;
	for (int i = m_iBmpWidth+ Bid*64*m_iBmpWidth+Tid * m_iBmpWidth; i < m_iBmpWidth + Bid * 64*m_iBmpWidth + Tid * m_iBmpWidth + m_iBmpWidth; i++)
	{
		//波能扩散
		AlpWave2[i] = ((AlpWave1[i - 1] + AlpWave1[i - m_iBmpWidth] +
			AlpWave1[i + 1] + AlpWave1[i + m_iBmpWidth]) >> 1) - AlpWave2[i];

		//波能衰减
		AlpWave2[i] -= (AlpWave2[i] >> 5);
	}
	
	
}

__global__ void CUDAWaveRender(int* AlpWave1,  BYTE* tempM_pBmpRender, BYTE* tempM_pBmpSource, int m_iBytesPerWidth, int m_iBmpWidth, int m_iBmpHeight) {
	int iPtrSource = 0;
	int iPtrRender = 0;
	int iPosX = 0;
	int iPosY = 0;
	int y= blockIdx.x*64+threadIdx.x+1;
	int lineIndex = y * m_iBmpWidth;
	for (int x = 0; x < m_iBmpWidth; x++)
	{
		//根据波幅计算位图数据偏移值，渲染点（x，y)对应原始图片（iPosX，iPosY）
		iPosX = x + (AlpWave1[lineIndex - 1] - AlpWave1[lineIndex + 1]);
		iPosY = y + (AlpWave1[lineIndex - m_iBmpWidth] - AlpWave1[lineIndex + m_iBmpWidth]);
		//另外一种计算偏移的方法
		//int waveData = (1024 - m_pWaveBuf1[lineIndex]);
		//iPosX = (x - m_iBmpWidth/2)*waveData/1024 + m_iBmpWidth/2;
		//iPosY = (y - m_iBmpHeight/2)*waveData/1024 + m_iBmpHeight/2;

		if (0 <= iPosX && iPosX < m_iBmpWidth &&
			0 <= iPosY && iPosY < m_iBmpHeight)
		{
			//分别计算原始位图（iPosX，iPosY）和渲染位图（x，y)对应的起始位图数据
			iPtrSource = iPosY * m_iBytesPerWidth + iPosX * 3;
			iPtrRender = y * m_iBytesPerWidth + x * 3;
			//渲染位图，重新打点数据
			for (int c = 0; c < 3; c++)
			{
				tempM_pBmpRender[iPtrRender + c] = tempM_pBmpSource[iPtrSource + c];
				///投入波源、图片渲染以及计算波幅的线程并行，会无形中修改tempM_pBmpSource的值，具体原因未知，以向tempM_pBmpSource重复赋值暂避
			}
		}

		lineIndex++;
	}
}

__global__ void ModifyCUDALpWave(int* AlpWave1, int m_iBmpWidth, int m_iBmpHeight,int x, int y, int stoneSize, int stoneWeight) {
	int posX = 0;
	int posY = 0;
	for (int i = -stoneSize; i < stoneSize; i++)
	{
		for (int j = -stoneSize; j < stoneSize; j++)
		{
			posX = x + i;
			posY = y + j;

			//控制范围，不能超出图片
			if (posX < 0 || posX >= m_iBmpWidth ||
				posY < 0 || posY >= m_iBmpHeight)
			{
				continue;
			}
			//在一个圆形区域内，初始化波能缓冲区1
			if (i * i + j * j <= stoneSize * stoneSize)
			{
				AlpWave1[posY * m_iBmpWidth + posX] = stoneWeight;
			}

		}
	}
}

void ToCUDAWaveSpreadThreadStart(int* AlpWave1, int* AlpWave2, int m_iBmpWidth, int m_iBmpHeight) {
	CUDAWaveSpread << <16, 64 >> > (AlpWave1, AlpWave2, m_iBmpWidth, m_iBmpHeight);
	cudaDeviceSynchronize();
}
void ToCUDACUDAWaveRenderThreadStart(int* AlpWave1, BYTE* tempM_pBmpRender, BYTE* tempM_pBmpSource, int m_iBytesPerWidth, int m_iBmpWidth, int m_iBmpHeight) {
	CUDAWaveRender << <32, 64 >> > (AlpWave1, tempM_pBmpRender, tempM_pBmpSource,m_iBytesPerWidth,m_iBmpWidth,m_iBmpHeight);
	cudaDeviceSynchronize();
}
void ToModifyCUDALpWaveThreadStart(int* AlpWave1, int m_iBmpWidth, int m_iBmpHeight, int x, int y, int stoneSize, int stoneWeight) {
	ModifyCUDALpWave << <1, 1 >> > (AlpWave1, m_iBmpWidth, m_iBmpHeight, x, y, stoneSize, stoneWeight);
}


