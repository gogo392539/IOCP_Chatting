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
	WAVEHDR*		m_arWaveOutHdrBuf[MAX_QUEUE_COUNT];

	bool			m_bWaveInStart;
	bool			m_bWaveOutStart;
	int				m_nOutIdx;

public:
	C_VOICE();
	void init();
	bool openInDevice(HWND hWnd);
	void waveInVoice();
	void waveInEnd();
	void reuseQueue(WAVEHDR* waveHdrBuf);
	void closeDevice();
	void release();

	bool openOutDevice(HWND hWnd);
	void waveOutVoice(char* pBuf, int nBufSize);
	void waveOutEnd();

	bool isWaveInStart();
	bool isWaveOutStart();
};
