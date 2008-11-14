# scons build script.
# blackrain at realizedsound dot net - 11 2006
#=======
# Additions by Andrzej Kopec - akopec at chopin dot edu dot pl - Oct 07 2007
#=======
# Additions by Marije Baalman - nescivi at gmail dot com - Mar 03 2008

import os.path
import platform

opts = Options('scache.conf', ARGUMENTS)
opts.AddOptions(
    BoolOption('STK',
               'Build with STK plugins', 0),
    BoolOption('AY',
               'Build with AY plugins', 0),
		('CXXFLAGS', 'C++ compiler flags'),
	)

print 'Building for ' + platform.system()
if platform.system() == 'Linux':
	opts.AddOptions(
		PathOption('SC3PATH', 'SuperCollider source path', '../' ),
    	PathOption('STKPATH', 'STK libary path', '/usr/lib')
	)
if platform.system() == 'Windows':
	opts.AddOptions(
    	PathOption('STKPATH',
        	       'STK libary path', 'C:'),
		PathOption('SC3PATH', 'SuperCollider source path', 'C:' ),
		PathOption('PTHREADSPATH', 'pthreads path', 'C:' )
	)
	
# Configure base environment for the current platform
if platform.system() == 'Windows':
	# Use mingw
	env = Environment(options = opts, tools = ['mingw'])
else:
	env = Environment(options = opts)


env.Append(CXXFLAGS = ['-Wno-deprecated', '-O3'])


########################################
# Configure for all platforms
 
sc3_source = env['SC3PATH']
print 'SuperCollider 3 source is at: ' + sc3_source
if not os.path.exists(sc3_source + 'Headers/plugin_interface/SC_Unit.h'):
	if os.path.exists(sc3_source + '../Headers/plugin_interface/SC_Unit.h'):
		print 'Automatically adjusted sc3_source path, one folder higher'
		sc3_source += '../'
	else:
		print 'Couldn\'t find SuperCollider plugin interface! Is "sc3_source" set correctly in your SConstruct file?'
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
	export_helper = sc3_source + '/windows/PlugIns/ExportHelper.cpp'
	if not os.path.exists(export_helper):
		print 'Couldn\'t find ExportHelper.cpp! Check your SuperCollider directory.'
		Exit(1)
	platform_CPPDEFINES = ['SC_WIN32', '__GCC__']
	platform_SOURCES = [ export_helper ]
	platform_HEADERS = [ sc3_source + '/libsndfile', pthreads ]
	platform_SHLIBSUFFIX = '.scx'
	
########################################
# Configure for Linux 

if platform.system() == 'Linux':
	platform_CPPDEFINES = ['SC_LINUX']
	platform_SOURCES = [ ]
	platform_HEADERS = [ ]
	platform_SHLIBSUFFIX = '.so'
	

##############################################
# simple ugens
headers = sc3_source + 'Headers'

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
	'MCLDTriggeredStatsUgens',
	'LoopBuf',
	'SLUGens',
	'TagSystemUgens',
	'bhobChaos',
	'BhobFilt',
	'bhobGrain',
	'BhobNoise',
	'LadspaUGens'
]

