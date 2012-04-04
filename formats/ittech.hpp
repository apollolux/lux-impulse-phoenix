#ifndef __IT_H__
#define __IT_H__

#include <nall/platform.hpp>
#include <nall/file.hpp>
//#include <phoenix.hpp>

#include "it.pattern.hpp"
#include "it.sample.hpp"
#include "it.instrument.hpp"
#include "it.module.hpp"

using namespace nall;

namespace Module {
namespace Impulse {
}	// /module::impulse

struct ImpulseModule {
	Impulse::Header header;
	vector<uint8_t> patternOrder;	// length = orders
	vector<Impulse::ITInstrument> instruments;
	vector<Impulse::ITInstrumentOld> old_instruments;
	vector<Impulse::ITSample> samples;
	vector<Impulse::ITPattern> patterns;
	vector<uint32_t> offsetsInstruments;
	vector<uint32_t> offsetsSamples;
	vector<uint32_t> offsetsPatterns;
	string message;
	static const int SOW = sizeof(uint16_t);
	static const int SOD = sizeof(uint32_t);
	bool load(file& f) {
		message = "";
		if (readHeader(f)) {
			readPatternOrder(f);
			readOffsets(f);
			uint32_t off = f.offset();
			if (header.special&1) {	// has message
				char _msg[header.msgLength];
				f.seek(header.msgOffset);
				f.read((uint8_t*)_msg, header.msgLength);
				message = string((const char*)_msg).replace("\r","\n");
				f.seek(off);
			}
			readInstruments(f);	// load instruments
			readSamples(f);	// load samples
			readPatterns(f);	// load patterns
			return true;
		}
		else return false;
	}
	string minVersion() {
		string _v = "";
		_v.append((string){
			(uint8_t)((header.minVersion>>8)&0xff),
			".",hex(header.minVersion&0xff)
		});
		return _v;
	}
	string version() {
		string _v = "";
		_v.append((string){
			Impulse::VersionStrings[(uint8_t)((header.version>>16)&0xf)],
			(uint8_t)((header.version>>8)&0xf),
			".",hex(header.version&0xff)
		});
		return _v;
	}
	bool readHeader(file& f) {
		f.read((uint8_t*)header.signature, 4);
		if (memcmp(header.signature,"IMPM",4)==0) {
			char _name[26]; f.read((uint8_t*)_name, 26);
			header.name = _name;
			//header.rowHighlight = f.readl(SOW);
			header.highlightMinor = f.read();
			header.highlightMajor = f.read();
			header.orders = f.readl(SOW);
			header.instruments = f.readl(SOW);
			header.samples = f.readl(SOW);
			header.patterns = f.readl(SOW);
			header.version = f.readl(SOW);
			header.minVersion = f.readl(SOW);
			header.flags = f.readl(SOW);
			header.special = f.readl(SOW);
			header.volumeGlobal = f.read();
			header.volumeMix = f.read();
			header.initSpeed = f.read();
			header.initTempo = f.read();
			header.panSeparation = f.read();
			header.pitchWheelDepth = f.read();
			header.msgLength = f.readl(SOW);
			header.msgOffset = f.readl(SOD);
			f.read(header.reserved, 4);
			f.read(header.pan, 64);
			f.read(header.volume, 64);
			if (f.offset()!=0xc0) print("Something's up! Current offset = ",f.offset(),"\n");
			return true;
		}
		else return false;
	}
	void readOffsets(file& f) {
		unsigned i;
		offsetsInstruments.reset();
		for (i = 0; i<header.instruments; ++i)
			offsetsInstruments.append(f.readl(SOD));
		offsetsSamples.reset();
		for (i = 0; i<header.samples; ++i)
			offsetsSamples.append(f.readl(SOD));
		offsetsPatterns.reset();
		for (i = 0; i<header.patterns; ++i)
			offsetsPatterns.append(f.readl(SOD));
	}
	void readPatternOrder(file& f) {
		patternOrder.reset();
		for (unsigned i = 0; i<header.orders; ++i)
			patternOrder.append(f.read());
	}
	void readInstruments(file& f) {
		uint32_t off = f.offset();
		if (header.minVersion<0x200) _readInstrumentsOld(f);
		else _readInstruments(f);
		f.seek(off);
	}
	void _readInstruments(file& f) {
		instruments.reset();
		for (auto i : offsetsInstruments) {
			f.seek(i);
			instruments.append(_readInstrument(f));
		}
	}
	Impulse::ITInstrument _readInstrument(file& f) {
		Impulse::ITInstrument _iti;
		_iti.read(f);
		return _iti;
	}
	void _readInstrumentsOld(file& f) {
		for (auto i : offsetsInstruments) {
			f.seek(i);
			_readInstrumentOld(f);
		}
	}
	void _readInstrumentOld(file& f) {
		// TODO!!! read old instrument
	}
	void readSamples(file& f) {
		uint32_t off = f.offset();
		samples.reset();
		for (auto i : offsetsSamples) {
			f.seek(i);
			samples.append(_readSample(f));
		}
		f.seek(off);
	}
	Impulse::ITSample _readSample(file& f) {
		Impulse::ITSample _its;
		_its.read(f);
		return _its;
	}
	void readPatterns(file& f) {
		uint32_t off = f.offset();
		patterns.reset();
		for (auto i : offsetsPatterns) {
			f.seek(i);
			patterns.append(_readPattern(f));
		}
		f.seek(off);
	}
	Impulse::ITPattern _readPattern(file& f) {
		Impulse::ITPattern _pat;
		_pat.read(f);
		return _pat;
	}
};

}	// /module

#endif