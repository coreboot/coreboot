<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"
            encoding="utf-8"
            indent="yes" />

<xsl:template match="/">
This is an automatically generated list of '''LinuxBIOS compile-time options'''.

Last update: <xsl:value-of select="//creationdate"/>.

{| border="1"
|- bgcolor="#6699dd"
! align="left" | Option
! align="left" | Comment
! align="left" | Default
! align="left" | Export
! align="left" | Format<xsl:for-each select="options/option">
|- bgcolor="#eeeeee"
|
<xsl:value-of select="@name"/>
|
<xsl:value-of select="comment"/>
|
<xsl:value-of select="default"/>
|
<xsl:value-of select="export"/>
|
<xsl:value-of select="format"/>
</xsl:for-each>
|}

</xsl:template>
</xsl:stylesheet>
