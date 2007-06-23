# scons build script.
# blackrain at realizedsound dot net - 11 2006

# edit this to point to your SuperCollider3 source directory

sc3_source = '../'

# enable this to build the StkUGens

build_stkugens = False
stklib_path = '/path/to/libstk.a'

##############################################
# simple ugens
headers = sc3_source + 'Headers'

plugs = [
	'AmbisonicUGens',
	'BEQSuiteUGens',
	'BlackrainUGens',
	'JoshUGens',
	'ReverbUGens',
	'MCLDBufferUGens',
	'MCLDChaosUGens',
	'MCLDTriggeredStatsUgens',
	'LoopBuf',
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
	).SharedLibrary(file, 'source/' + file + '.cpp');



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
	).SharedLibrary('StkUGens', 'source/StkUGens/StkAll.cpp', LIBS='libstk.a', LIBPATH=stklib_path);


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

FFT_Env.SharedLibrary('JoshPVUGens', ['source/JoshPVUGens.cpp'] + fft_src_base);

##############################################
# MCLDFFTTriggeredUGens

FFT_Env.SharedLibrary('MCLDFFTTriggeredUGen.cpp', ['source/MCLDFFTTriggeredUGen.cpp', sc3_source + '/Source/plugins/SCComplex.cpp', sc3_source + '/Source/common/fftlib.c']);

##############################################
# MCLDFFTUGens

FFT_Env.SharedLibrary('MCLDFFTUGens', ['source/MCLDFFTUGens.cpp', sc3_source + '/Source/plugins/SCComplex.cpp']);

##############################################
# bhobfft

FFT_Env.SharedLibrary('bhobfft', ['source/bhobFFT.cpp', 'source/FFT2InterfaceBhob.cpp', sc3_source + '/Source/plugins/FeatureDetection.cpp', sc3_source + '/Source/common/fftlib.c', sc3_source + '/Source/plugins/PV_ThirdParty.cpp', sc3_source + '/Source/plugins/SCComplex.cpp' ]);


