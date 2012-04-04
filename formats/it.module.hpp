#ifndef __IT_H_IMP__
#define __IT_H_IMP__

#include <nall/platform.hpp>
#include <nall/file.hpp>

using namespace nall;

namespace Module {
namespace Impulse {
	enum Version : unsigned {
		VER_IMPULSE,
		VER_SCHISM,
		VER_UNK_2,
		VER_UNK_3,
		VER_UNK_4,
		VER_OPENMPT
	};
	string VersionStrings[6] = {
		"Impulse Tracker*",
		"Schism Tracker",
		"UNUSED",
		"UNUSED",
		"UNUSED",
		"OpenMPT"
	};
	struct Header {
		char signature[4];
		string name;	// 26 bytes
		uint8_t highlightMinor;	// broken down from uint16_t rowHighlight
		uint8_t highlightMajor;
		uint16_t orders;
		uint16_t instruments;
		uint16_t samples;
		uint16_t patterns;
		uint16_t version;
		uint16_t minVersion;
		uint16_t flags;
		uint16_t special;
		uint8_t volumeGlobal;
		uint8_t volumeMix;
		uint8_t initSpeed;
		uint8_t initTempo;
		uint8_t panSeparation;
		uint8_t pitchWheelDepth;
		uint16_t msgLength;
		uint32_t msgOffset;
		uint8_t reserved[4];
		uint8_t pan[64];
		uint8_t volume[64];
	};
}	// /module::impulse
}	// /module

#endif
