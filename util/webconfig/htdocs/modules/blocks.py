#!/usr/bin/python
def PrintHeader():
	print """Content-type: text/html


<html>
	<head>
		<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
		                      "http://www.w3.org/TR/html4/loose.dtd">
		<title>
			LinuxBIOS: Web Configuration new
		</title>
	</head>
	<body bgcolor="white">

		<table width=600 border=0>

			<tr>
				<td width=136>
					<img src="/images/linuxbioslogo.jpg" width=136 height=136 alt="The LinuxBIOS logo">
				</td>
				<td width=464 colspan=2 halign=left align=left valign=top bgcolor="#EEEEEE">
					&nbsp;<br>
					<font face=ariel,helvetica>
						&nbsp;<b>LinuxBIOS:</b><br>
						<font size=+3>
							&nbsp;&nbsp;&nbsp;Configuration
						</font>
					</font>
				</td>
			</tr>
			<tr>
				<td halign=center align=center valign=top width=136 bgcolor="#EEEEEE">
					<font face=ariel,helvetica color="#669966">
						&nbsp;<br>
						<a href="/index.html"><font size=+2 color="#FF2222">home</font></a><br>
						<a href="/bogus.html">bogus</a><br>
					</font>
				</td>
				<td width=10>
					&nbsp;&nbsp;&nbsp;<br>
				</td>
				<td width=454 halign=left align=left valign=top>
					&nbsp;<br>
"""

def PrintTrailer():
	print """
					<br>&nbsp<br>
				</td>
			</tr>
			<tr>
				<td width=136 halign=center align=center>
					<a href="http://www.linuxbios.org"><font size=-2 face=courier color="#FF2222">www.linuxbios.org</font></a>
				</td>
				<td colspan=2 halign=right align=right bgcolor="#EEEEEE">
					<a href="http://www.lanl.gov/misc/copyright.html"><font face=ariel,helvetica size=-2 color=black><b>&copy; 1999 University of California</b></font></a> |
					<a href="http://www.lanl.gov/misc/disclaimer.html"><font face=ariel,helvetica size=-2 color=black>Disclaimer</font></a></b>
				</td>
			</tr>
		</table>
	</body>
</html>
"""

def IncompleteData(CForm,OffendingItem):
	PrintHeader()
	print	"<h2><center>ERROR: Missing Data for Field %s </center></h2>" % OffendingItem
	print "<p>Dump of form data follows:</p>"
	print "<table>"
	print "<tr><th>Key</th><th>Value</th></tr>"
	for FormKey in CForm.keys():
		print "<tr><td>%s</td><td>%s</td></tr>" % (FormKey, CForm[FormKey])
	print "</table>"
	PrintTrailer()
