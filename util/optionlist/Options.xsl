<?xml version="1.0"?>
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
<title>LinuxBIOS Options</title>
</head>
<body>
<h2>LinuxBIOS Options</h2>
<p>This is an automatically generated list of LinuxBIOS compile time
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
