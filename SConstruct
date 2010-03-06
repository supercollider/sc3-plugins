# scons build script.
# blackrain at realizedsound dot net - 11 2006
#=======
# Additions by Andrzej Kopec - akopec at chopin dot edu dot pl - Oct 07 2007
#=======
# Additions by Marije Baalman - nescivi at gmail dot com - Mar 03 2008

import os.path
import platform
import re
import shutil

PACKAGE = 'SC3plugins'

f = open('VERSION')
VERSION = f.readline()
f.close()

opts = Options('scache.conf', ARGUMENTS)

ANY_FILE_RE = re.compile('.*')
HELP_FILE_RE = re.compile('.*\.(rtf(d)?|scd|html)$')
SC_FILE_RE = re.compile('.*\.sc$')

print 'Building for ' + platform.system()
if platform.system() == 'Linux':
	opts.AddOptions(
		PathOption('SC3PATH', 'SuperCollider source path', '../common/' ),
		PathOption('STKPATH', 'STK libary path', '/usr/lib')
	)
	PLUGIN_FILE_RE = re.compile('.*\.so$')
	PLUGIN_EXT = '.so'
	DEFAULT_PREFIX = '/usr/local'
if platform.system() == 'OSX':
	opts.AddOptions(
		PathOption('SC3PATH', 'SuperCollider source path', '../common/' ),
		PathOption('STKPATH', 'STK libary path', '/usr/lib')
	)
	PLUGIN_FILE_RE = re.compile('.*\.scx$')
	PLUGIN_EXT = '.scx'
	DEFAULT_PREFIX = '/usr/local'
if platform.system() == 'Windows':
	opts.AddOptions(
    	PathOption('STKPATH', 'STK libary path', 'C:/'),
		PathOption('SC3PATH', 'SuperCollider source path', '../common/' ),
		PathOption('PTHREADSPATH', 'pthreads path', '../../pthreads-win32' ),
		PathOption('FFTW3PATH', 'fftw3 path', '../../fftw3' )
	)
	PLUGIN_FILE_RE = re.compile('.*\.scx$')
	PLUGIN_EXT = '.scx'
	DEFAULT_PREFIX = 'C:/'

opts.AddOptions(
    BoolOption('STK',
               'Build with STK plugins', 0),
    BoolOption('STKDynamic',
               'Dynamically link to STK lib', 0),
    BoolOption('AY',
               'Build with AY plugins', 0),
    BoolOption('QUARKS',
               'Installation as quarks', 0),
    PathOption('PREFIX',
               'Installation prefix', DEFAULT_PREFIX),
    PathOption('DESTDIR',
               'Intermediate installation prefix for packaging', '/'),
	('CXXFLAGS', 'C++ compiler flags', "-Wno-deprecated -O3"),
    #BoolOption('SSE',
               #'Build with SSE support', 1),
	)

def make_os_env(*keys):
	env = os.environ
	res = {}
	for key in keys:
		if env.has_key(key):
			res[key] = env[key]
	return res


# Configure base environment for the current platform

if platform.system() != 'Windows':
	env = Environment(options = opts, 
	                  ENV = make_os_env('PATH', 'PKG_CONFIG_PATH'),
                      PACKAGE = PACKAGE,
                      VERSION = VERSION,
                      URL = 'http://sc3-plugins.sourceforge.net',
                      TARBALL = PACKAGE + VERSION + '.tbz2')
	env.Append(PATH = ['/usr/local/bin', '/usr/bin', '/bin'])

else:
	# Use mingw - no pkg_config
	env = Environment(options = opts,
                      tools = ['mingw'],
	                  ENV = os.environ,
	                  PACKAGE = PACKAGE,
	                  VERSION = VERSION,
	                  URL = 'http://sc3-plugins.sourceforge.net',
	                  TARBALL = PACKAGE + VERSION + '.tbz2')

########################################
# install function

def install_dir(env, src_dir, dst_dir, filter_re, strip_levels=0):
	nodes = []
	for root, dirs, files in os.walk(src_dir):
		src_paths = []
		dst_paths = []
		if 'CVS' in dirs: dirs.remove('CVS')
		if '.svn' in dirs: dirs.remove('.svn')
		for d in dirs[:]:
			if filter_re.match(d):
				src_paths += flatten_dir(os.path.join(root, d))
				dirs.remove(d)
		for f in files:
			if filter_re.match(f):
				src_paths.append(os.path.join(root, f))
		dst_paths += map(
			lambda f:
			os.path.join(
			dst_dir,
			*f.split(os.path.sep)[strip_levels:]),
			src_paths)
		nodes += env.InstallAs(dst_paths, src_paths)
	return nodes

