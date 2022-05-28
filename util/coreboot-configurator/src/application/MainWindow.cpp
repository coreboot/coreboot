/* SPDX-License-Identifier: GPL-2.0-only */

#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QShortcut>
#include <QtGui>

#include "AboutDialog.h"
#include "Configuration.h"
#include "MainWindow.h"
#include "NvramToolCli.h"
#include "ToggleSwitch.h"
#include "ui_MainWindow.h"

static auto s_errorWindowTitle = MainWindow::tr("Error Occurred");
static auto s_nvramErrorMessage = MainWindow::tr("Nvramtool was not able to access cmos settings. Look at documentation for possible causes of errors.");

QString makeNvramErrorMessage(const QString& error){
	if(!error.trimmed().isEmpty()){
		return QString(MainWindow::tr("%1<br><br>Error message:<br><tt>%2</tt>")).arg(s_nvramErrorMessage,
											Qt::convertFromPlainText(error));
	}
	return s_nvramErrorMessage;
}

namespace YAML {
template <>
struct convert<QString>{
	static Node encode(const QString& rhs) { return Node(rhs.toUtf8().data()); }

	static bool decode(const Node& node, QString& rhs) {
		if (!node.IsScalar())
			return false;
		rhs = QString::fromStdString(node.Scalar());
		return true;
	}
};
}

static auto s_metadataErrorMessage =  MainWindow::tr("Can't load categories metadata file. Check your installation.");
static constexpr char s_sudoProg[] = "/usr/bin/pkexec";

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->actionAbout, &QAction::triggered, this, [](){
		AboutDialog().exec();
	});

#if MOCK
	this->setWindowTitle("coreboot configurator "+tr("[MOCKED DATA]"));
#else
	this->setWindowTitle("coreboot configurator");
#endif
	this->setWindowIcon(QIcon::fromTheme("coreboot_configurator"));

	QFile catFile(":/config/categories.yaml");

	if(!catFile.open(QFile::ReadOnly)){
		QMessageBox::critical(this, s_errorWindowTitle, s_metadataErrorMessage);
		this->close();
		return;
	}

	m_categories = YAML::Load(catFile.readAll());

	if(m_categories.IsNull() || !m_categories.IsDefined()){
		QMessageBox::critical(this, s_errorWindowTitle, s_metadataErrorMessage);
		this->close();
		return;
	}

	QShortcut* returnAction = new QShortcut(QKeySequence("Ctrl+Return"), this);
	connect(returnAction, &QShortcut::activated, this, &MainWindow::on_saveButton_clicked);

	generateUi();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::pullSettings()
{
	QString error;
	m_parameters = NvramToolCli::readParameters(&error);

	if(m_parameters.isEmpty()){
		QMessageBox::critical(this, s_errorWindowTitle, makeNvramErrorMessage(error));

		/* we need delayed close as initialization error happened before event loop start so we can't stop application properly */
		QTimer::singleShot(0, this, &MainWindow::close);
	}
}

void MainWindow::pushSettings()
{
	QString error;
	if(!NvramToolCli::writeParameters(m_parameters, &error)){
		QMessageBox::critical(this, s_errorWindowTitle, makeNvramErrorMessage(error));
	}
}


QComboBox* MainWindow::createComboBox(const QString& key) {
	auto box = new QComboBox(this);

	auto opts = NvramToolCli::readOptions(key);

	box->addItems(opts);
	box->setCurrentText(m_parameters[key]);

	connect(ui->advancedModeCheckBox, &QCheckBox::clicked, this, [box](bool clicked){
		box->setEditable(clicked);
	});

	connect(this, &MainWindow::updateValue, this, [box, this, key](const QString& name){
		if(key!=name || m_parameters[name]==box->currentText()){
			return;
		}
		box->setCurrentText(m_parameters[name]);
	});

	connect(box, &QComboBox::currentTextChanged, this, [key, this](const QString& value){
		if(value==m_parameters[key]){
			return;
		}
		m_parameters[key] = value;
		emit updateValue(key);
	});

	return box;
}
QString boolToString(bool value){
	return value?QStringLiteral("Enable"):QStringLiteral("Disable");
}
bool stringToBool(const QString& str){
	return str==QStringLiteral("Enable");
}
QCheckBox* MainWindow::createCheckBox(const QString& key) {
	auto box = new ToggleSwitch(this);

	box->setChecked(stringToBool(m_parameters[key]));

	connect(this, &MainWindow::updateValue, this, [box, this, key](const QString& name){

		if(key!=name || m_parameters[name]==boolToString(box->isChecked())){
			return;
		}
		auto newValue = stringToBool(m_parameters[name]);

		box->setChecked(newValue);
	});

	connect(box, &QCheckBox::clicked, this, [key, this](bool checked){
		auto value = boolToString(checked);
		if(value==m_parameters[key]){
			return;
		}
		m_parameters[key] = value;
		emit updateValue(key);
		});

	return box;
}


