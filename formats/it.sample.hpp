#ifndef __IT_H_ITS__
#define __IT_H_ITS__

#include <nall/platform.hpp>
#include <nall/file.hpp>

using namespace nall;

namespace Module {
namespace Impulse {

namespace Sample {
	struct Header {
		char signature[4];
		string filename;	// 12 bytes, 12345678.123
		uint8_t _unused;	// should be 0
		uint8_t volumeGlobal;
		uint8_t flags;
		uint8_t volume;
		string name;	// 26 bytes
		uint8_t conversion;
		uint8_t pan;
		uint32_t length;	// in frames
		uint32_t loopBegin;	// in frames
		uint32_t loopEnd;	// in frames, actually the end+1
		uint32_t middleC;	// samplerate in Hz for C5
		uint32_t sustainLoopBegin;	// in frames
		uint32_t sustainLoopEnd;	// in frames, 1 /after/ sustain end
		uint32_t offset;	// sample offset in file
		uint8_t vibratoSpeed;
		uint8_t vibratoDepth;
		uint8_t vibratoRate;
		uint8_t vibratoType;	// 0 = sine, 1 = ramp down, 2 = square, 3 = random
	};
}	// /module::impulse::sample
struct ITSample {
	Sample::Header header;
	// PROC
	static const int SOW = sizeof(uint16_t);
	static const int SOD = sizeof(uint32_t);
	bool readHeader(file& f) {
		f.read((uint8_t*)header.signature, 4);
		if (memcmp(header.signature,"IMPS",4)==0) {
			char _fn[12]; f.read((uint8_t*)_fn, 12);
			header.filename = _fn;
			header._unused = f.read();	// should be 0
			header.volumeGlobal = f.read();
			header.flags = f.read();
			header.volume = f.read();
			char _name[26]; f.read((uint8_t*)_name, 26);
			header.name = _name;	// 26 bytes
			header.conversion = f.read();
			header.pan = f.read();
			header.length = f.readl(SOD);
			header.loopBegin = f.readl(SOD);
			header.loopEnd = f.readl(SOD);
			header.middleC = f.readl(SOD);
			header.sustainLoopBegin = f.readl(SOD);
			header.sustainLoopEnd = f.readl(SOD);
			header.offset = f.readl(SOD);
			header.vibratoSpeed = f.read();
			header.vibratoDepth = f.read();
			header.vibratoRate = f.read();
			header.vibratoType = f.read();
			return true;
		}
		else return false;
	}
	bool read(file& f) {
		if (readHeader(f)) {
			//// TODO!!! load sample data into struct
			return true;
		}
		else return false;
	}
};

}	// /module::impulse
}	// /module

#endif
