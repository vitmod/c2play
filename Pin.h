#pragma once

#include "Codec.h"
#include "PacketBuffer.h"


#include <memory>


//
// Element.Inputs[]
// Element.Outputs[]

enum class PinDirection
{
	Out = 0,
	In
};

enum class MediaCategory
{
	Unknown = 0,
	Audio,
	Video,
	Subtitle,
	Clock
};

enum class VideoStreamType
{
	Unknown = 0,
	Mpeg2,
	Mpeg4,
	Avc,
	VC1,
	Hevc
};

enum class AudioStreamType
{
	Unknown = 0,
	Pcm,
	Mpeg2Layer3,
	Ac3,
	Aac,
	Dts,
	WmaPro,
	DolbyTrueHD
};

enum class SubtitleStreamType
{
	Unknown = 0,
	Text
};


class PinInfo
{
	MediaCategory category;

public:

	MediaCategory Category() const
	{
		return category;
	}


	PinInfo(MediaCategory category)
		: category(category)
	{
	}
};
typedef std::shared_ptr<PinInfo> PinInfoSPTR;


typedef std::vector<unsigned char> ExtraData;
typedef std::shared_ptr<std::vector<unsigned char>> ExtraDataSPTR;
class VideoPinInfo : public PinInfo
{
public:
	VideoPinInfo()
		: PinInfo(MediaCategory::Video)
	{
	}

	VideoStreamType StreamType = VideoStreamType::Unknown;
	int Width;
	int Height;
	double FrameRate = 0;
	ExtraDataSPTR ExtraData;
};
typedef std::shared_ptr<VideoPinInfo> VideoPinInfoSPTR;


class AudioPinInfo : public PinInfo
{

public:
	AudioPinInfo()
		: PinInfo(MediaCategory::Audio)
	{
	}


	AudioStreamType StreamType = AudioStreamType::Unknown;
	int Channels = 0;
	int SampleRate = 0;


};
typedef std::shared_ptr<AudioPinInfo> AudioPinInfoSPTR;

class SubtitlePinInfo : public PinInfo
{
public:

	SubtitlePinInfo()
		: PinInfo(MediaCategory::Subtitle)
	{
	}


	SubtitleStreamType StreamType = SubtitleStreamType::Unknown;
};
typedef std::shared_ptr<SubtitlePinInfo> SubtitlePinInfoSPTR;


class Pin : public std::enable_shared_from_this<Pin>
{
	PinDirection direction;
	ElementWPTR owner;
	PinInfoSPTR info;
	std::string name;


protected:
	Pin(PinDirection direction, ElementWPTR owner, PinInfoSPTR info)
		: Pin(direction, owner, info, "Pin")
	{
	}
	Pin(PinDirection direction, ElementWPTR owner, PinInfoSPTR info, std::string name)
		: direction(direction), owner(owner), info(info), name(name)
	{
		if (owner.expired())
			throw ArgumentNullException();

		if (!info)
			throw ArgumentNullException();
	}


public:

	PinDirection Direction() const
	{
		return direction;
	}

	ElementWPTR Owner() const
	{
		return owner;
	}

	PinInfoSPTR Info() const
	{
		return info;
	}

	std::string Name() const
	{
		return name;
	}
	void SetName(std::string value)
	{
		name = value;
	}



	virtual ~Pin()
	{
	}


	virtual void Flush()
	{
	}
};


template <typename T>	// where T : PinSPTR
class PinCollection
{
	friend class Element;

	std::vector<T> pins;


protected:

	void Add(T value)
	{
		if (!value)
			throw ArgumentNullException();

		pins.push_back(value);
	}

	void Clear()
	{
		pins.clear();
	}


public:

	PinCollection()
	{
	}


	int Count() const
	{
		return pins.size();
	}

	T Item(int index)
	{
		if (index < 0 || index > pins.size())
			throw ArgumentOutOfRangeException();

		return pins[index];
	}

	void Flush()
	{
		for (auto pin : pins)
		{
			pin->Flush();
		}
	}

	T FindFirst(MediaCategory category)
	{
		for (auto item : pins)
		{
			if (item->Info()->Category() == category)
			{
				return item;
			}
		}

		return nullptr;
	}
};