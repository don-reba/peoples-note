#include <mmsystem.h>

class WaveOut
{
private:

	HWAVEOUT handle;

public:

	WaveOut
		( UINT           deviceId
		, WAVEFORMATEX * format
		, DRVCALLBACK  * callback
		, void         * callbackParameter
		, DWORD          flags
		);
	~WaveOut();

	void PrepareHeader(WAVEHDR * header);

	void Reset();

	void Write(WAVEHDR * header);

	bool UnprepareHeader(WAVEHDR * header);

private:

	void Open
		( UINT           deviceId
		, WAVEFORMATEX * format
		, DWORD          callback
		, DWORD          callbackParameter
		, DWORD          flags
		);
};