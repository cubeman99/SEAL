#ifndef _VISION_H_
#define _VISION_H_

#include <math/Matrix4f.h>


//-----------------------------------------------------------------------------
// VisionChannel - a single color channel for a vision strip.
//-----------------------------------------------------------------------------
class VisionChannel
{
public:
	friend class Retina;

	VisionChannel();
	~VisionChannel();

	void Configure(unsigned int channelIndex, unsigned int resolution);

	void Clear();
	float GetSightValue(unsigned int index) const;
	float GetInterpolatedSightValue(float t) const;
	
private:
	unsigned int	m_channelIndex;
	unsigned int	m_resolution;
	float*			m_colorBuffer; // color values for each pixel
	float*			m_depthBuffer; // depth values for each pixel. 0 = close, 1 = far.
};


//-----------------------------------------------------------------------------
// Retina - An eyeball that stores a strip of color data, and is positioned
//          on an agent's body.
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
	float GetSightValue(unsigned int channel, float t) const;
	float GetSightValueAtIndex(unsigned int channel, unsigned int index) const;
	float GetInterpolatedSightValue(unsigned int channel, float t) const;

	void ClearSightValues();
	void SetSightValue(unsigned int channel, unsigned int index, float sightValue, float depth);

	inline const Matrix4f& GetWorldToEye() const { return m_worldToEye; }
	inline const Matrix4f& GetEyeToProjection() const { return m_eyeToProjection; }
	inline void SetWorldToEye(const Matrix4f& worldToEye) { m_worldToEye = worldToEye; }
	inline void SetEyeToProjection(const Matrix4f& eyeToProjection) { m_eyeToProjection = eyeToProjection; }

private:
	float			m_fieldOfView; // in radians.
	float			m_viewDistance;
	unsigned int	m_numChannels;
	VisionChannel*	m_channels;
	Matrix4f		m_worldToEye; // Converts points from world-space to eye-space.
	Matrix4f		m_eyeToProjection; // Converts points from eye-space to eye-projection-space.
};


#endif // _VISION_H_