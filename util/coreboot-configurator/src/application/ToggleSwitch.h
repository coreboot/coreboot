/* SPDX-License-Identifier: GPL-2.0-only */

#pragma once

#include <QCheckBox>
#include <QFile>
#include <QObject>
#include <QSvgRenderer>


/*
 * The ToggleSwitch class represents Toggle Switch widget based on QCheckBox and toggles svg with colorscheme support
 */
class ToggleSwitch : public QCheckBox {
	Q_OBJECT
public:
	explicit ToggleSwitch(QWidget* parent = nullptr);

private:
	QSvgRenderer m_svgr;

	static const QByteArray s_toggleOnSvgContent;
	static const QByteArray s_toggleOffSvgContent;
	static const int s_colorPosInToggleOn;

	QByteArray m_toggleOnSvgContentColored;

	/* QWidget interface */
protected:
	void paintEvent(QPaintEvent *event) override;

	/* QAbstractButton interface */
protected:
	bool hitButton(const QPoint &pos) const override
	{
		/* needs to be clickable on */
		return rect().contains(pos);
	}
};
