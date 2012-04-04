#ifndef __IT_H_PAT__
#define __IT_H_PAT__

#include <nall/platform.hpp>
#include <nall/file.hpp>

using namespace nall;

namespace Module {
namespace Impulse {

namespace Pattern {
	struct Header {
		uint16_t length;	// in bytes
		uint16_t rows;	// 32..200
		uint8_t _unused[4];
	};
}	// /module::impulse::pattern
struct ITPattern {
	Pattern::Header header;
	vector<uint8_t> data;	// packed, see GetNextChannelMarker
	// PROC
	static const int SOW = sizeof(uint16_t);
	static const int SOD = sizeof(uint32_t);
	void readHeader(file& f) {
		header.length = f.readl(SOW);
		header.rows = f.readl(SOW);
		f.read(header._unused, 4);
	}
	bool read(file& f) {
		readHeader(f);
		uint16_t row = 0, ch, i = 0;
		uint8_t cv, mv;
		uint8_t _c, _i, _v, _f, _p;
		uint8_t _mask[64] = {0};
		uint8_t _pd[header.length]; f.read(_pd, header.length);
		while (row<header.rows) {
			cv = _pd[i++];
			if (cv==0) {
				++row;
				// note += 64;
				continue;
			}
			ch = (cv-1)&63;
			if (cv&128) {
				mv = _pd[i++];
				_mask[ch] = mv;
			}
			else {
				mv = _mask[ch];
			}
			if (mv&1) {	// read note
				_c = _pd[i++];
				// TODO!!! read note
			}
			if (mv&2) {	// read inst
				_i = _pd[i++];
				// TODO!!! read inst
			}
			if (mv&4) {	// read vol/pan
				_v = _pd[i++];
				// TODO!!! read vol/pan
			}
			if (mv&8) {	// read command
				_f = _pd[i++]&0x1f;
				_p = _pd[i++];
				// TODO!!! read command
			}
			if (mv&16) {	// note = channel's last note
			}
			if (mv&32) {	// inst = channel's last inst
			}
			if (mv&64) {	// vol/pan = channel's last vol/pan
			}
			if (mv&128) {	// command = channel's last command
			}
		}
	}
};

}	// /module::impulse
}	// /module

#endif
