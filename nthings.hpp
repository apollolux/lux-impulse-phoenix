#ifndef __N_THINGS__
#define __N_THINGS__

#include <nall/platform.hpp>
#include <phoenix.hpp>

#include "ncanvas.hpp"

using namespace nall;
using namespace phoenix;

struct FrameBorderTop : HorizontalLayout {
	Label label;
	NCanvas _ctl, _ctr, _cbl, _cbr;
	VerticalLayout _cl, _cr;
	bool hasBorder;
	void setText(const string &l) { label.setText(l); }
	void create(const string &s="Label", uint32_t col=0xc0c0c0c0) {
		setText(s);
		_ctl.setSize({4,1}); _ctl.reset(col);
		_cbl.setSize({1,3}); _cbl.reset(col);
		_ctr.setSize({2048,1}); _ctr.reset(col);
		_cbr.setSize({1,3}); _cbr.reset(col);
		setBorder(true);
	}
	void setBorder(const bool &b=true) {hasBorder = b; reflow();}
	void reflow() {
		_cl.setAlignment(0.0); _cr.setAlignment(1.0);
		_cl.append(_ctl, {4,1}); _cl.append(_cbl, {1,3});
		_cr.append(_ctr, {~0,1}); _cr.append(_cbr, {1,3});
		setAlignment(1.0);
		remove(_cl); remove(label); remove(_cr);
		if (hasBorder) {
			append(_cl, {4,4}, 2);
			append(label, {0,0}, 2);
			append(_cr, {~0,4});
		}
		else append(label, {~0,0});
	}
};
struct Fieldset : VerticalLayout {
	NCanvas _cl, _cr, _cz;
	FrameBorderTop _lt;
	HorizontalLayout _lm;
	VerticalLayout _lv;
	Widget _pt, _pb;
	bool hasBorder;
	void setText(const string &l) { _lt.setText(l); }
	void create(const string &s="Label") {
		uint32_t col = 0xc0c0c0c0;
		_lt.create(s,col);
		uint32_t *dp;
		unsigned i, j;
		_cl.setSize({1, 1024}); _cl.reset(col);
		_cr.setSize({1, 1024}); _cr.reset(col);
		_cz.setSize({2048, 1}); _cz.reset(col);
		setBorder(true);
	}
	void setBorder(const bool &b=true) {
		hasBorder = b;
		_lt.setBorder(b);
		reflow();
	}
	void reflow() {
		remove(_lt); remove(_lm); remove(_cz);
		_lm.remove(_cl); _lm.remove(_lv); _lm.remove(_cr);
		if (hasBorder) {
			_lm.append(_cl, {1,~0}, 3);
			_lm.append(_lv, {~0,~0}, 3);
			_lm.append(_cr, {1,~0});
		}
		else _lm.append(_lv,{~0,~0});
		append(_lt, {~0,0});
		append(_lm, {~0,~0});
		if (hasBorder) append(_cz, {~0,1});
	}
	void appendIt(Sizable &s) { appendIt(s,{~0,~0},0); }
	void appendIt(Sizable& s, const Size& z) { appendIt(s, z, 0); }
	void appendIt(Sizable& s, const Size& z, const int& p) { _lv.append(s, z, p); _lv.append(_pb, {~0, 3}); }
};

struct LabeledField : HorizontalLayout {
	Label label;
	uint32_t labelWidth;
	void setLabel(const string &l) {label.setText(l);}
	void setLabelWidth(const uint32_t &w=0) {labelWidth = w; reflow();}
	virtual void reflow() = 0;
};
struct ComboField : LabeledField {
	ComboBox field;
	void appendValue(const string &l) { field.append(l); }
	void create(const string &s="Label", const string &v="") {
		setLabel(s);
		setLabelWidth(0);
		//setAlignment(0.5);
		//field.setWordWrap(false);
	}
	void appendIt(const string &v) {field.append(v);}
	void reset() {field.reset();}
	unsigned selection() {return field.selection();}
	void reflow() {
		remove(label); remove(field);
		append(label, {labelWidth, 0}, 2);
		append(field, {~0, 0});
	}
};
struct TextField : LabeledField {
	LineEdit field;
	void setText(const string &l) { setLabel(l); setValue(l); }
	void setValue(const string &l) { field.setText(l); }
	string text() { return field.text(); }
	void create(const string &s="Label", const string &v="") {
		setLabel(s);
		setValue(v);
		setLabelWidth(0);
		//setAlignment(0.5);
		//field.setWordWrap(false);
	}
	void setLabelWidth(const uint32_t &w=0) {labelWidth = w; reflow();}
	void reflow() {
		remove(label); remove(field);
		append(label, {labelWidth, 0}, 2);
		append(field, {~0, 0});
	}
	//Geometry minimumGeometry() {}
};

struct FileEntry : HorizontalLayout {
	Label label;
	LineEdit input;
	Button bBrowse;
	Button bClear;
	string filter;
	string basePath;
	function<void (const string&)> cb;
	FileEntry() : HorizontalLayout() {
		basePath = "";
		// layout
		init("Label");
		reflow();
		// proc
		cb = dummy;
		bBrowse.onActivate = [this]() {
			//MessageWindow::information(Window::None, "TODO: browse for file");
			string _lastdir = dir(getPath())||basePath;
			string _file;
			_file = DialogWindow::fileOpen(*window(), _lastdir, filter);
			if (_file.length()) {
				setPath(_file);
				cb(getPath());
			}
			//cb(getPath());	// uncomment to run on cancelled browse
		};
		bClear.onActivate = [this]() {
			input.setText("");
			cb("");
		};
	}
	void init(const string &l, const string &b="Browse...", const string &c="Clear") {
		setLabel(l);
		bBrowse.setText(b);
		bClear.setText(c);
	}
	void reflow() {
		input.setEditable(false);
		append(label, {0, 0}, 4);
		append(input, {~0, 0}, 4);
		append(bClear, {0, 0}, 2);
		append(bBrowse, {0, 0});
	}
	void setLabel(const string &l) { label.setText(l); }
	void setPath(const string &p) { input.setText(p.length() > 0 ? p : " "); }
	string getPath() { return input.text(); }
	void setBasePath(const string &p="") { basePath = p; }
	void setFilter(const string &filt) { filter = filt; }
	void setOnLoad(const function<void (const string&)>& _cb) { cb = _cb; }
	static void dummy(const string&) {}
};

struct NWin : public Window {
	string title;
	Geometry G;
	VerticalLayout layout;
	NWin() : Window() { init("Window"); }
	NWin(const string& t) : Window() { init(t); }
	void init(const string& t="Window") {
		retitle(t);
		//layout.setMargin(4);
		append(layout);
	}
	void retitle(const string& t) { title = t; setTitle(t); }
	void setBaseGeometry(const Geometry& g, bool canResize=true) { G = g; setGeometry(g); setResizable(canResize); }
	void appendToWin(Sizable& s) { appendToWin(s, {~0, ~0}, 0); }
	void appendToWin(Sizable& s, const Size& z) { appendToWin(s, z, 0); }
	void appendToWin(Sizable& s, const Size& z, const int& p) { layout.append(s, z, p); }
};



#endif