QTableWidget *MainWindow::createRawTable()
{
	/* Create Raw values table */
	auto table = new QTableWidget(m_parameters.size(), 2);
	table->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
	table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
	table->verticalHeader()->hide();
	table->setSelectionBehavior(QTableWidget::SelectRows);

	connect(table, &QTableWidget::cellChanged, this, [table, this](int row, int column){
		if(column != 1 || row >= table->rowCount() || row < 0 ){
			/* Weird state when changed cell is not a value cell */
			return;
		}
		auto keyItem = table->item(row, 0);
		auto valueItem = table->item(row, 1);

		if(keyItem == nullptr || valueItem == nullptr){
			/* Invalid cells */
			return;
		}

		if(valueItem->text()==m_parameters[keyItem->text()]){
			return;
		}

		m_parameters[keyItem->text()] = valueItem->text();
		emit updateValue(keyItem->text());
	});

	auto it = m_parameters.begin();
	for(int i = 0; i<m_parameters.size(); i++, ++it){

		auto item = new QTableWidgetItem(it.key());
		item->setFlags(item->flags() ^ Qt::ItemIsEditable);
		table->setItem(i,0,item);

		item = new QTableWidgetItem(it.value());
		connect(this, &MainWindow::updateValue, this, [item, it, this](const QString& name){
			if(it.key()!=name || m_parameters[name]==item->text()){
				return;
			}
			item->setText(m_parameters[name]);
		});

		table->setItem(i,1,item);
	}
	return table;
}

void MainWindow::generateUi()
{
	pullSettings();

	if(!m_categories.IsMap()){
		return;
	}
	for(const auto& category : m_categories){
		if(!category.second.IsMap()){
			continue;
		}
		auto name = category.second["displayName"].as<QString>();

		auto layout = new QVBoxLayout;

		auto tabPage = new QWidget(this);
		tabPage->setLayout(layout);

		ui->centralTabWidget->addTab(tabPage, name);

		for(const auto& value : category.second){
			if(!value.second.IsMap() || !m_parameters.contains(value.first.as<QString>())){
				continue;
			}
			auto displayName = value.second["displayName"];
			if(!displayName.IsDefined()){
				continue;
			}
			auto type = value.second["type"];
			if(!type.IsDefined()){
				continue;
			}

			auto controlLayout = new QHBoxLayout();

			auto help = value.second["help"];

			if(help.IsDefined()){
				auto labelWithTooltip = new QWidget;
				labelWithTooltip->setToolTip(help.as<QString>());
				labelWithTooltip->setCursor({Qt::WhatsThisCursor});
				labelWithTooltip->setLayout(new QHBoxLayout);

				auto helpButton = new QLabel();
				helpButton->setPixmap(QIcon::fromTheme("help-hint").pixmap(16,16));

				{
					auto layout = qobject_cast<QHBoxLayout*>(labelWithTooltip->layout());
					layout->addWidget(new QLabel(displayName.as<QString>()));
					layout->addWidget(helpButton,1);
				}
				controlLayout->addWidget(labelWithTooltip, 0);
			} else {
				controlLayout->addWidget(new QLabel(displayName.as<QString>()), 0);
			}

			controlLayout->addStretch(1);

			QWidget* res = nullptr;

			if(type.as<QString>() == QStringLiteral("bool")){
				res = createCheckBox(value.first.as<QString>());
			} else if (type.as<QString>() == QStringLiteral("enum")){
				res = createComboBox(value.first.as<QString>());
			} else {
				controlLayout->deleteLater();
				continue;
			}
		res->setObjectName(value.first.as<QString>());

		controlLayout->addWidget(res, 0);

		layout->addLayout(controlLayout);
		}
	}

	auto table = createRawTable();

	connect(ui->advancedModeCheckBox, &QCheckBox::clicked, this, [table,this](bool clicked){
		if(clicked && ui->centralTabWidget->widget(ui->centralTabWidget->count()-1) != table){
			ui->centralTabWidget->addTab(table, tr("Raw"));
		} else if(!clicked && ui->centralTabWidget->widget(ui->centralTabWidget->count()-1) == table) {
			ui->centralTabWidget->removeTab(ui->centralTabWidget->count()-1);
		}
	});
}

void MainWindow::askForReboot()
{
	QMessageBox rebootDialog(QMessageBox::Question,
				 tr("Reboot"),
				 tr("Changes are saved. Do you want to reboot to apply changes?"));

	auto nowButton = rebootDialog.addButton(tr("Reboot now"), QMessageBox::AcceptRole);
	rebootDialog.addButton(tr("Reboot later"), QMessageBox::RejectRole);

	rebootDialog.exec();
	if(rebootDialog.clickedButton()==nowButton){
		QProcess::startDetached(s_sudoProg, {"/usr/bin/systemctl", "reboot"});
		this->close();
	}
}

void MainWindow::readSettings(const QString &fileName)
{
	if(fileName.isEmpty()){
		return;
	}

	auto configValues = Configuration::fromFile(fileName);

	for(auto it = configValues.begin(); it != configValues.end(); ++it){
		if(!m_parameters.contains(it.key())){
			continue;
		}
		m_parameters[it.key()]=it.value();
		emit updateValue(it.key());
	}
}

void MainWindow::writeSettings(const QString &fileName)
{
	if(fileName.isEmpty()){
		return;
	}
	if(!Configuration::toFile(fileName, m_parameters)){
		QMessageBox::critical(this, tr("Error Occurred"), tr("Can't open file to write"));
		this->close();
	}
}


void MainWindow::on_actionSave_triggered()
{
	auto filename = QFileDialog::getSaveFileName(this,
						     tr("Select File To Save"),
						     QDir::homePath(),
						     tr("Coreboot Configuration Files")+"(*.cfg)");
	writeSettings(filename);
}


void MainWindow::on_actionLoad_triggered()
{
	auto filename = QFileDialog::getOpenFileName(this,
						     tr("Select File To Load"),
						     QDir::homePath(),
						     tr("Coreboot Configuration Files")+"(*.cfg)");

	readSettings(filename);
}


void MainWindow::on_saveButton_clicked()
{
	ui->centralwidget->setEnabled(false);
	ui->menubar->setEnabled(false);

	pushSettings();

	askForReboot();

	ui->centralwidget->setEnabled(true);
	ui->menubar->setEnabled(true);
}