def lib_dir(prefix):
	return os.path.join(prefix, 'lib')
def share_dir(prefix):
	return os.path.join(prefix, 'share')

def is_home_directory(dir):
	return os.path.normpath(dir) == os.path.normpath(os.environ.get('HOME', '/'))

def pkg_data_dir(prefix, *args):
	if PLATFORM == 'darwin':
		base = '/Library/Application Support'
		if is_home_directory(prefix):
			base = os.path.join(prefix, base)
	else:
		base = os.path.join(prefix, 'share')
	return os.path.join(base, PACKAGE, *args)

def pkg_lib_dir(prefix, *args):
	return os.path.join(lib_dir(prefix), PACKAGE, *args)

def pkg_plug_dir(prefix, *args):
	if env['QUARKS'] :
		return os.path.join(share_dir(prefix), 'SuperCollider', PACKAGE, *args)
	else :
		return os.path.join(share_dir(prefix), 'SuperCollider/Extensions', PACKAGE, *args)

def pkg_help_dir(prefix, *args):
	return os.path.join(share_dir(prefix), 'SuperCollider/Extensions/Help', PACKAGE, *args)

def flatten_dir(dir):
	res = []
	for root, dirs, files in os.walk(dir):
		if 'CVS' in dirs: dirs.remove('CVS')
		if '.svn' in dirs: dirs.remove('.svn')
		for f in files:
			res.append(os.path.join(root, f))
	return res

########################################
# Configure for all platforms

sc3_source = env['SC3PATH']
print 'SuperCollider 3 source is at: ' + sc3_source
if not os.path.exists(sc3_source + 'Headers/plugin_interface/SC_Unit.h'):
	if os.path.exists(sc3_source + '../../common/Headers/plugin_interface/SC_Unit.h'):
		print 'Automatically adjusted sc3_source path, one folder higher'
		sc3_source += '../../common'
	else:
		print 'Couldn\'t find SuperCollider plugin interface! Please specify "SC3PATH" argument.'
		Exit(1)

if env['STK']:
	build_stkugens = True
	stklib_path = env['STKPATH']
else:
	build_stkugens = False

if env['AY']:
	build_ay = True
	ay_path = 'source/AY_libayemu/'
else:
	build_ay = False

########################################
# Configure for Windows

if platform.system() == 'Windows':
	pthreads = env['PTHREADSPATH']
	print 'pthreads source is at: ' + pthreads
	if not os.path.exists(pthreads + '/pthread.h'):
		print 'Couldn\'t find pthreads! Is "pthreads" set correctly in your SConstruct file?'
		Exit(1)
	export_helper = sc3_source + 'windows/PlugIns/ExportHelper.cpp'
	if not os.path.exists(export_helper):
		print 'Couldn\'t find ExportHelper.cpp! Check your SuperCollider directory.'
		Exit(1)
	fftw3 = env['FFTW3PATH']
	print 'fftw source is at: ' + fftw3
	platform_CPPDEFINES = ['SC_WIN32', '__GCC__']
	platform_SOURCES = [ export_helper ]
	platform_HEADERS = [ sc3_source + '/libsndfile', pthreads, sc3_source + '/windows/compat_stuff' ]

########################################
# Configure for Linux

if platform.system() == 'Linux':
	platform_CPPDEFINES = ['SC_LINUX']
	platform_SOURCES = [ ]
	platform_HEADERS = [ ]

########################################
# Configure for OSX

if platform.system() == 'OSX':
	platform_CPPDEFINES = ['SC_DARWIN']
	platform_SOURCES = [ ]
	platform_HEADERS = [ ]

##############################################
# simple ugens
headers = sc3_source + 'Headers'

def make_plugin_target(name):
    return os.path.join('build', name)

plugins = []

plugs = [
	'BatUGens',
	'BlackrainUGens',
	'JoshUGens',
	'JoshAmbiUGens',
	'JoshGrainUGens',
	'MCLDBufferUGens',
	'MCLDFilterUGens',
	'MCLDGetenvUGen',
	'MCLDChaosUGens',
	'MCLDDistortionUGens',
	'MCLDOscUGens',
	'MCLDSOMUGens',
	'MCLDTreeUGens',
	'MCLDTriggeredStatsUgens',
	'LoopBuf',
	'RFWUGens',
	'RMEQSuite',
	'SLUGens',
	'TagSystemUgens',
	'BhobChaos',
	'BhobFilt',
	'BhobGrain',
	'BhobNoise',
	'LadspaUGens',
	'VOSIM',
	'BerlachUGens'
]

