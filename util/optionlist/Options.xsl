<?xml version="1.0"?>
<!--
  Copyright (C) 2005 Florian Zeitz <florian-zeitz@lycos.de>
  Copyright (C) 2005 Stefan Reinauer <stepan@coresystems.de>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--<xsl:choose>
<xsl:when test="system-property('xsl:vendor')='Transformiix'">
-->
<xsl:output method="xml"
	    doctype-public="PUBLIC -//W3C//DTD XHTML 1.0 Strict//EN"
            doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
            encoding="utf-8"
	    indent="yes" />
<!--
</xsl:when>
<xsl:otherwise>
<xsl:output method="xhtml"
	    doctype-public="PUBLIC -//W3C//DTD XHTML 1.0 Strict//EN"
            doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
            encoding="utf-8"
	    indent="yes" />
</xsl:otherwise>
</xsl:choose>
-->

<xsl:template match="/">
<html>
<head>
<title>Coreboot Options</title>
</head>
<body>
<h2>Coreboot Options</h2>
<p>This is an automatically generated list of coreboot compile time
options. Created at <xsl:value-of select="//creationdate"/>.</p>
<table border="1">
<tr bgcolor="#0975a7">
<th align="left">Option</th>
<th align="left">Comment</th>
<th align="left">Default</th>
<th align="left">Export</th>
<th align="left">Format</th>
</tr>
<xsl:for-each select="options/option">
<tr>
<td><xsl:value-of select="@name"/></td>
<td><xsl:value-of select="comment"/></td>
<td><xsl:value-of select="default"/></td>
<td><xsl:value-of select="export"/></td>
<td><xsl:value-of select="format"/></td>
</tr>
</xsl:for-each>
</table>
</body>
</html>
</xsl:template>
</xsl:stylesheet>
