#include "Vision.h"
#include <assert.h>
#include <math/MathLib.h>
#include <string.h>


//-----------------------------------------------------------------------------
// VisionChannel
//-----------------------------------------------------------------------------
	
VisionChannel::VisionChannel() :
	m_colorBuffer(nullptr),
	m_depthBuffer(nullptr),
	m_resolution(0),
	m_channelIndex(0)
{
}

VisionChannel::~VisionChannel()
{
	delete [] m_colorBuffer;
	m_colorBuffer = nullptr;
	delete [] m_depthBuffer;
	m_depthBuffer = nullptr;
}

void VisionChannel::Configure(unsigned int channelIndex, unsigned int resolution)
{
	delete [] m_colorBuffer;
	delete [] m_depthBuffer;
	m_resolution = resolution;
	m_channelIndex = channelIndex;
	m_colorBuffer = new float[m_resolution];
	m_depthBuffer = new float[m_resolution];
}

void VisionChannel::Clear()
{
	for (unsigned int i = 0; i < m_resolution; ++i)
	{
		m_colorBuffer[i] = 0.0f; // Clear sight value to 0 (black).
		m_depthBuffer[i] = 1.0f; // Clear depth to max depth.
	}
}
	
float VisionChannel::GetSightValue(unsigned int index) const
{
	assert(index < m_resolution);
	return m_colorBuffer[index];
}

float VisionChannel::GetInterpolatedSightValue(float t) const
{
	if (m_resolution == 0)
		return 0.0f;
	else if (m_resolution == 1)
		return m_colorBuffer[0];
	
	t = Math::Clamp(t, 0.0f, 1.0f);

	float neuronIndex = (t * m_resolution) - 0.5f;
	int index0 = Math::Max((int) neuronIndex + 0, 0);
	int index1 = Math::Min((int) neuronIndex + 1, (int) m_resolution - 1);
	float lerpFactor = neuronIndex - (float) index0;

	return Math::Lerp(GetSightValue(index0), GetSightValue(index1), lerpFactor);
}


//-----------------------------------------------------------------------------
// Retina
//-----------------------------------------------------------------------------

Retina::Retina() :
	m_viewDistance(1.0f),
	m_numChannels(3),
	m_fieldOfView(0.8f)
{
	m_channels = new VisionChannel[m_numChannels];
}

Retina::~Retina()
{
	delete [] m_channels;
	m_channels = nullptr;
}

void Retina::Configure(float fieldOfView, float viewDistance,
		unsigned int numChannels, unsigned int* channelResolutions)
{
	m_fieldOfView = fieldOfView;
	m_viewDistance = viewDistance;

	// Reallocate the channels buffer if need-be.
	if (m_channels == nullptr || m_numChannels != numChannels)
	{
		delete m_channels;
		m_numChannels = numChannels;
		m_channels = new VisionChannel[m_numChannels];
	}

	// Configure each channels.
	for (unsigned int i = 0; i < m_numChannels; ++i)
		m_channels[i].Configure(i, channelResolutions[i]);
}

unsigned int Retina::GetResolution(unsigned int channel) const
{
	assert(channel < m_numChannels);
	return m_channels[channel].m_resolution;
}

float Retina::GetSightValueAtIndex(unsigned int channel, unsigned int index) const
{
	assert(channel < m_numChannels);
	return m_channels[channel].GetSightValue(index);
}

float Retina::GetSightValue(unsigned int channel, float t) const
{
	assert(channel < m_numChannels);
	int index = (int) (m_channels[channel].m_resolution * t);
	index = Math::Clamp(index, 0, (int) m_channels[channel].m_resolution - 1);
	return m_channels[channel].GetSightValue((unsigned int) index);
}

float Retina::GetInterpolatedSightValue(unsigned int channel, float t) const
{
	assert(channel < m_numChannels);
	return m_channels[channel].GetInterpolatedSightValue(t);
}

void Retina::ClearSightValues()
{
	// Clear each channel.
	for (unsigned int i = 0; i < m_numChannels; ++i)
		m_channels[i].Clear();
}

void Retina::SetSightValue(unsigned int channel, unsigned int index, float sightValue, float depth)
{
	assert(channel < m_numChannels);
	assert(index < m_channels[channel].m_resolution);

	// Perform a depth test.
	if (depth < m_channels[channel].m_depthBuffer[index])
	{
		m_channels[channel].m_colorBuffer[index] = sightValue;
		m_channels[channel].m_depthBuffer[index] = depth;
	}
}
