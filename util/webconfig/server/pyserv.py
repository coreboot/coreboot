#!/usr/bin/python
#
#  pyserv.py: Minimal web server for the LinuxBIOS webconfig utility
#   
#  I typically edit in vim with ":set ts=2 sw=2 ai sm"; in case
#  you're using emacs or something and this looks horrible to you,
#  you now know why.
# 
#  Copyright 2001 Robert Drzyzgula, bob@drzyzgula.org
# 
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
# 
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
# 
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 
#
#  Release history
#
#   0.1 Initial release, July 18 2001, R. Drzyzgula
#   0.2 Fixed some typos, added environment variables, July 19 2001, R. Drzyzgula
#

#
# Need some modules
#

from BaseHTTPServer import HTTPServer
from CGIHTTPServer import CGIHTTPRequestHandler
import cgi
import os
import sys
import getopt

#
# Print out the usage syntax
#

def usage():
	print 'Usage: %s [-d] [-t] [-h] [-f freebios] [-s httpdir] [-c cgidir] \\' % sys.argv[0]
	print '          [-l logdir] [-i ipaddr] [-p port]'
	print '       %s [--debug] [--test] [--help] [--freebios=<freebios>] [--httpdir=<httpdir>] \\' % sys.argv[0]
	print '          [--cgidir=<cgidir>] [--log=<logdir>] [--ip=<ipaddr>] [--port=<port>]'
	print 'Where:'
	print '  "-d" or "--debug" enables diagnostic output'
	print '  "-t" or "--test" processes arguments but doesn\'t run the server'
	print '  "-h" or "--help" prints this message'
	print '  "freebios" is the top level of the freebios cvs tree'
	print '      default is environment variable FREEBIOS or /usr/src/freebios'
	print '  "httpdir"  is the directory containing files to be served out '
	print '      default is environment variable HTTPDIR or <freebios>/util/webconfig/htdocs/'
	print '  "cgidir"   is the directory containing cgi scripts to be served out '
	print '      default is environment variable CGIDIR or /cgi-bin/'
	print '      cgidir is always a subdirectory of the httpdir root'
	print '  "logdir"   is the directory where the server logs should be written'
	print '      default is environment variable LOGDIR or <freebios>/util/webconfig/var/'
	print '  "ipaddr"   is the ip address to bind to; it can be a hostname'
	print '      default is 127.0.0.1 (localhost)'
	print '      CAUTION: Changing this could expose this server to external traffic!'
	print '  "port"     is the port number to bind to'
	print '      default is 8083'
	sys.exit()

#
# sift through all the defaults, command-line arguments and envionment
# variables to get stuff set by the rules: Command line has precedance
# over environment, which has precedance over defaults...
#

def process_args():
	#

	debug = testrun = freebios = httpdir = cgidir = logdir = ipaddr = port = None

	#
	# Call getopt to parse any arguments...
	#

	try:
		optlist, tail = getopt.getopt(sys.argv[1:], 'dtf:h:c:l:i:p:', \
			['debug','test','freebios=','httpdir=','cgidir=','log=','ip=','port='])
	except (getopt.GetoptError,RuntimeError), e:
		usage()

  #
	# Let's see what the user set...
	#

	for option, argument in optlist:
		if option in ("-d", "--debug"):
			debug = "TRUE"
		elif option in ("-t", "--test"):
			testrun = "TRUE"
		elif option in ("-h", "--help"):
			usage()
			sys.exit()
		elif option in ("-f", "--freebios"):
			freebios = argument
		elif option in ("-s", "--httpdir"):
			httpdir = argument
		elif option in ("-c", "--cgidir"):
			cgidir = argument
		elif option in ("-l", "--log"):
			logdir = argument
		elif option in ("-i", "--ip"):
			ipaddr = argument
		elif option in ("-p", "--port"):
			port = argument
  #
	# If it ain't a listed argument, we don't have any use for it...
	#
	
	if debug:
		if tail:
			sys.stderr.write("extranious arguments ignored:\n")
			sys.stderr.write(repr(tail))
			sys.stderr.write("\n")

  #
	# set freebios; must end in /, environment has priority over default...
  #

	if freebios == None:
		try:
			freebios = os.environ['FREEBIOS']
		except KeyError:
			freebios = "/usr/src/freebios/"
	if freebios[-1:] != "/":
		freebios = freebios + "/"

	#
	# set httpdir; environment has priority over default,
	# if value doesn't start with "/", prefix with "freebios"
	# Note that "freebios" better be settled by now...
	#

	if httpdir == None:
		try:
			httpdir = os.environ['HTTPDIR']
		except KeyError:
			httpdir = freebios + "util/webconfig/htdocs/"
	if httpdir[:1] != "/":
		httpdir = freebios + httpdir
	if httpdir[-1:] != "/":
		httpdir = httpdir + "/"

	#
	# set cgidir...
	#

	if cgidir == None:
		try:
			cgidir = os.environ['CGIDIR']
		except KeyError:
			cgidir = "/cgi-bin"

	#
	# set logdir...
	#

	if logdir == None:
		try:
			logdir = os.environ['LOGDIR']
		except KeyError:
			logdir = freebios + "util/webconfig/var/"
	if logdir[:1] != "/":
		logdir = freebios + logdir
	if logdir[-1:] != "/":
		logdir = logdir + "/"

	#
	# set the IP address to bind to...
	# by default we bind only to localhost,
	# meaning that this web server should only
	# be visible from the local system.
	# setting ipaddr to be "ALL" will result in
	# the server binding to all addresses on
	# the local machine.
	#
	# WARNING: Setting this to anything but
	# localhost (127.0.0.1) will expose this
	# server to external networks -- this carries
	# additional security risks.
	#

	if ipaddr == None:
		ipaddr = "127.0.0.1"

	#
	# Set the port to bind to. Make sure you don't already
	# have a server on that port. Ports 80, 8000 and 8080
	# are quite commonly use, we default to 8083
	#

	if port == None:
		port = 8083

	#
	# send the results back on up...
	#

	return(debug,testrun,freebios,httpdir,cgidir,logdir,ipaddr,port)

