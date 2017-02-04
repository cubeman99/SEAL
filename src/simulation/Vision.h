#ifndef _VISION_H_
#define _VISION_H_


//-----------------------------------------------------------------------------
// VisionChannel
//-----------------------------------------------------------------------------
class VisionChannel
{
public:
	friend class Retina;

	VisionChannel();
	~VisionChannel();

	void Configure(unsigned int channelIndex, unsigned int resolution);

	float GetSightValue(unsigned int index) const;
	float GetInterpolatedSightValue(float t) const;
	
private:
	unsigned int	m_channelIndex;
	unsigned int	m_resolution;
	float*			m_buffer;
};


//-----------------------------------------------------------------------------
// Retina
//-----------------------------------------------------------------------------
class Retina
{
public:
	Retina();
	~Retina();

	void Configure(float fieldOfView, float viewDistance,
		unsigned int numChannels, unsigned int* channelResolutions);

	inline float GetViewDistance() const { return m_viewDistance; }
	inline float GetFieldOfView() const { return m_fieldOfView; }
	inline unsigned int GetNumChannels() const { return m_numChannels; }

	unsigned int GetResolution(unsigned int channel) const;
	float GetSightValue(unsigned int channel, unsigned int index) const;
	float GetInterpolatedSightValue(unsigned int channel, float t) const;

	void ClearSightValues();
	void SetSightValue(unsigned int channel, unsigned int index, float sightValue);

private:
	float			m_fieldOfView; // in radians.
	float			m_viewDistance;
	unsigned int	m_numChannels;
	VisionChannel*	m_channels;
};


#endif // _VISION_H_