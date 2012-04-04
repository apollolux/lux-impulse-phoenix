#include <nall/platform.hpp>
using namespace nall;

#include <phoenix.hpp>
using namespace phoenix;

#include "nthings.hpp"
#include "formats/ittech.hpp"

struct WinITmsg : NWin {
	VerticalLayout layMain;
	TextEdit tMsg;
	WinITmsg() {
		init("Module Message");
		setBaseGeometry({256,128,400,400},false);
		layMain.setMargin(4);
		layMain.append(tMsg, {~0,~0});
		appendToWin(layMain);
		tMsg.setWordWrap(true); tMsg.setEditable(false);
	}
};
struct EnvelopeLayout : HorizontalLayout {
	VerticalLayout layInfo;
	ListView lvNodes;
	CheckBox bOn, bLoop, bSusLoop, bIsFilter;
	EnvelopeLayout() {
		bOn.setText("On"); bOn.setEnabled(false);
		bLoop.setText("Loop");  bLoop.setEnabled(false);
		bSusLoop.setText("Sus Loop"); bSusLoop.setEnabled(false);
		bIsFilter.setText("Is Filter"); bIsFilter.setEnabled(false);
		layInfo.append(bOn, {~0,0});
		layInfo.append(bLoop, {~0,0});
		layInfo.append(bSusLoop, {~0,0});
		layInfo.append(bIsFilter, {~0,0});
		append(layInfo, {~0,~0},2);
		lvNodes.setHeaderText("Y","At"); lvNodes.setHeaderVisible(true);
		append(lvNodes, {~0,~0});
	}
	void hasFilterBit(bool isFilter=true) {bIsFilter.setVisible(isFilter);}
};
struct LoopLayout : HorizontalLayout {
	//HorizontalLayout layToggle, layLoop;
	CheckBox bOn, bPP;
	TextField tStart, tEnd;
	LoopLayout() {
		bOn.setText("On"); bPP.setText("Ping-Pong");
		tStart.create("Start"); tStart.setLabelWidth(32);
		tEnd.create("End"); tEnd.setLabelWidth(32);
		append(bOn,{0,0},4); append(tStart,{~0,0},4); append(tEnd,{~0,0},4); append(bPP,{0,0});
		//layToggle.append(bOn,{~0,0},4); layToggle.append(bPP,{~0,0});
		//layLoop.append(tStart,{~0,0},4); layLoop.append(tEnd,{~0,0});
		//append(layToggle,{~0,0},4); append(layLoop,{~0,0});
	}
};
struct WinITsmp : NWin {
	VerticalLayout layMain;
	HorizontalLayout layAll;
	VerticalLayout layLeft, layRight;
	Fieldset lSmp, lInfo, lConv, lLoop, lSusLoop, lVib;
	ListView lvSmp;
	LoopLayout layLoop, laySusLoop;
	TextField tC5, tPan,
		tViS, tViD, tViR;
	ComboField sViT;
	CheckBox bOn, bB16, bStereo, bCmp;
	CheckBox bConvSigned, bConvBE, bConvDelta, bConvBDelta, bConvTX, bConvStereo;
	function<void (const unsigned &r)> updateInfo;
	WinITsmp() {
		init("Sample Data");
		setBaseGeometry({256,256,400,400},true);
		reflow();
		updateInfo = dummy;
	}
	void setFlagBits(uint8_t c) {
		bOn.setChecked(c&0x01);
		bB16.setChecked(c&0x02);
		bStereo.setChecked(c&0x04);
		bCmp.setChecked(c&0x08);
		layLoop.bOn.setChecked(c&0x10);
		laySusLoop.bOn.setChecked(c&0x20);
		layLoop.bPP.setChecked(c&0x40);
		laySusLoop.bPP.setChecked(c&0x80);
	}
	void setConversionBits(uint8_t c) {
		bConvSigned.setChecked(c&0x01);
		bConvBE.setChecked(c&0x02);
		bConvDelta.setChecked(c&0x04);
		bConvBDelta.setChecked(c&0x08);
		bConvTX.setChecked(c&0x10);
		bConvStereo.setChecked(c&0x20);
	}
	void reflow() {
		lSmp.create("Samples"); lSmp.appendIt(lvSmp);
		lvSmp.setHeaderText("ID","Offset","Name","DOS Name","Pointer"); lvSmp.setHeaderVisible(true);
		layLeft.append(lSmp, {~0,~0},4);
		lConv.create("Conversion");
		bConvSigned.setText("Signed"); lConv.appendIt(bConvSigned,{~0,0});
		bConvBE.setText("Big Endian"); lConv.appendIt(bConvBE,{~0,0});
		bConvDelta.setText("Delta Values"); lConv.appendIt(bConvDelta,{~0,0});
		bConvBDelta.setText("Byte Delta"); lConv.appendIt(bConvBDelta,{~0,0});
		bConvTX.setText("12-Bit TX-Wave"); lConv.appendIt(bConvTX,{~0,0});
		bConvStereo.setText("Channel Prompt"); lConv.appendIt(bConvStereo,{~0,0});
		layLeft.append(lConv,{~0,0});
		layAll.append(layLeft,{~0,~0},4);
		lInfo.create("Info");
		bOn.setText("Associated"); lInfo.appendIt(bOn,{~0,0});
		bB16.setText("16-Bit"); lInfo.appendIt(bB16,{~0,0});
		bStereo.setText("Stereo"); lInfo.appendIt(bStereo,{~0,0});
		bCmp.setText("Compressed"); lInfo.appendIt(bCmp,{~0,0});
		tPan.create("Def Pan"); lInfo.appendIt(tPan,{~0,0});
		layRight.append(lInfo,{~0,0},4);
		lLoop.create("Loop"); lLoop.appendIt(layLoop);
		layRight.append(lLoop,{~0,0},4);
		lSusLoop.create("Sus Loop"); lSusLoop.appendIt(laySusLoop);
		layRight.append(lSusLoop,{~0,0},4);
		lVib.create("Vibrato");// lVib.setBorder(false);
		tViS.create("Speed"); tViS.setLabelWidth(48);
		tViD.create("Depth"); tViD.setLabelWidth(48);
		sViT.create("Type"); sViT.setLabelWidth(48); sViT.reset(); sViT.field.append("Sine","Ramp down","Square","Random");
		tViR.create("Rate"); tViR.setLabelWidth(48);
		lVib.appendIt(tViS,{~0,0}); lVib.appendIt(tViD,{~0,0}); lVib.appendIt(sViT,{~0,0}); lVib.appendIt(tViR,{~0,0});
		layRight.append(lVib,{~0,0});
		layAll.append(layRight,{~0,~0});
		layMain.setMargin(4);
		layMain.append(layAll, {~0,~0});
		appendToWin(layMain);
	}
	static void dummy(const unsigned&) {}
};
struct WinITinst : NWin {
	VerticalLayout layMain;
	HorizontalLayout layAll;
	VerticalLayout layLeft, layRight;
	Fieldset lInst, lNotes;
	ListView lvInst, lvNotes;
	Fieldset lEnvVol, lEnvPan, lEnvPit;
	EnvelopeLayout layEnvVol, layEnvPan, layEnvPit;
	function<void (const unsigned &r)> updateInfo;
	WinITinst() {
		init("Instrument Data");
		setBaseGeometry({256,128,400,400},true);
		reflow();
		updateInfo = dummy;
	}
	void reflow() {
		lInst.create("Instruments"); lInst.appendIt(lvInst); lvInst.setHeaderText("ID","Offset","Name","DOS Name"); lvInst.setHeaderVisible(true);
		lNotes.create("Notes"); lNotes.appendIt(lvNotes); lvNotes.setHeaderText("Note","Sample"); lvNotes.setHeaderVisible(true);
		layLeft.append(lInst, {~0,~0}, 4);
		layLeft.append(lNotes, {~0,~0});
		layAll.append(layLeft, {~0,~0}, 4);
		lEnvVol.create("Volume Envelope"); lEnvVol.appendIt(layEnvVol); layEnvVol.hasFilterBit(false);
		lEnvPan.create("Panning Envelope"); lEnvPan.appendIt(layEnvPan); layEnvPan.hasFilterBit(false);
		lEnvPit.create("Pitch Envelope"); lEnvPit.appendIt(layEnvPit); layEnvPit.hasFilterBit(true);
		layRight.append(lEnvVol, {~0,~0}, 4);
		layRight.append(lEnvPan, {~0,~0}, 4);
		layRight.append(lEnvPit, {~0,~0});
		layAll.append(layRight, {~0,~0});
		layMain.setMargin(4);
		layMain.append(layAll, {~0,~0});
		appendToWin(layMain);
	}
	static void dummy(const unsigned&) {}
};
struct WinInfo : NWin {
	string path;
	file f;
	// MENU
	Menu mFile;
	Item mFile_ShowIns, mFile_ShowSmp, mFile_ShowMsg, mFile_Close;
	// WINDOW
	VerticalLayout layMain;
	VerticalLayout layLeft, layRight;
	HorizontalLayout layInfo, layOut;
	HorizontalLayout layHi, layVol, layTim, layOrd;
	Fieldset lOut, lInfo;
	Fieldset lPat, lOrd;
	Fieldset lHi, lVol, lTim;
	TextField tName, tVer,
		tRowHi, tRowLo,
		tVolG, tVolM,
		tInitS, tInitT;
	TextEdit tOut, tInfo, tData;
	ListView lvPat, lvOrd;
	// AUX WINDOW
	WinITinst wIns;
	WinITsmp wSmp;
	WinITmsg wMsg;
	// MODULE
	Module::ImpulseModule imp;
	// PROC
	WinInfo() {
		// MAIN
		init("File Info");
		initMenu();
		// "STATUSBAR"
		setStatusText(path);
		setStatusVisible();
		// LAYOUT
		reflow();
	}
	bool open(const string &p);
	void updateInstruments();
	void updateSamples();
	void updatePatterns();
	void reflow();
	void initMenu();
};

struct Config : configuration {
	string geoMain;
	string pathMod;
	string geoInfo;
	Config();
};
struct App : NWin {
	Config cfg;
	string basePath;
	// MENU
	Menu mFile, mHelp;
	Separator msFile;
	Item mFile_Load, mFile_Quit, mHelp_About;
	// WINDOW
	VerticalLayout layMain;
	//// AUX WINDOWS
	WinInfo wInfo;
	// FIELDS
	FileEntry inMod;
	// PROC
	App(int argc, char** argv);
	void create();
	void initConfig();
	void initMenu();
	void loadFile(const string& path);
	void loadFileIT(const string& path);
	void loadFileS3M(const string& path);
	void loadFileXM(const string& path);
	void loadFileMOD(const string& path);
};
