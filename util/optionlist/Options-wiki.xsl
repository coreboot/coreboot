<?xml version="1.0"?>
<!--
 Copyright (C) 2006 Stefan Reinauer <stepan@coresystems.de>
 Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 
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

<xsl:output method="text"
            encoding="utf-8"
            indent="yes" />

<xsl:template match="/">
This is an automatically generated list of '''coreboot compile-time options'''.

Last update: <xsl:value-of select="//creationdate"/>.

{| border="0"
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
