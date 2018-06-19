#include "stdafx.h"

C_VOICE::C_VOICE() : 
	m_hWaveIn(nullptr),
	m_waveFormat{},
	m_arWaveHdrBuf{},
	m_bWaveInStart(false),
	m_bWaveOutStart(false),
	m_hWaveOut(nullptr),
	m_nOutIdx(0),
	m_arWaveOutHdrBuf{}
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
		m_arWaveHdrBuf[i] = (WAVEHDR*)::HeapAlloc(GetProcessHeap(), 0, sizeof(WAVEHDR));
		m_arWaveHdrBuf[i]->lpData = (char*)::HeapAlloc(GetProcessHeap(), 0, QUEUE_SIZE);
		m_arWaveHdrBuf[i]->dwBufferLength = QUEUE_SIZE;
		m_arWaveHdrBuf[i]->dwFlags = 0;
		m_arWaveHdrBuf[i]->dwLoops = 0;

		m_arWaveOutHdrBuf[i] = (WAVEHDR*)::HeapAlloc(GetProcessHeap(), 0, sizeof(WAVEHDR));
		m_arWaveOutHdrBuf[i]->lpData = (char*)::HeapAlloc(GetProcessHeap(), 0, QUEUE_SIZE);
		m_arWaveOutHdrBuf[i]->dwBufferLength = QUEUE_SIZE;
		m_arWaveOutHdrBuf[i]->dwFlags = 0;
		m_arWaveOutHdrBuf[i]->dwLoops = 0;
	}
	
}

bool C_VOICE::openInDevice(HWND hWnd)
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
		waveInPrepareHeader(m_hWaveIn, m_arWaveHdrBuf[i], sizeof(WAVEHDR));
		waveInAddBuffer(m_hWaveIn, m_arWaveHdrBuf[i], sizeof(WAVEHDR));
	}

	waveInStart(m_hWaveIn);
}

void C_VOICE::waveInEnd()
{
	waveInStop(m_hWaveIn);
	waveInReset(m_hWaveIn);
	for (int i = 0; i < MAX_QUEUE_COUNT; i++)
	{
		waveInUnprepareHeader(m_hWaveIn, m_arWaveHdrBuf[i], sizeof(WAVEHDR));
	}
	
	closeDevice();
}

void C_VOICE::reuseQueue(WAVEHDR* waveHdrBuf)
{
	waveInPrepareHeader(m_hWaveIn, waveHdrBuf, sizeof(WAVEHDR));
	waveInAddBuffer(m_hWaveIn, waveHdrBuf, sizeof(WAVEHDR));
}

void C_VOICE::closeDevice()
{
	waveInClose(m_hWaveIn);
	m_hWaveIn = nullptr;
	m_bWaveInStart = true;
}

void C_VOICE::release()
{
	for (int i = 0; i < MAX_QUEUE_COUNT; i++)
	{
		::HeapFree(GetProcessHeap(), 0, m_arWaveHdrBuf[i]->lpData);
		::HeapFree(GetProcessHeap(), 0, m_arWaveHdrBuf[i]);

		::HeapFree(GetProcessHeap(), 0, m_arWaveOutHdrBuf[i]->lpData);
		::HeapFree(GetProcessHeap(), 0, m_arWaveOutHdrBuf[i]);
	}
}

bool C_VOICE::openOutDevice(HWND hWnd)
{
	MMRESULT mResult = waveOutOpen(&m_hWaveOut, (UINT)WAVE_MAPPER, &m_waveFormat,
		(DWORD)hWnd, NULL, (DWORD)CALLBACK_WINDOW);
	if (mResult != MMSYSERR_NOERROR)
		return false;

	m_bWaveOutStart = true;

	return false;
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
