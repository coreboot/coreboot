package main

import (
	"io/fs"
)

type Board struct {
	Vendor                 string
	VendorNice             string
	Vendor2nd              string
	VendorBoard            string
	Board                  string
	BoardNice              string
	BoardURL               string
	NorthbridgeNice        string
	SouthbridgeNice        string
	SuperIONice            string
	CPUNice                string
	SocketNice             string
	ROMPackage             string
	ROMProtocol            string
	ROMSocketed            string
	FlashromSupport        string
	VendorCooperationScore string
	VendorCooperationPage  string
}

type Path struct {
	Path     string
	Basename string
}

type Log struct {
	Reference    bool
	VendorBoard  string
	Time         string
	TimeReadable string
	Upstream     string
	Files        []Path
}

type Category string
type Timeframe string

type DateString string

type TemplateData struct {
	Categories            []Category
	CategoryNiceNames     map[Category]string
	BoardsByCategory      map[Category][]Board
	Timeframes            []Timeframe
	Logs                  map[Timeframe][]Log
	VendorBoardDate       map[string]DateString
	VendorBoardReferenced map[string]bool
}

type NamedFS struct {
	FS   fs.FS
	Name string
}
