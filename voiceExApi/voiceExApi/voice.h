#pragma once

class C_VOICE
{
private:
	enum
	{
		MAX_QUEUE_COUNT = 3,
		QUEUE_SIZE = 4800,
	};
private:
	HWAVEIN			m_hWaveIn;
	WAVEFORMATEX	m_waveFormat;
	WAVEHDR*		m_arWaveHdrBuf[MAX_QUEUE_COUNT];

	HWAVEOUT		m_hWaveOut;
//	WAVEFORMATEX	m_waveOutFormat;
	WAVEHDR*		m_arWaveOutHdrBuf[MAX_QUEUE_COUNT];

//	DWORD			m_dwVoiceLen;
	bool			m_bEndRecord;
	int				m_nOutIdx;
//	BYTE*			m_pSoundBuf[QUEUE_SIZE * 10];

public:
	C_VOICE();
	void init();
	bool openDevice(HWND hWnd);
	void startRecord();
	void stopRecord();
	void reuseQueue(WAVEHDR* waveHdrBuf);
	void closeDevice();
	void release();

	bool openOutDevice(HWND hWnd);
	void waveOutVoice(char* pBuf, int nBufSize);
	void waveOutEnd();

	bool isStart();
};