if platform.system() == 'OSX':
	plugs += 'MCLD_CQ_UGens'

Basic_Env = env.Clone(
	CPPPATH = platform_HEADERS + [headers + '/common', headers + '/plugin_interface', headers + '/server'],
	CPPDEFINES = platform_CPPDEFINES + ['_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 16)],
	CCFLAGS = ['-Wno-unknown-pragmas'],
	SHLIBPREFIX = '',
	SHLIBSUFFIX = PLUGIN_EXT
);

if platform.system() == 'Windows':
	Basic_Env.SharedObject(target = 'ExportHelper.o', source = export_helper)
	platform_SOURCES = ['ExportHelper.o']
	Basic_Env.Append(LIBPATH=fftw3)

for file in plugs :
	plugins.append( Basic_Env.SharedLibrary(make_plugin_target(file), ['source/' + file + '.cpp'] + platform_SOURCES ) )


##############################################
# MdaUGens

plugins.append( Basic_Env.SharedLibrary('build/MdaUGens', ['source/MdaUGens/MdaUGens.cpp'] + platform_SOURCES ) )


##############################################
# MembraneUGens

plugins.append( Basic_Env.SharedLibrary('build/MembraneUGens', ['source/Membrane_shape.c', 'source/Membrane.cpp'] + platform_SOURCES ) )


##############################################
# MLfftwUGens

plugins.append( Basic_Env.SharedLibrary('build/MLfftwUGens', ['source/MLfftwUGens/AnalyseEvents2fftw.cpp', 'source/MLfftwUGens/AutoTrackfftw.cpp', 'source/MLfftwUGens/Concatfftw.cpp', 'source/MLfftwUGens/MLfftwUGens.cpp', 'source/MLfftwUGens/Qitchfftw.cpp', 'source/MLfftwUGens/Tartini.cpp'  ] + platform_SOURCES , LIBS='fftw3f') )


##############################################
# StkUGens

#def make_conf(env):
    #return Configure(env, custom_tests = { 'CheckPKGConfig' : CheckPKGConfig,
                                           #'CheckPKG' : CheckPKG })

if build_stkugens == True:
	if env['STKDynamic']:
		stkEnv = env.Clone(
			#	CPPPATH = platform_HEADERS + ['include', headers + '/common', headers + '/plugin_interface', headers + '/server', 'source/StkUGens/include'],
				CPPPATH = platform_HEADERS + ['/usr/include/stk', headers + '/common', headers + '/plugin_interface', headers + '/server', 'source/StkUGens/include'],
				CPPDEFINES = platform_CPPDEFINES + ['_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
				CCFLAGS = ['-Wno-unknown-pragmas'],
				SHLIBPREFIX = '',
				SHLIBSUFFIX = PLUGIN_EXT,
				#LINKFLAGS = File(stklib_path+'/libstk.so')
		)
		stkEnv.Append( LIBS = ['stk','asound','rt','jack', 'pthread', 'm'], DEFS = ['__LINUX_OSS__' , '__LINUX_ALSA__' , '__LINUX_JACK__', '__LINUX_ALSASEQ__' ] )
		plugins.append( stkEnv.SharedLibrary('build/StkUGens', ['source/StkUGens/StkAll.cpp'] + platform_SOURCES)
		)
	else:
		plugins.append( env.Clone(
				CPPPATH = platform_HEADERS + ['include', headers + '/common', headers + '/plugin_interface', headers + '/server', 'source/StkUGens/include'],
				CPPDEFINES = platform_CPPDEFINES + ['_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
				CCFLAGS = ['-Wno-unknown-pragmas'],
				SHLIBPREFIX = '',
				SHLIBSUFFIX = PLUGIN_EXT
		).SharedLibrary('build/StkUGens', ['source/StkUGens/StkAll.cpp'] + platform_SOURCES, LIBS=File(stklib_path+'/libstk.a'))
		)

##############################################
# base FFT Envirnonment

FFT_Env = Basic_Env.Clone()

FFT_Env.Append(CPPPATH=[sc3_source + '/Source/plugins'])

if platform.system() == 'Windows':
	FFT_Env.Append(LIBPATH=fftw3)

##############################################
# JoshPVUGens

fft_src_base = [ sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/common/SC_fftlib.cpp', sc3_source + '/Source/plugins/SCComplex.cpp' ]

plugins.append( FFT_Env.SharedLibrary('build/' + 'JoshPVUGens', ['source/JoshPVUGens.cpp'] + fft_src_base  + platform_SOURCES, LIBS='fftw3f') )

##############################################
# NCAnalysis

plugins.append( FFT_Env.SharedLibrary('build/NCAnalysis', ['source/NCAnalysis/SMS.cpp','source/NCAnalysis/TPV.cpp','source/NCAnalysis/NCAnalysis.cpp', 'source/NCAnalysis/LPCAnalysis.cpp', 'source/NCAnalysis/LPCAnalyzer.cpp', 'source/NCAnalysis/WalshHadamard.cpp', 'source/NCAnalysis/WaveletDaub.cpp', sc3_source + '/Source/common/SC_fftlib.cpp', sc3_source + '/Source/plugins/SCComplex.cpp'  ] + platform_SOURCES , LIBS='fftw3f') )

##############################################
# MCLDFFTTriggeredUGens
# DEPRECATED - see FFTTriggered helpfile for why
#plugins.append( FFT_Env.SharedLibrary('build/' + 'MCLDFFTTriggeredUGen', ['source/MCLDFFTTriggeredUGen.cpp', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/common/fftlib.c']  + platform_SOURCES) )

##############################################
# MCLDFFTUGens

plugins.append( FFT_Env.SharedLibrary('build/' + 'MCLDFFTUGens', ['source/MCLDFFTUGens.cpp', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/common/SC_fftlib.cpp']  + platform_SOURCES, LIBS='fftw3f') )

##############################################
# bhobfft

plugins.append( FFT_Env.SharedLibrary('build/' + 'BhobFFT', ['source/BhobFFT.cpp', 'source/FFT2InterfaceBhob.cpp', sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/plugins/PV_ThirdParty.cpp', sc3_source + '/Source/plugins/SCComplex.cpp' ] + platform_SOURCES) )

##############################################
# AY
if build_ay == True:
	env.Clone(
		CPPPATH = ['include', ay_path + 'include' ],
		CCFLAGS = ['-Wno-unknown-pragmas', '-fPIC'],
	).StaticLibrary(ay_path + 'AY', [ay_path + 'src/ay8912.c'])
	ay_env = Basic_Env.Clone()
	ay_env.Append(CPPPATH = ['include', ay_path + 'include'])
	ay_env.SharedLibrary(make_plugin_target('AY_UGen'), 'source/AY_UGen.cpp', LIBS='AY.a', LIBPATH=ay_path)

opts.Save('scache.conf', env)
Help(opts.GenerateHelpText(env))

plugdirs = [
	'AY',
	'Bat',
	'Bhob',
	'Blackrain',
	'Josh',
	'Ladspa',
	'LoopBuf',
	'MCLD',
	'Mda',
	'Membrane',
	'MLfftw',
	'RFW',
	'RMEQSuite',
	'SL',
	'Stk',
	'TagSystem',
	'VOSIM',
	'Berlach',
	'NCAnalysis'
]

# ======================================================================
# installation directories
# ======================================================================

def is_installing():
	pat = re.compile('^install.*$')
	for x in COMMAND_LINE_TARGETS:
		if pat.match(x): return True
	return False


FINAL_PREFIX = '$PREFIX'
INSTALL_PREFIX = os.path.join('$DESTDIR', '$PREFIX')

if env['PREFIX'] == '/usr':
	FINAL_CONFIG_PREFIX = '/etc'
else:
	FINAL_CONFIG_PREFIX = os.path.join(env['PREFIX'], 'etc')
CONFIG_PREFIX = '$DESTDIR' + FINAL_CONFIG_PREFIX

if is_installing():
	print "move plugins into the right dirs"
	for plugname in plugdirs :
		thisplug = os.path.join( 'build', plugname+"*"+PLUGIN_EXT )
		for plug in plugins :
			env.Install(os.path.join( 'build', plugname+"UGens" ), plug)

env.Alias('install-plugins',
	install_dir(
		env, 'build/',
		pkg_plug_dir(INSTALL_PREFIX),
		ANY_FILE_RE, 1)
	)
env.Alias('install-plugins',
	install_dir(
		env, 'Help/',
		pkg_help_dir(INSTALL_PREFIX),
		ANY_FILE_RE, 1)
	)


# ======================================================================
# installation
# ======================================================================

installEnv = Environment(
	ALL = ['install-plugins']
	)

env.Alias('install', installEnv['ALL'])
