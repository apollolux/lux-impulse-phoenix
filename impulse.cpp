#include "impulse.hpp"

void WinInfo::reflow() {
	layMain.setMargin(4);
	lOut.create("Header"); lOut.appendIt(layOut);
	tName.create("Module Name");
	tVer.create("Tracker Version"); tVer.field.setEditable(false);
	tRowHi.create("Major"); tRowHi.setLabelWidth(32);
	tRowLo.create("Minor"); tRowLo.setLabelWidth(32);
	tVolG.create("Global"); tVolG.setLabelWidth(40);
	tVolM.create("Mixing"); tVolM.setLabelWidth(40);
	tInitS.create("Speed"); tInitS.setLabelWidth(32);
	tInitT.create("Tempo"); tInitT.setLabelWidth(32);
	layLeft.append(tName,{~0,0});
	layLeft.append(tVer,{~0,0});
	//layLeft.append(tOut,{~0,~0});
	tOut.setWordWrap(true); tOut.setEditable(false);
	lHi.create("Row Highlight"); lHi.setBorder(false);
	lHi.appendIt(layHi); layHi.append(tRowHi,{~0,0},4); layHi.append(tRowLo,{~0,0});
	lVol.create("Volume"); lVol.setBorder(false);
	lVol.appendIt(layVol); layVol.append(tVolG,{~0,0},4); layVol.append(tVolM,{~0,0});
	lTim.create("Init. Timing"); lTim.setBorder(false);
	lTim.appendIt(layTim); layTim.append(tInitT,{~0,0},4); layTim.append(tInitS,{~0,0});
	//layRight.append(tData,{~0,~0}); tData.setWordWrap(true); tData.setEditable(false);
	layOut.setAlignment(0.0);
	layOut.append(layLeft,{~0,~0},4);
	layOut.append(lHi,{128,0},4);
	layOut.append(lVol,{144,0},4);
	layOut.append(lTim,{128,0},4);
	layRight.append(tOut,{~0,~0},4);
	layOut.append(layRight,{~0,~0});
	layMain.append(lOut, {~0,0}, 4);
	layMain.append(layInfo, {~0,~0});
	lPat.create("Patterns"); lPat.appendIt(lvPat); lvPat.setHeaderText("ID","Offset","Length","Rows"); lvPat.setHeaderVisible(true);
	lOrd.create("Order"); lOrd.appendIt(lvOrd); lvOrd.setHeaderText("###"); lvOrd.setHeaderVisible(true);
	layOrd.append(lOrd, {48,~0},4);
	layOrd.append(lPat, {~0,~0});
	layInfo.append(layOrd, {~0,~0});
	appendToWin(layMain);
}
void WinInfo::initMenu() {
	mFile.setText("File");
	mFile_ShowIns.setText("Instruments");
	mFile_ShowSmp.setText("Samples");
	mFile_ShowMsg.setText("Message");
	//mFile_Close.setText("Close");
	append(mFile);
		mFile.append(mFile_ShowIns);
		mFile.append(mFile_ShowSmp);
		mFile.append(mFile_ShowMsg);
		//mFile.append(mFile_Close);
	setMenuVisible();
	mFile_ShowSmp.onActivate = [&]() {
		//MessageWindow::information(*this, "TODO: show ins window");
		wSmp.setVisible();
	};
	mFile_ShowIns.onActivate = [&]() {
		//MessageWindow::information(*this, "TODO: show ins window");
		wIns.setVisible();
	};
	mFile_ShowMsg.onActivate = [&]() {
		wMsg.setVisible();
		//MessageWindow::information(*this, "TODO: show msg window");
	};
}
bool WinInfo::open(const string& p) {
	if (file::exists(p)) {
		path = p;
		setStatusText(path);
		if (f.open(path, file::mode::read)==true) {
			unsigned size = f.size();
			if (imp.load(f)) {
				string _it = "", _id = "", _im = "";
				tName.setValue(imp.header.name);
				tVer.setValue(imp.version());
				_it.append((string){
					//"Song Name: ",imp.header.name,"\n",
					//"Tracker Version: ",imp.version()," (",imp.minVersion()," compat)","\n",
					"Tracker Compatibility: ",imp.minVersion(),"\n",
					//"# Orders: ",imp.header.orders,"\n",
					//"# Instruments: ",imp.header.instruments,"\n",
					//"# Samples: ",imp.header.samples,"\n",
					//"# Patterns: ",imp.header.patterns,"\n"
					""
				});
				tRowHi.setValue(imp.header.highlightMajor);
				tRowLo.setValue(imp.header.highlightMinor);
				tVolG.setValue(imp.header.volumeGlobal);
				tVolM.setValue(imp.header.volumeMix);
				tInitS.setValue(imp.header.initSpeed);
				tInitT.setValue(imp.header.initTempo);
				_id.append((string){""
					//"Row Highlight: ",imp.header.highlightMinor," / ",imp.header.highlightMajor,"\n",
					//"Volume - ",
					//	"Global: ",imp.header.volumeGlobal,", ",
					//	"Mix: ",imp.header.volumeMix,"\n",
					//"Initial Speed/Tempo: ",imp.header.initSpeed," / ",imp.header.initTempo,"\n"
				});
				tOut.setText(_it);
				tData.setText(_id);
				if (imp.header.special&1) {	// msg attached
					_im.append(imp.message);
				}
				if (imp.header.special&2) ;	// reserved
				if (imp.header.special&4) ;	// reserved
				if (imp.header.special&8) {	// midi cfg embedded
				}
				updateInstruments();
				updateSamples();
				updatePatterns();
				wMsg.tMsg.setText(_im);
			}
		}
		else {
			//MessageWindow::warning(*this, {"Couldn't load module ",path});
			return false;
		}
	}
	else {
		//MessageWindow::warning(*this, {"Couldn't load ",path});
		return false;
	}
	return true;
}
void WinInfo::updatePatterns() {
	lPat.setText((string){"Patterns",imp.header.patterns>0?(string){" (",imp.header.patterns,")"}:""});
	//lOrd.setText((string){"Order",imp.header.orders>0?(string){" (",imp.header.orders,")"}:""});
	lvPat.reset();
	for (unsigned i = 0; i<imp.offsetsPatterns.size(); ++i)
		lvPat.append(i+1, imp.offsetsPatterns[i], imp.patterns[i].header.length, imp.patterns[i].header.rows);
	lvPat.autoSizeColumns();
	lvOrd.reset();
	for (auto o : imp.patternOrder) {
		lvOrd.append(
			o==255?"---":
			(o==254?"+++":
				(o<200?(string){o}:"xxx"))
		);
	}
	lvOrd.autoSizeColumns();
}
void WinInfo::updateSamples() {
	wSmp.lSmp.setText((string){"Samples",imp.header.samples>0?(string){" (",imp.header.samples,")"}:""});
	wSmp.lvSmp.reset();
	for (unsigned i = 0; i<imp.offsetsSamples.size(); ++i)
		wSmp.lvSmp.append(i+1, imp.offsetsSamples[i], imp.samples[i].header.name, imp.samples[i].header.filename, imp.samples[i].header.offset);
	wSmp.lvSmp.autoSizeColumns();
	wSmp.updateInfo = [&](const unsigned &r) {
		//MessageWindow::information(wSmp, (string){"TODO: update sample info ",r});
		Module::Impulse::ITSample &smp = imp.samples[r];
		wSmp.tC5.setValue(smp.header.middleC);
		wSmp.setFlagBits(smp.header.flags);
		wSmp.setConversionBits(smp.header.conversion);
		wSmp.layLoop.tStart.setValue(smp.header.loopBegin);
		wSmp.layLoop.tEnd.setValue(smp.header.loopEnd);
		wSmp.laySusLoop.tStart.setValue(smp.header.sustainLoopBegin);
		wSmp.laySusLoop.tEnd.setValue(smp.header.sustainLoopEnd);
		wSmp.tViS.setValue(smp.header.vibratoSpeed);
		wSmp.tViD.setValue(smp.header.vibratoDepth);
		wSmp.tViR.setValue(smp.header.vibratoRate);
		wSmp.sViT.field.setSelection(smp.header.vibratoType);
	};
	wSmp.lvSmp.onActivate = wSmp.lvSmp.onChange = [&]() {
		wSmp.updateInfo(wSmp.lvSmp.selection());
	};
}
void WinInfo::updateInstruments() {
	wIns.lInst.setText((string){"Instruments",imp.header.instruments>0?(string){" (",imp.header.instruments,")"}:""});
	wIns.lvInst.reset();
	for (unsigned i = 0; i<imp.offsetsInstruments.size(); ++i)
		wIns.lvInst.append(i+1, imp.offsetsInstruments[i], imp.instruments[i].header.name, imp.instruments[i].header.filename);
	wIns.lvInst.autoSizeColumns();
	wIns.updateInfo = [&](const unsigned &r) {
		//MessageWindow::information(*this, (string){"TODO: update notes ",r});
		unsigned n;
		Module::Impulse::ITInstrument &inst = imp.instruments[r];
		wIns.lvNotes.reset();
		for (n = 0; n<120; ++n)
			wIns.lvNotes.append(
				inst.notes[n].toString(),
				inst.notes[n].sample>0?(string)inst.notes[n].sample:"--"
			);
		wIns.lvNotes.autoSizeColumns();
		wIns.layEnvVol.lvNodes.reset();
		wIns.layEnvVol.bOn.setChecked(inst.envelopeVolume.flags&1);
		wIns.layEnvVol.bLoop.setChecked(inst.envelopeVolume.flags&2);
		wIns.layEnvVol.bSusLoop.setChecked(inst.envelopeVolume.flags&4);
		wIns.layEnvVol.bIsFilter.setChecked(inst.envelopeVolume.flags&128);
		wIns.layEnvPan.lvNodes.reset();
		wIns.layEnvPan.bOn.setChecked(inst.envelopePan.flags&1);
		wIns.layEnvPan.bLoop.setChecked(inst.envelopePan.flags&2);
		wIns.layEnvPan.bSusLoop.setChecked(inst.envelopePan.flags&4);
		wIns.layEnvPan.bIsFilter.setChecked(inst.envelopePan.flags&128);
		wIns.layEnvPit.lvNodes.reset();
		wIns.layEnvPit.bOn.setChecked(inst.envelopePitch.flags&1);
		wIns.layEnvPit.bLoop.setChecked(inst.envelopePitch.flags&2);
		wIns.layEnvPit.bSusLoop.setChecked(inst.envelopePitch.flags&4);
		wIns.layEnvPit.bIsFilter.setChecked(inst.envelopePitch.flags&128);
		for (n = 0; n<25; ++n) {
			wIns.layEnvVol.lvNodes.append(
				inst.envelopeVolume.points[n].y,
				inst.envelopeVolume.points[n].ticks
			);
			wIns.layEnvPan.lvNodes.append(
				inst.envelopePan.points[n].y,
				inst.envelopePan.points[n].ticks
			);
			wIns.layEnvPit.lvNodes.append(
				inst.envelopePitch.points[n].y,
				inst.envelopePitch.points[n].ticks
			);
		}
		wIns.layEnvVol.lvNodes.autoSizeColumns();
		wIns.layEnvPan.lvNodes.autoSizeColumns();
		wIns.layEnvPit.lvNodes.autoSizeColumns();
	};
	wIns.lvInst.onActivate = wIns.lvInst.onChange = [&]() {
		wIns.updateInfo(wIns.lvInst.selection());
	};
}
Config::Config() {
	attach(geoMain="+128,+128,512,256", "geoMain");
	attach(pathMod="", "pathMod");
	attach(geoInfo="+512,+128,512,400", "geoInfo");
}
App::App(int argc, char** argv) {
	initConfig();
	// MAIN
	init("Impulse Reader (Phoenix)");
	setBaseGeometry(cfg.geoMain, false);
	// MENU STUFF
	initMenu();
	// "STATUSBAR"
	setStatusText({title," by lux"});
	setStatusVisible();
	// MAIN PANEL
	layMain.setMargin(4);
	layMain.append(inMod, {~0, 0});
	inMod.setLabel("Music Module");
	/*string filt = {
		"Impulse Tracker files (*.it)","\t",
		"Scream Tracker files (*.s3m)","\t",
		"Fast Tracker 2 files (*.xm)","\t",
		"Amiga Module files (*.mod)","\t",
		"All files (*)"
	};*/
	string filt = "Music Module files (*.it, *.s3m, *.xm, *.mod)";
	inMod.setFilter(filt);
	inMod.setOnLoad({&App::loadFile, this});
	inMod.setBasePath(cfg.pathMod);
	// WINDOW
	appendToWin(layMain);
	// PROC
	mFile_Load.onActivate = inMod.bBrowse.onActivate;
	mHelp_About.onActivate = [&]() { MessageWindow::information(*this, {title,"\n(c)2012 Apollolux Digital Designs"}); };
	wInfo.onClose = [&]() {
		cfg.geoInfo = wInfo.geometry().text();
	};
	onClose = mFile_Quit.onActivate = [&]() {
		cfg.geoMain = geometry().text();
		cfg.geoInfo = wInfo.geometry().text();
		cfg.save((string){basePath, "settings.cfg"});
		OS::quit();
	};
}
void App::initConfig() {
	basePath = userpath();
	#if !defined(PHOENIX_WINDOWS)
	basePath.append(".config/");
	#endif
	basePath.append("impulse/");
	mkdir(basePath,0777);
	cfg.load((string){basePath, "settings.cfg"});
	cfg.save((string){basePath, "settings.cfg"});
	//print("Loaded: ", basePath, "settings.cfg\n");
	//print("geoMap: ",cfg.geoMap,"\n");
	//print("pathMap: ",cfg.pathMap,"\n");
}
void App::initMenu() {
	mFile.setText("File");
	mFile_Load.setText("Open...");
	mFile_Quit.setText("Quit");
	mHelp.setText("Help");
	mHelp_About.setText("About...");
	append(mFile);
		mFile.append(mFile_Load);
		mFile.append(msFile);
		mFile.append(mFile_Quit);
	append(mHelp);
		mHelp.append(mHelp_About);
	setMenuVisible();
}
void App::loadFile(const string &path) {
	if (path.length()) {
		//MessageWindow::information(*this, {"TODO: load ",path});
		string ext = extension(path);
		if (ext=="it") loadFileIT(path);
		else if (ext=="s3m") loadFileS3M(path);
		else if (ext=="xm") loadFileXM(path);
		else if (ext=="mod") loadFileMOD(path);
		else MessageWindow::warning(*this, {"Couldn't load as music module ",path});
	}
}
void App::loadFileIT(const string& path) {
	//MessageWindow::information(*this, {"TODO: load as it ",path});
	cfg.pathMod = dir(path);
	if (wInfo.open(path)) {
		wInfo.setBaseGeometry(cfg.geoInfo, true);
		wInfo.setVisible();
	}
	else MessageWindow::warning(*this, {"Couldn't load as IT ",path});
}
void App::loadFileS3M(const string& path) {
	MessageWindow::information(*this, {"TODO: load as s3m ",path});
}
void App::loadFileXM(const string& path) {
	MessageWindow::information(*this, {"TODO: load as xm ",path});
}
void App::loadFileMOD(const string& path) {
	MessageWindow::information(*this, {"TODO: load as mod ",path});
}
void App::create() {
	setVisible(true);
}

int main(int argc, char** argv) {
	App app(argc, argv);
	app.create();
	OS::main();
	return 0;
}