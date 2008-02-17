# scons build script.
# blackrain at realizedsound dot net - 11 2006
#<<<<<<< .mine
# vim:ft=python:
#=======
# Additions by Andrzej Kopec - akopec at chopin dot edu dot pl - Oct 07 2007
# vim:ft=python:
#>>>>>>> .r152

import os.path

# edit this to point to your SuperCollider3 source directory

sc3_source = '../'

# enable this to build the StkUGens

#build_stkugens = False
#stklib_path = '/path/to/libstk.a'

#build_ay = True
ay_path = 'source/AY_libayemu/'


if not os.path.exists(sc3_source + 'Headers/plugin_interface/SC_Unit.h'):
	if os.path.exists(sc3_source + '../Headers/plugin_interface/SC_Unit.h'):
		print 'Automatically adjusted sc3_source path, one folder higher'
		sc3_source += '../'
	else:
		print 'Couldn\'t find SuperCollider plugin interface! Is "sc3_source" set correctly in your SConstruct file?'
		Exit(1)


opts = Options('scache.conf', ARGUMENTS)
opts.AddOptions(
    BoolOption('STK',
               'Build with STK plugins', 0),
    PathOption('STKPATH',
               'STK libary path', '/usr/lib'),
    BoolOption('AY',
               'Build with AY plugins', 0)
)

env = Environment(options = opts)

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



##############################################
# simple ugens
headers = sc3_source + 'Headers'

plugs = [
	'BEQSuiteUGens',
	'BlackrainUGens',
	'JoshUGens',
	'JoshAmbiUGens',
	'JoshGrainUGens',
	'MCLDBufferUGens',
	'MCLDFilterUGens',
	'MCLDGetenvUGen',
	'MCLDChaosUGens',
	'MCLDDistortionUGens',
	'MCLDTriggeredStatsUgens',
	'LoopBuf',
	'TagSystemUgens',
	'bhobChaos',
	'BhobFilt',
	'bhobGrain',
	'BhobNoise'
]

for file in plugs :
	Environment(
        	CPPPATH = [headers + '/common', headers + '/plugin_interface', headers + '/server'],
        	CPPDEFINES = ['SC_LINUX', '_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
        	CCFLAGS = ['-Wno-unknown-pragmas'],
        	CXXFLAGS =  ['-Wno-deprecated', '-O3'],
        	SHLIBPREFIX = '',
        	SHLIBSUFFIX = '.so'
	).SharedLibrary(file, 'source/' + file + '.cpp')



##############################################
# StkUGens

if build_stkugens == True:
	Environment(
        	CPPPATH = ['include', headers + '/common', headers + '/plugin_interface', headers + '/server', 'source/StkUGens/include'],
        	CPPDEFINES = ['SC_LINUX', '_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
        	CCFLAGS = ['-Wno-unknown-pragmas'],
       		CXXFLAGS =  ['-Wno-deprecated', '-O3'],
        	SHLIBPREFIX = '',
        	SHLIBSUFFIX = '.so'
	).SharedLibrary('StkUGens', 'source/StkUGens/StkAll.cpp', LIBS='libstk.a', LIBPATH=stklib_path)


##############################################
# base FFT Envirnonment

FFT_Env = Environment(
       	CPPPATH = [headers + '/common', headers + '/plugin_interface', headers + '/server', sc3_source + '/Source/plugins'],
       	CPPDEFINES = ['SC_LINUX', '_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
       	CCFLAGS = ['-Wno-unknown-pragmas'],
       	CXXFLAGS =  ['-Wno-deprecated', '-O3'],
       	SHLIBPREFIX = '',
       	SHLIBSUFFIX = '.so'
)

##############################################
# JoshPVUGens

fft_src_base = [ sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/plugins/Convolution.cpp', sc3_source + '/Source/plugins/FeatureDetection.cpp' ]

FFT_Env.SharedLibrary('JoshPVUGens', ['source/JoshPVUGens.cpp'] + fft_src_base)

##############################################
# MCLDFFTTriggeredUGens

FFT_Env.SharedLibrary('MCLDFFTTriggeredUGen.cpp', ['source/MCLDFFTTriggeredUGen.cpp', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/common/fftlib.c'])

##############################################
# MCLDFFTUGens

FFT_Env.SharedLibrary('MCLDFFTUGens', ['source/MCLDFFTUGens.cpp', sc3_source + '/Source/plugins/SCComplex.cpp'])

##############################################
# bhobfft

FFT_Env.SharedLibrary('bhobfft', ['source/bhobFFT.cpp', 'source/FFT2InterfaceBhob.cpp', sc3_source + '/Source/plugins/FeatureDetection.cpp', sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/plugins/PV_ThirdParty.cpp', sc3_source + '/Source/plugins/SCComplex.cpp' ])

##############################################
# AY
if build_ay == True:
	Environment(
		CPPPATH = ['include', ay_path + 'include' ],
		CCFLAGS = ['-Wno-unknown-pragmas'],
		CXXFLAGS =  ['-Wno-deprecated', '-O3'],
	).StaticLibrary(ay_path + 'AY', [ay_path + 'src/ay8912.c'])
	Environment(
		CPPPATH = ['include', headers + '/common', headers + '/plugin_interface', headers + '/server', ay_path + 'include'],
		CPPDEFINES = ['SC_LINUX', '_REENTRANT', 'NDEBUG', ('SC_MEMORY_ALIGNMENT', 1)],
		CCFLAGS = ['-Wno-unknown-pragmas'],
		CXXFLAGS =  ['-Wno-deprecated', '-O3'],
		SHLIBPREFIX = '',
		SHLIBSUFFIX = '.so'
	).SharedLibrary('AY_UGen', 'source/AY_UGen.cpp', LIBS='AY.a', LIBPATH=ay_path)