#
# Main...
#

(debug,testrun,freebios,httpdir,cgidir,logdir,ipaddr,port) = process_args()

if debug:
	sys.stderr.write("freebios = %s\n" % freebios)
	sys.stderr.write("cgidir = %s\n" % cgidir)
	sys.stderr.write("httpdir = %s\n" % httpdir)
	sys.stderr.write("logdir = %s\n" % logdir)
	sys.stderr.write("ipaddr = %s\n" % ipaddr)
	sys.stderr.write("port = %s\n" % port)

#
# Stuff some stuff away in environment variables so the
# CGI scripts can pick them up...
#
#
# Actually, this doesn't work because the CGIHTTPRequestHandler
# hand-crafts the environment according to CGI standards.
# It could be made to work by reworking a bunch of the
# handler code but I think that's probably a bad idea...
#
# os.environ['LBWC_FB'] = freebios
# os.environ['LBWC_CD'] = cgidir
# os.environ['LBWC_HD'] = httpdir
# os.environ['LBWC_LD'] = logdir
#
#
# We should now have everything we need to start up the web server...
#

try:
	os.chdir(httpdir)
except:
	print "Cannot change directory to %s. Exiting now." % httpdir
	sys.exit()

#
# Extend the HTTPServer class
# We do this because the default log_message function doesn't
# flush the output after each write.
#

class MyRequestHandler(CGIHTTPRequestHandler):
	logfile = sys.stderr
	def log_message(self,format, *args):
		logfile.write("%s - - [%s] %s\n" %
			(self.address_string(), self.log_date_time_string(), format%args))
		logfile.flush()

#
# Instantiate the server
#

if ipaddr == "ALL":
	serv = HTTPServer(("",port),MyRequestHandler)
else:
	serv = HTTPServer((ipaddr,port),MyRequestHandler)

#
# Set the cgi directory
#

MyRequestHandler.cgi_directories = [cgidir]

#
# Switch stdout to logdir/pyservlog
#

openerror = None
try:
	logfile = open(logdir+"pyservlog","a+")
except:
	openerror = "TRUE"
if openerror:
	if debug:
		sys.stderr.write("Caution: Can't create %s, logging to stdout\n" % logdir+"pyservlog")
else:
	MyRequestHandler.logfile = logfile

#
# Generate file containing this process's pid
#

pid = os.getpid()
openerror = None
try:
	pidlog = open(logdir+"pid","w+")
except:
	openerror = "TRUE"
if openerror:
	if debug:
		sys.stderr.write("Caution: Can't create %s, writing to log\n" % logdir+"pid")
	print pid
	sys.stdout.flush()
else:
	pidlog.write("%s\n" % pid)
	pidlog.close()

#
# Print a message and exit if this is just a test run...
#

if testrun:
	if debug:
		sys.stderr.write("this is where we'd run serv.serve_forever()\n")
	sys.exit()

#
# Start the server...
#

serv.serve_forever()

#
