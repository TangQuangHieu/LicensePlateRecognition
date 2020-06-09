#pragma once
#define _CALCULATE_COMPUTATION_TIME

//! Macro to construct the fourcc code of the codec. Same as CV_FOURCC()
#define CV_FOURCC_MACRO(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))

/** @brief Constructs the fourcc code of the codec function

Simply call it with 4 chars fourcc code like `CV_FOURCC('I', 'Y', 'U', 'V')`

List of codes can be obtained at [Video Codecs by FOURCC](http://www.fourcc.org/codecs.php) page.
FFMPEG backend with MP4 container natively uses other values as fourcc code:
see [ObjectType](http://www.mp4ra.org/codecs.html).
*/
inline int CV_FOURCC(char c1, char c2, char c3, char c4)
{
	return CV_FOURCC_MACRO(c1, c2, c3, c4);
}

inline float round(float fNum, int iNumOfRound)
{
	float fValue = (int)(fNum * pow(10, iNumOfRound)+0.5);
	return (float)fValue / pow(10, iNumOfRound);
}