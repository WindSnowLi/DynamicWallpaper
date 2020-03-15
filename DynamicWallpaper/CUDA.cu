#include "CUDA.cuh"
#include <stdio.h>
#include <Windows.h>
__global__ void CUDAWaveSpread(int* m_pWaveBuf1, int* m_pWaveBuf2, int m_iBmpWidth, int m_iBmpHeight) {
	int* lpWave1 = m_pWaveBuf1;
	int* lpWave2 = m_pWaveBuf2;
	int Bid = blockIdx.x;
	int Tid = threadIdx.x;
	for (int i = m_iBmpWidth+ Bid*64*m_iBmpWidth+Tid * m_iBmpWidth; i < m_iBmpWidth + Bid * 64*m_iBmpWidth + Tid * m_iBmpWidth + m_iBmpWidth; i++)
	{
		//波能扩散
		lpWave2[i] = ((lpWave1[i - 1] + lpWave1[i - m_iBmpWidth] +
			lpWave1[i + 1] + lpWave1[i + m_iBmpWidth]) >> 1) - lpWave2[i];

		//波能衰减
		lpWave2[i] -= (lpWave2[i] >> 5);
	}
	
	
}

__global__ void CUDAWaveRender(int* m_pWaveBuf1, BYTE* m_pBmpRender, BYTE* m_pBmpSource, int m_iBytesPerWidth, int m_iBmpWidth, int m_iBmpHeight) {
	int iPtrSource = 0;
	int iPtrRender = 0;
	int iPosX = 0;
	int iPosY = 0;
	int y= blockIdx.x*64+threadIdx.x+1;
	int lineIndex = y * m_iBmpWidth;
	for (int x = 0; x < m_iBmpWidth; x++)
	{
		//根据波幅计算位图数据偏移值，渲染点（x，y)对应原始图片（iPosX，iPosY）
		iPosX = x + (m_pWaveBuf1[lineIndex - 1] - m_pWaveBuf1[lineIndex + 1]);
		iPosY = y + (m_pWaveBuf1[lineIndex - m_iBmpWidth] - m_pWaveBuf1[lineIndex + m_iBmpWidth]);
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
				m_pBmpRender[iPtrRender + c] = m_pBmpSource[iPtrSource + c];
			}
		}

		lineIndex++;
	}
}

void ToCUDAWaveSpreadThreadStart(int* templpWave1, int* templpWave2, int m_iBmpWidth, int m_iBmpHeight) {


	CUDAWaveSpread << <16, 64 >> > (templpWave1, templpWave2, m_iBmpWidth, m_iBmpHeight);
	

	cudaDeviceSynchronize();

	/*
	int* templpWave1;
	int* templpWave2;
	cudaMallocManaged(&templpWave1, sizeof(int) * m_iBmpWidth * m_iBmpHeight);
	cudaMallocManaged(&templpWave2, sizeof(int) * m_iBmpWidth * m_iBmpHeight);

	cudaMemcpy(templpWave1, lpWave1, sizeof(int) * m_iBmpWidth * m_iBmpHeight, cudaMemcpyHostToDevice);
	cudaMemcpy(templpWave2, lpWave2, sizeof(int) * m_iBmpWidth * m_iBmpHeight, cudaMemcpyHostToDevice);
	*/

	//testWaveSpread(lpWave1, lpWave2, m_iBmpWidth, m_iBmpHeight);

	//testWaveSpread << <1, 1 >> > (templpWave1, templpWave2, m_iBmpWidth, m_iBmpHeight);

	//cudaDeviceSynchronize();
	/*
	cudaMemcpy(lpWave2, templpWave1, sizeof(int) * m_iBmpWidth * m_iBmpHeight, cudaMemcpyDeviceToHost);
	cudaMemcpy(lpWave1, templpWave2, sizeof(int) * m_iBmpWidth * m_iBmpHeight, cudaMemcpyDeviceToHost);
	*/

}
void ToCUDACUDAWaveRenderThreadStart(int* m_pWaveBuf1, BYTE* m_pBmpRender, BYTE* m_pBmpSource, int m_iBytesPerWidth, int m_iBmpWidth, int m_iBmpHeight) {
	CUDAWaveRender << <32, 64 >> > (m_pWaveBuf1,m_pBmpRender,m_pBmpSource,m_iBytesPerWidth,m_iBmpWidth,m_iBmpHeight);
	cudaDeviceSynchronize();
}



