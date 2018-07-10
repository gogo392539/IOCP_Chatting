#include "stdafx.h"

#include "stdafx.h"

C_VOICE::C_VOICE() :
	m_waveFormat{},
	m_hWaveIn(nullptr),
	m_arWaveInHdrBuf{},
	m_hWaveOut(nullptr),
	m_arWaveOutHdrBuf{},
	m_bWaveInStart(false),
	m_bWaveOutStart(false),
	m_nOutIdx(0)
{
}

void C_VOICE::init()
{
	m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels = 1;
	m_waveFormat.nSamplesPerSec = 8000;
	m_waveFormat.wBitsPerSample = 16;
	m_waveFormat.nBlockAlign = m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
	m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;
	m_waveFormat.cbSize = 0;

	for (int i = 0; i < MAX_QUEUE_COUNT; i++)
	{
		//m_arWaveInHdrBuf[i] = (WAVEHDR*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WAVEHDR));
		//m_arWaveInHdrBuf[i]->lpData = (char*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, QUEUE_SIZE);
		m_arWaveInHdrBuf[i] = new WAVEHDR{};
		m_arWaveInHdrBuf[i]->lpData = new char[QUEUE_SIZE] {};
		m_arWaveInHdrBuf[i]->dwBufferLength = QUEUE_SIZE;
		m_arWaveInHdrBuf[i]->dwFlags = 0;
		m_arWaveInHdrBuf[i]->dwLoops = 0;

		//m_arWaveOutHdrBuf[i] = (WAVEHDR*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WAVEHDR));
		//m_arWaveOutHdrBuf[i]->lpData = (char*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, QUEUE_SIZE);
		m_arWaveOutHdrBuf[i] = new WAVEHDR{};
		m_arWaveOutHdrBuf[i]->lpData = new char[QUEUE_SIZE] {};
		m_arWaveOutHdrBuf[i]->dwBufferLength = QUEUE_SIZE;
		m_arWaveOutHdrBuf[i]->dwFlags = 0;
		m_arWaveOutHdrBuf[i]->dwLoops = 0;
	}

}

bool C_VOICE::waveInOpenDevice(HWND hWnd)
{
	MMRESULT mResult = waveInOpen(&m_hWaveIn, (UINT)WAVE_MAPPER, &m_waveFormat,
		(DWORD)hWnd, NULL, (DWORD)CALLBACK_WINDOW);
	if (mResult != MMSYSERR_NOERROR)
		return false;

	m_bWaveInStart = true;

	return true;
}

void C_VOICE::waveInVoice()
{
	for (int i = 0; i < MAX_QUEUE_COUNT; i++)
	{
		waveInPrepareHeader(m_hWaveIn, m_arWaveInHdrBuf[i], sizeof(WAVEHDR));
		waveInAddBuffer(m_hWaveIn, m_arWaveInHdrBuf[i], sizeof(WAVEHDR));
	}

	waveInStart(m_hWaveIn);
}

void C_VOICE::waveInEnd()
{
	waveInStop(m_hWaveIn);
	waveInReset(m_hWaveIn);
	for (int i = 0; i < MAX_QUEUE_COUNT; i++)
	{
		waveInUnprepareHeader(m_hWaveIn, m_arWaveInHdrBuf[i], sizeof(WAVEHDR));
	}

	waveInCloseDevice();
}

void C_VOICE::waveInReuseQueue(WAVEHDR* waveHdrBuf)
{
	waveInPrepareHeader(m_hWaveIn, waveHdrBuf, sizeof(WAVEHDR));
	waveInAddBuffer(m_hWaveIn, waveHdrBuf, sizeof(WAVEHDR));
}

void C_VOICE::waveInCloseDevice()
{
	waveInClose(m_hWaveIn);
	m_hWaveIn = nullptr;
	m_bWaveInStart = false;
}

void C_VOICE::release()
{
	for (int i = 0; i < MAX_QUEUE_COUNT; i++)
	{
		/*::HeapFree(GetProcessHeap(), 0, m_arWaveInHdrBuf[i]->lpData);
		::HeapFree(GetProcessHeap(), 0, m_arWaveInHdrBuf[i]);

		::HeapFree(GetProcessHeap(), 0, m_arWaveOutHdrBuf[i]->lpData);
		::HeapFree(GetProcessHeap(), 0, m_arWaveOutHdrBuf[i]);*/

		delete[] m_arWaveInHdrBuf[i]->lpData;
		delete m_arWaveInHdrBuf[i];

		delete[] m_arWaveOutHdrBuf[i]->lpData;
		delete m_arWaveOutHdrBuf[i];
	}
}

bool C_VOICE::waveOutOpenDevice(HWND hWnd)
{
	MMRESULT mResult = waveOutOpen(&m_hWaveOut, (UINT)WAVE_MAPPER, &m_waveFormat,
		(DWORD)hWnd, NULL, (DWORD)CALLBACK_WINDOW);
	if (mResult != MMSYSERR_NOERROR)
		return false;

	m_bWaveOutStart = true;

	return true;
}

void C_VOICE::waveOutVoice(char* pBuf, int nBufSize)
{
	memcpy(m_arWaveOutHdrBuf[m_nOutIdx]->lpData, pBuf, nBufSize);

	waveOutPrepareHeader(m_hWaveOut, m_arWaveOutHdrBuf[m_nOutIdx], sizeof(WAVEHDR));
	waveOutWrite(m_hWaveOut, m_arWaveOutHdrBuf[m_nOutIdx], sizeof(WAVEHDR));

	m_nOutIdx = (m_nOutIdx + 1) % MAX_QUEUE_COUNT;
}

void C_VOICE::waveOutEnd()
{
	MMRESULT mResult = waveOutPause(m_hWaveOut);
	if (mResult == MMSYSERR_NOERROR)
	{
		waveOutReset(m_hWaveOut);
		for (int i = 0; i < MAX_QUEUE_COUNT; i++)
		{
			waveOutUnprepareHeader(m_hWaveOut, m_arWaveOutHdrBuf[i], sizeof(WAVEHDR));
		}
	}
	waveOutClose(m_hWaveOut);
	m_hWaveOut = nullptr;
	m_nOutIdx = 0;
	m_bWaveOutStart = false;
}

bool C_VOICE::isWaveInStart()
{
	return m_bWaveInStart;
}

bool C_VOICE::isWaveOutStart()
{
	return m_bWaveOutStart;
}
