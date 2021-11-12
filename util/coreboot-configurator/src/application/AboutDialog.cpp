/* SPDX-License-Identifier: GPL-2.0-only */

#include "AboutDialog.h"
#include "NvramToolCli.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->logoLabel->setPixmap(QPixmap(":/images/star.svg"));

	ui->versionLabel->setText("<tt>"+NvramToolCli::version()+"</tt>");
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
