/* SPDX-License-Identifier: GPL-2.0-only */

#pragma once

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AboutDialog(QWidget *parent = nullptr);
	~AboutDialog();

private:
	Ui::AboutDialog *ui;
};
