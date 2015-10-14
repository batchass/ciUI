/********************************************************************************** 
 
 Copyright (C) 2012 Syed Reza Ali (www.syedrezaali.com)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **********************************************************************************/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void guiEvent(ciUIEvent *event);

	ciUITextInput *host;
	ciUITextInput *port;
	ciUILabel *status;
	ColorA backgroundColor;
	ciUICanvas *gui;
	ciUIMovingGraph *mvg;

	map<int, int> controlValues;
	std::vector< ciUISlider * > sliders;
	std::vector< ciUIRotarySlider * > rotarys;
	std::vector< ciUILabelToggle * > togglesUp;
	std::vector< ciUILabelToggle * > togglesMid;
	std::vector< ciUILabelToggle * > togglesDown;
	std::string					destinationHost;
	int							destinationPort;
};

void _TBOX_PREFIX_App::setup() {
	gui = new ciUICanvas(0, 0, getWindowWidth(), getWindowHeight());
	gui->setTheme(CI_UI_THEME_RUSTICORANGE);

	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0;
	}

	for (int i = 0; i < 8; i++)
	{
		stringstream rotaryName;
		rotaryName << i + 11;
		rotarys.push_back(new ciUIRotarySlider(dim*2.5, 0.0, 127.0, controlValues[i + 11], rotaryName.str())); //0x0B = 11
		if (i == 0)
		{
			gui->addWidgetSouthOf(rotarys[i], "IP");
		}
		else
		{
			gui->addWidgetEastOf(rotarys[i], rotarys[i - 1]->getName());
		}

		stringstream toggleName0;
		toggleName0 << i + 21;
		togglesUp.push_back(new ciUILabelToggle(dim, controlValues[i + 21], toggleName0.str(), CI_UI_FONT_MEDIUM, dim));//0x15 = 21
		gui->addWidgetSouthOf(togglesUp[i], rotarys[i]->getName());

		stringstream toggleName1;
		toggleName1 << i + 31;
		togglesMid.push_back(new ciUILabelToggle(dim, controlValues[i + 31], toggleName1.str(), CI_UI_FONT_MEDIUM, dim));// 0x1F = 31
		gui->addWidgetSouthOf(togglesMid[i], togglesUp[i]->getName());

		stringstream toggleName2;
		toggleName2 << i + 41;
		togglesDown.push_back(new ciUILabelToggle(dim, controlValues[i + 41], toggleName2.str(), CI_UI_FONT_MEDIUM, dim));// 0x29 = 41
		gui->addWidgetSouthOf(togglesDown[i], togglesMid[i]->getName());

		stringstream sliderName;
		sliderName << i + 1;
		sliders.push_back(new ciUISlider(dim, length / 3.0 - CI_UI_GLOBAL_WIDGET_SPACING, 0.0, 127.0, controlValues[i + 1], sliderName.str())); //0x01
		gui->addWidgetEastOf(sliders[i], togglesUp[i]->getName());
	}
	// no midi in for pad
	float padWidth = 256;
	float padHeight = 192;
	gui->addWidgetDown(new ciUI2DPad(padWidth, padHeight, vec2(0, 1024), vec2(0, 768), vec2(padWidth*.5, padHeight*.5), "pad"));
	// LaunchPad
	((ciUIToggleMatrix *)gui->addWidgetRight(new ciUIToggleMatrix(dim, dim, 4, 5, "LaunchPad")))->setAllowMultiple(false);
	// ip text gui->addWidgetRight(new ciUITextInput(160, "IP", "Input Text", CI_UI_FONT_LARGE, 30.0f)); 
	host = new ciUITextInput(120, "ip", destinationHost, CI_UI_FONT_SMALL, 40.0f);
	host->setAutoClear(false);
	gui->addWidgetRight(host);
	// port
	port = new ciUITextInput(80, "port", "9999", CI_UI_FONT_SMALL, 40.0f);
	port->setAutoClear(false);
	gui->addWidgetRight(port);
	// cnx btn
	gui->addWidgetRight(new ciUILabelButton(80, false, "connect", CI_UI_FONT_SMALL, 40.0f));
	// audio
	gui->addWidgetSouthOf(new ciUILabel("audio", CI_UI_FONT_SMALL), "ip");
	gui->addWidgetSouthOf(new ciUIWaveform(240, 30, mData, 1024, -1.0, 1.0, "waveform"), "audio");
	/*gui->addWidgetDown(new ciUISpectrum(240, 30, mData, 1024, -1.0, 1.0, "SPECTRUM"));  */
	gui->addWidgetSouthOf(new ciUILabel("fps", CI_UI_FONT_SMALL), "waveform");

	mvg = (ciUIMovingGraph *)gui->addWidgetSouthOf(new ciUIMovingGraph(240, 30, mbuffer, 1024, 0, 120, "fpsmvg"), "fps");

	// status
	status = new ciUILabel("status", CI_UI_FONT_SMALL);
	gui->addWidgetSouthOf(status, "fpsmvg");

	gui->registerUIEvents(this, &CiuiProjectApp::guiEvent);
}
void _TBOX_PREFIX_App::update()
{
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear(Color(0, 0, 0));
}

CINDER_APP(_TBOX_PREFIX_App, RendererGl)
