/* SPDX-License-Identifier: GPL-2.0-only */

#include <QProcess>
#include <QTextStream>

#include "NvramToolCli.h"
#include "Util.h"

static constexpr char s_sudoProg[] = "/usr/bin/pkexec";
static constexpr char s_nvramToolProg[] = "/usr/sbin/nvramtool";

#if MOCK

QMap<QString, QString> NvramToolCli::readParameters(QString *error) {
	return QMap<QString,QString>({
		{"boot_option","Normal"},
		{"reboot_counter","0x0"},
		{"debug_level","Spew"},
		{"vtd","Enable"},
		{"power_profile","Performance"},
		{"wireless","Enable"},
		{"webcam","Enable"},
		{"microphone","Enable"},
		{"legacy_8254_timer","Enable"},
		{"usb_always_on","Disable"},
		{"kbl_timeout","Never"},
		{"fn_ctrl_swap","Enable"},
		{"max_charge","100%"},
		{"power_on_after_fail","Disable"},
		{"fn_lock_state","0x2"},
		{"trackpad_state","0x40"},
		{"kbl_brightness","0xc4"},
		{"kbl_state","0x22"}
	});
}

QStringList NvramToolCli::readOptions(const QString &parameter, QString *error){
	return (parameter=="power_profile")?
	QStringList{
		"Power Saver","Balanced","Performance"
	} : QStringList{};
}

#else

QMap<QString, QString> NvramToolCli::readParameters(QString *error)
{
	QProcess nvramtoolProcess;
	nvramtoolProcess.start(s_sudoProg, {s_nvramToolProg, "-a"});

	nvramtoolProcess.waitForFinished();

	if(error) *error = nvramtoolProcess.readAllStandardError();

	if(nvramtoolProcess.exitCode() != 0){
		return {};
	}

	return Util::parseParameters(nvramtoolProcess);
}

QStringList NvramToolCli::readOptions(const QString &parameter, QString *error)
{
	QStringList result;

	QProcess nvramtoolProcess;
	nvramtoolProcess.start(s_sudoProg, {s_nvramToolProg, "-e", parameter});
	nvramtoolProcess.waitForFinished();

	if(error) *error = nvramtoolProcess.readAllStandardError();

	while (nvramtoolProcess.canReadLine()) {
		result.append(nvramtoolProcess.readLine().trimmed());
	}

	return result;
}
#endif

bool NvramToolCli::writeParameters(const QMap<QString, QString> &parameters, QString *error)
{

#if MOCK
	QTextStream outStream(stdout);
#else
	QProcess nvramtoolProcess;
	nvramtoolProcess.start(s_sudoProg, {s_nvramToolProg, "-i"});
	nvramtoolProcess.waitForStarted();
	QTextStream outStream(&nvramtoolProcess);
#endif
	for(auto it = parameters.begin(); it != parameters.end(); ++it){
		outStream << it.key() << " = " << it.value() << "\n";
	}

	outStream.flush();
#if MOCK
	return true;
#else
	nvramtoolProcess.closeWriteChannel();
	nvramtoolProcess.waitForFinished();

	if(error){
		*error = nvramtoolProcess.readAllStandardError();
	}

	return nvramtoolProcess.exitCode()==0;
#endif
}



QString NvramToolCli::version()
{
	QProcess nvramtoolProcess;
	nvramtoolProcess.start(s_nvramToolProg, {"-v"});

	nvramtoolProcess.waitForFinished();

	return nvramtoolProcess.readAll();
}
