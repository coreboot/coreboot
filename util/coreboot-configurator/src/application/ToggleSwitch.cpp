/* SPDX-License-Identifier: GPL-2.0-only */

#include <QEvent>
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>
#include <QTimer>

#include "ToggleSwitch.h"
#include "ToggleSwitch.svg.h"

const QByteArray ToggleSwitch::s_toggleOffSvgContent =  ToggleSwitchSVG::s_toggledOffContent;
const QByteArray ToggleSwitch::s_toggleOnSvgContent = ToggleSwitchSVG::s_toggledOnContent;
const int ToggleSwitch::s_colorPosInToggleOn = ToggleSwitch::s_toggleOnSvgContent.indexOf("#1a73e8");

ToggleSwitch::ToggleSwitch(QWidget *parent) : QCheckBox(parent){

	setFixedWidth(50);
	setFixedHeight(width()/2);

	m_toggleOnSvgContentColored = s_toggleOnSvgContent;
}

void ToggleSwitch::paintEvent(QPaintEvent *event){
	QPainter p(this);

	if(isChecked()){
		auto accent = palette().highlight().color();
		m_toggleOnSvgContentColored = m_toggleOnSvgContentColored.replace(s_colorPosInToggleOn, 7, accent.name().toLatin1());

		m_svgr.load(m_toggleOnSvgContentColored);
	} else {
		m_svgr.load(s_toggleOffSvgContent);
	}

	m_svgr.render(&p, this->rect());
	p.end();
}
