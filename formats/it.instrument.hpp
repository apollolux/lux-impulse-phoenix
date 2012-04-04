#ifndef __IT_H_ITI__
#define __IT_H_ITI__

#include <nall/platform.hpp>
#include <nall/file.hpp>

using namespace nall;

namespace Module {
namespace Impulse {

namespace Instrument {
	string NoteStrings[12] = { "C-", "C#", "D-", "Eb", "E-", "F-", "F#", "G-", "Ab", "A-", "Bb", "B-" };
	string noteToString(uint8_t note) {
		string ret = "";
		ret.append((string){
			NoteStrings[note%12],
			(unsigned)(note/12)
		});
		return ret;
	}
	struct Node {
		int8_t y;
		uint16_t ticks;
	};
	struct Envelope {
		uint8_t flags;
		uint8_t nodes;
		uint8_t loopBegin;
		uint8_t loopEnd;
		uint8_t sustainLoopBegin;
		uint8_t sustainLoopEnd;
		vector<Node> points;	// up to 25 nodes
		uint8_t _unused;
	};
	struct NoteEntry {
		uint8_t note;	// 0..119 / C-0..B-9
		uint8_t sample;	// 1..99, or 0 = no sample
		string toString() {
			string ret = "";
			ret.append((string){
				NoteStrings[note%12],
				(unsigned)(note/12)
			});
			return ret;
		}
	};
	struct Header {
		char signature[4];
		string filename;	// 12 bytes, 12345678.123
		uint8_t _unused_1;	// should be 0
		uint8_t newNoteAction;	// 0 = cut, 1 = continue, 2 = note off, 3 = note fade
		uint8_t duplicateCheckType;	// 0 = off, 1 = note, 2 = sample, 3 = instrument
		uint8_t duplicateCheckAction;	// 0 = cut, 1 = note off, 2 = note fade
		uint16_t fadeOut;
		uint8_t pitchpanSeparation;
		uint8_t pitchpanCenter;
		uint8_t volumeGlobal;
		uint8_t pan;
		uint8_t randomVariationVolume;
		uint8_t randomVariationPan;
		uint16_t version;
		uint8_t samples;
		uint8_t _unused_2;
		string name;	// 26 bytes
		uint8_t initialFilterCutoff;
		uint8_t initialFilterResonance;
		uint8_t midiChannel;
		uint8_t midiProgram;
		uint16_t midiBank;
	};
}	// /module::impulse::instrument
struct ITInstrument {
	Instrument::Header header;
	Instrument::NoteEntry notes[120];
	Instrument::Envelope envelopeVolume;
	Instrument::Envelope envelopePan;
	Instrument::Envelope envelopePitch;
	uint8_t _unused[4];
	// PROC
	static const int SOW = sizeof(uint16_t);
	static const int SOD = sizeof(uint32_t);
	bool readHeader(file& f) {
		f.read((uint8_t*)header.signature, 4);
		if (memcmp(header.signature,"IMPI",4)==0) {
			char _fn[12]; f.read((uint8_t*)_fn, 12);
			header.filename = _fn;
			header._unused_1 = f.read();	// should be 0
			header.newNoteAction = f.read();	// 0 = cut, 1 = continue, 2 = note off, 3 = note fade
			header.duplicateCheckType = f.read();	// 0 = off, 1 = note, 2 = sample, 3 = instrument
			header.duplicateCheckAction = f.read();	// 0 = cut, 1 = note off, 2 = note fade
			header.fadeOut = f.readl(SOW);
			header.pitchpanSeparation = f.read();
			header.pitchpanCenter = f.read();
			header.volumeGlobal = f.read();
			header.pan = f.read();
			header.randomVariationVolume = f.read();
			header.randomVariationPan = f.read();
			header.version = f.readl(SOW);
			header.samples = f.read();
			header._unused_2 = f.read();
			char _name[26]; f.read((uint8_t*)_name, 26);
			header.name = _name;	// 26 bytes
			header.initialFilterCutoff = f.read();
			header.initialFilterResonance = f.read();
			header.midiChannel = f.read();
			header.midiProgram = f.read();
			header.midiBank = f.readl(SOW);
			return true;
		}
		else return false;
	}
	void readEnvelope(file& f, Instrument::Envelope &e) {
		e.flags = f.read();
		e.nodes = f.read();
		e.loopBegin = f.read();
		e.loopEnd = f.read();
		e.sustainLoopBegin = f.read();
		e.sustainLoopEnd = f.read();
		e.points.reset();
		for (unsigned i = 0; i<25; ++i) {
			Instrument::Node _n;
			_n.y = f.read();
			_n.ticks = f.readl(SOW);
			e.points.append(_n);
		}
		e._unused = f.read();
	}
	bool read(file& f) {
		if (readHeader(f)) {
			for (unsigned i = 0; i<120; ++i) {
				notes[i].note = f.read();
				notes[i].sample = f.read();
			}
			readEnvelope(f, envelopeVolume);
			readEnvelope(f, envelopePan);
			readEnvelope(f, envelopePitch);
			f.read(_unused, 4);
			return true;
		}
		else return false;
	}
};

namespace OldInstrument {
	struct Header {
		char signature[4];
		string filename;	// 12 bytes, 12345678.123
		uint8_t _unused_1;	// should be 0
		uint8_t flags;
		uint8_t volumeLoopBegin;
		uint8_t volumeLoopEnd;
		uint8_t sustainLoopBegin;
		uint8_t sustainLoopEnd;
		uint8_t _unused_2[2];
		uint16_t fadeOut;
		uint8_t newNoteAction;
		uint8_t duplicateNoteCheck;
		uint16_t version;
		uint8_t samples;
		uint8_t _unused_3;
		string name;	// 26 bytes
		uint8_t _unused_4[6];
	};
}	// /module::impulse::oldinstrument
struct ITInstrumentOld {
	OldInstrument::Header header;
	Instrument::NoteEntry notes[120];
	uint8_t envelope[200];
	uint8_t nodes[50];	// read tick THEN magnitude
};

}	// /module::impulse
}	// /module

#endif
