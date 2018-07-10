#pragma once

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
	WAVEFORMATEX	m_waveFormat;
	HWAVEIN			m_hWaveIn;
	WAVEHDR*		m_arWaveInHdrBuf[MAX_QUEUE_COUNT];
	HWAVEOUT		m_hWaveOut;
	WAVEHDR*		m_arWaveOutHdrBuf[MAX_QUEUE_COUNT];

	bool			m_bWaveInStart;
	bool			m_bWaveOutStart;
	int				m_nOutIdx;

public:
	C_VOICE();
	void init();
	void release();

	bool waveInOpenDevice(HWND hWnd);
	void waveInVoice();
	void waveInEnd();
	void waveInReuseQueue(WAVEHDR* waveHdrBuf);
	void waveInCloseDevice();

	bool waveOutOpenDevice(HWND hWnd);
	void waveOutVoice(char* pBuf, int nBufSize);
	void waveOutEnd();

	bool isWaveInStart();
	bool isWaveOutStart();
};