Basic_Env = env.Clone(
        	CPPPATH = platform_HEADERS + [headers + '/common', headers + '/plugin_interface', headers + '/server'],
        	CPPDEFINES = platform_CPPDEFINES + ['_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
        	CCFLAGS = ['-Wno-unknown-pragmas'],
        	SHLIBPREFIX = '',
        	SHLIBSUFFIX = platform_SHLIBSUFFIX
);

for file in plugs :
	Basic_Env.SharedLibrary('build/' + file, ['source/' + file + '.cpp'] + platform_SOURCES )


##############################################
# MdaUGens

Basic_Env.SharedLibrary('build/MdaUGens', ['source/MdaUGens/MdaUGens.cpp'] + platform_SOURCES )


##############################################
# MembraneUGens

Basic_Env.SharedLibrary('build/MembraneUGens', ['source/Membrane_shape.c', 'source/Membrane.cpp'] + platform_SOURCES )


##############################################
# MLfftwUGens

Basic_Env.SharedLibrary('build/MLfftwUGens', ['source/MLfftwUGens/AnalyseEvents2fftw.cpp', 'source/MLfftwUGens/AutoTrackfftw.cpp', 'source/MLfftwUGens/Concatfftw.cpp', 'source/MLfftwUGens/MLfftwUGens.cpp', 'source/MLfftwUGens/Qitchfftw.cpp', 'source/MLfftwUGens/Tartini.cpp'  ] + platform_SOURCES , LIBS='fftw3f')

##############################################
# StkUGens

if build_stkugens == True:
	env.Clone(
       		CPPPATH = platform_HEADERS + ['include', headers + '/common', headers + '/plugin_interface', headers + '/server', 'source/StkUGens/include'],
        	CPPDEFINES = platform_CPPDEFINES + ['_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
        	CCFLAGS = ['-Wno-unknown-pragmas'],
        	SHLIBPREFIX = '',
        	SHLIBSUFFIX = platform_SHLIBSUFFIX
	).SharedLibrary('build/StkUGens', ['source/StkUGens/StkAll.cpp'] + platform_SOURCES, LIBS='libstk.a', LIBPATH=stklib_path)


##############################################
# base FFT Envirnonment

FFT_Env = env.Clone(
       	CPPPATH = platform_HEADERS + [headers + '/common', headers + '/plugin_interface', headers + '/server', sc3_source + '/Source/plugins'],
       	CPPDEFINES = platform_CPPDEFINES + ['_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
       	CCFLAGS = ['-Wno-unknown-pragmas'],
       	SHLIBPREFIX = '',
       	SHLIBSUFFIX = platform_SHLIBSUFFIX
)

##############################################
# JoshPVUGens

fft_src_base = [ sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/common/SC_fftlib.cpp', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/plugins/Convolution.cpp', sc3_source + '/Source/plugins/FeatureDetection.cpp' ]

FFT_Env.SharedLibrary('build/' + 'JoshPVUGens', ['source/JoshPVUGens.cpp'] + fft_src_base  + platform_SOURCES, LIBS='fftw3f')

##############################################
# MCLDFFTTriggeredUGens

FFT_Env.SharedLibrary('build/' + 'MCLDFFTTriggeredUGen', ['source/MCLDFFTTriggeredUGen.cpp', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/common/fftlib.c']  + platform_SOURCES)

##############################################
# MCLDFFTUGens

FFT_Env.SharedLibrary('build/' + 'MCLDFFTUGens', ['source/MCLDFFTUGens.cpp', sc3_source + '/Source/plugins/SCComplex.cpp']  + platform_SOURCES)

##############################################
# bhobfft

FFT_Env.SharedLibrary('build/' + 'bhobfft', ['source/bhobFFT.cpp', 'source/FFT2InterfaceBhob.cpp', sc3_source + '/Source/plugins/FeatureDetection.cpp', sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/plugins/PV_ThirdParty.cpp', sc3_source + '/Source/plugins/SCComplex.cpp' ] + platform_SOURCES)

##############################################
# AY
if build_ay == True:
	env.Clone(
		CPPPATH = ['include', ay_path + 'include' ],
		CCFLAGS = ['-Wno-unknown-pragmas'],
	).StaticLibrary(ay_path + 'AY', [ay_path + 'src/ay8912.c'])
	env.Clone(
		CPPPATH = ['include', headers + '/common', headers + '/plugin_interface', headers + '/server', ay_path + 'include'],
		CPPDEFINES = ['SC_LINUX', '_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
		CCFLAGS = ['-Wno-unknown-pragmas'],
		SHLIBPREFIX = '',
		SHLIBSUFFIX = '.so'
	).SharedLibrary('AY_UGen', 'source/AY_UGen.cpp', LIBS='AY.a', LIBPATH=ay_path)

opts.Save('scache.conf', env)
Help(opts.GenerateHelpText(env))

