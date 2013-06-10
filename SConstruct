env = Environment()
env.Append(CPPFLAGS=['-O3'])

circuit = env.Object('Circuit.cpp')
parser = env.Object('VPRNetParser.cpp')
timing = env.Object('timing.cpp')
circuit_profiler = env.Program('CircuitProfiler', 
        [circuit, parser, 'CircuitProfiler.cpp', timing],
        LIBS=['boost_filesystem', 'boost_regex', 'rt',
                'boost_program_options', 'boost_serialization', 'log4cpp'])
circuit_profile_gen = env.Program('CircuitProfileGenerator', 
        [circuit, parser, 'CircuitProfileGenerator.cpp', timing],
        LIBS=['boost_filesystem', 'boost_regex', 'rt',
                'boost_program_options', 'boost_serialization', 'log4cpp'])
circuit_info = env.Program('CircuitInfo', 
        [circuit, parser, 'CircuitInfo.cpp', timing],
        LIBS=['boost_filesystem', 'boost_regex', 'rt',
                'boost_program_options', 'boost_serialization', 'log4cpp'])
circuit_profile_info = env.Program('CircuitProfileInfo', 
        ['CircuitProfileInfo.cpp', timing],
        LIBS=['boost_filesystem', 'boost_regex', 'rt',
                'boost_program_options', 'boost_serialization', 'log4cpp'])
benchmark_gen = env.Program('BenchmarkGenerator', 
        [circuit, parser, 'CircuitBenchmarkGenerator.cpp', timing],
        LIBS=['boost_filesystem', 'boost_regex', 'rt',
                'boost_program_options', 'boost_serialization', 'log4cpp'])
