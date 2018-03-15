import os
import subprocess
import sys

import sysconfig
try:
    import numpy as np
except ImportError:
    pass

##################################################################### FUNCTIONS

def checkAdd(lib = None, header = None, usage = ""):
    """ Checks for a library and/or header and appends it to env if not already appended. """

    usage = " (needed for " + usage + ") " if usage else ""
    if lib and header:
        if not conf.CheckLibWithHeader(lib, header = header, autoadd=0, language="C++"):
            print("ERROR: Library '" + lib + "' or header '" + header + "'" + usage + "not found.")
            Exit(1)
        conf.env.AppendUnique(LIBS = [lib])
    elif lib:
        if not conf.CheckLib(lib, autoadd=0, language="C++"):
            print("ERROR: Library '" + lib + "'" + usage + "not found!")
            Exit(1)
        conf.env.AppendUnique(LIBS = [lib])
    elif header:
        if not conf.CheckCXXHeader(header):
            print("ERROR: Header '" + header + "'" + usage + "not found!")
            Exit(1)



def print_options(vars):
    """ Print all build option and if they have been modified from their default value. """
    for opt in vars.options:
        try:
            is_default = vars.args[opt.key] == opt.default
        except KeyError:
            is_default = True
        vprint(opt.key, env[opt.key], is_default, opt.help)

def vprint(name, value, default=True, description = None):
    """ Pretty prints an environment variabe with value and modified or not. """
    mod = "(default)" if default else "(modified)"
    desc = "   " + description if description else ""
    print("{0:10} {1:10} = {2!s:8}{3}".format(mod, name, value, desc))

def checkset_var(varname, default):
    """ Checks if environment variable is set, use default otherwise and print the value. """
    var = os.getenv(varname)
    if not var:
        var = default
        vprint(varname, var)
    else:
        vprint(varname, var, False)
    return var

def get_real_compiler(compiler):
    """ Gets the compiler behind the MPI compiler wrapper. """
    if compiler.startswith("mpi"):
        try:
            output = subprocess.check_output("%s -show" % compiler, shell=True)
        except (OSError, subprocess.CalledProcessError) as e:
            print("Error getting wrapped compiler from MPI compiler")
            print("Command was:", e.cmd, "Output was:", e.output)
        else:
            return output.split()[0]
    else:
        return compiler



########################################################################## MAIN

vars = Variables(None, ARGUMENTS)

vars.Add(PathVariable("builddir", "Directory holding build files.", "build", PathVariable.PathAccept))
vars.Add(EnumVariable('build', 'Build type', "Debug", allowed_values=('release', 'debug', 'Release', 'Debug')))
vars.Add("compiler", "Compiler to use.", "g++")
vars.Add(BoolVariable("mpi", "Enables MPI-based communication and running coupling tests.", True))
vars.Add(BoolVariable("spirit2", "Used for parsing VRML file geometries and checkpointing.", True))
vars.Add(BoolVariable("petsc", "Enable use of the Petsc linear algebra library.", True))
vars.Add(BoolVariable("python", "Used for Python scripted solver actions.", True))
vars.Add(BoolVariable("gprof", "Used in detailed performance analysis.", False))
vars.Add(EnumVariable('platform', 'Special configuration for certain platforms', "none", allowed_values=('none', 'supermuc', 'hazelhen')))

env = Environment(variables = vars, ENV = os.environ)   # For configuring build variables

conf = Configure(env) # For checking libraries, headers, ...

Help(vars.GenerateHelpText(env))
env.Append(CPPPATH = ['#src'])

print
print_options(vars)

if env["build"] == 'debug':
    env["build"] == 'Debug'
    print("WARNING: Lower-case build type 'debug' is deprecated, use 'Debug' instead!")

if env["build"] == 'release':
    env["build"] == 'Release'
    print("WARNING: Lower-case build type 'release' is deprecated, use 'Release' instead!")
    

buildpath = os.path.join(env["builddir"], "") # Ensures to have a trailing slash

print

env.Append(LIBPATH = [('#' + buildpath)])
env.Append(CCFLAGS= ['-Wall', '-std=c++11'])

# ====== Compiler Settings ======

# Produce position independent code for dynamic linking
env.Append(CCFLAGS = ['-fPIC'])

real_compiler = get_real_compiler(env["compiler"])
if real_compiler == 'icc':
    env.AppendUnique(LIBPATH = ['/usr/lib/'])
    env.Append(LIBS = ['stdc++'])
    if env["build"] == 'debug':
        env.Append(CCFLAGS = ['-align'])
    elif env["build"] == 'release':
        env.Append(CCFLAGS = ['-w', '-fast', '-align', '-ansi-alias'])
elif real_compiler == 'g++':
    pass
elif real_compiler == "clang++":
    env.Append(CCFLAGS= ['-Wsign-compare']) # sign-compare not enabled in Wall with clang.
elif real_compiler == "g++-mp-4.9":
    # Some special treatment that seems to be necessary for Mac OS.
    # See https://github.com/precice/precice/issues/2
    env.Append(LIBS = ['libstdc++.6'])
    env.AppendUnique(LIBPATH = ['/opt/local/lib/'])

env.Replace(CXX = env["compiler"])
env.Replace(CC = env["compiler"])

if 'CONDA_PREFIX' in os.environ:  # building takes place in conda environment
    env.Append(CPPPATH = os.path.join( os.environ['CONDA_PREFIX'], 'include'))
    env.Append(LIBPATH = os.path.join( os.environ['CONDA_PREFIX'], 'lib'))

if not conf.CheckCXX():
    Exit(1)

# ====== Build Directories ======
if env["build"] == 'Debug':
    # The Assert define does not actually switches asserts on/off, these are controlled by NDEBUG.
    # It's kept in place for some legacy code.
    env.Append(CCFLAGS = ['-g3', '-O0'])
    env.Append(LINKFLAGS = ["-rdynamic"]) # Gives more informative backtraces
    buildpath += "debug"
elif env["build"] == 'Release':
    env.Append(CPPDEFINES = ['NDEBUG']) # Standard C++ macro which disables all asserts, also used by Eigen
    env.Append(CCFLAGS = ['-O3'])
    buildpath += "release"

    
# ====== libpthread ======
checkAdd("pthread")

    
# ====== PETSc ======
if env["petsc"]:
    if 'CONDA_PREFIX' in os.environ:  # building takes place in conda environment
        PETSC_DIR = ""  # todo determine path of petsc in conda environment
        PETSC_ARCH = ""
        print("using petsc with conda currently not supported!")
        Exit(1)
    else:
        PETSC_DIR = checkset_var("PETSC_DIR", "")
        PETSC_ARCH = checkset_var("PETSC_ARCH", "")
    
    if not env["mpi"]:
        print("PETSc requires MPI to be enabled.")
        Exit(1)
       
    env.Append(CPPPATH = [os.path.join( PETSC_DIR, "include"),
                          os.path.join( PETSC_DIR, PETSC_ARCH, "include")])
    env.Append(LIBPATH = [os.path.join( PETSC_DIR, PETSC_ARCH, "lib"),
                          os.path.join( PETSC_DIR, "lib")])
    if env["platform"] == "hazelhen":
        checkAdd("craypetsc_gnu_real")
    else:
        checkAdd("petsc")
else:
    env.Append(CPPDEFINES = ['PRECICE_NO_PETSC'])
    buildpath += "-nopetsc"

# ====== Eigen ======
if 'CONDA_PREFIX' in os.environ:  # building takes place in conda environment
    env.Append(CPPPATH = os.path.join( os.environ['CONDA_PREFIX'], 'include/eigen3'))

checkAdd(header = "Eigen/Dense", usage = "Eigen")
if env["build"] == "debug":
    env.Append(CPPDEFINES = ['EIGEN_INITIALIZE_MATRICES_BY_NAN'])

# ====== Boost ======
# Needed for correct linking on Hazel Hen
# Otherwise it would link partly to old system boost, partly to newer modules boost
if env["platform"] == "hazelhen":
    env.Append(CPPPATH = os.path.join( os.environ['BOOST_ROOT'], 'include'))
    env.Append(LIBPATH = os.path.join( os.environ['BOOST_ROOT'], 'lib'))

env.Append(CPPDEFINES= ['BOOST_SPIRIT_USE_PHOENIX_V3',
                        'BOOST_ALL_DYN_LINK',
                        'BOOST_ASIO_ENABLE_OLD_SERVICES']) # Interfaces have changed in 1.66

checkAdd("boost_log")
checkAdd("boost_log_setup")
checkAdd("boost_thread")
checkAdd("boost_system")
checkAdd("boost_filesystem")
checkAdd("boost_program_options")
checkAdd("boost_unit_test_framework")

checkAdd(header = 'boost/vmd/is_empty.hpp', usage = 'Boost Variadic Macro Data Library')
checkAdd(header = 'boost/geometry.hpp', usage = 'Boost Geometry Library')
checkAdd(header = 'boost/signals2.hpp', usage = 'Boost Signals2')

# ====== Spirit2 ======
if not env["spirit2"]:
    env.Append(CPPDEFINES = ['PRECICE_NO_SPIRIT2'])
    buildpath += "-nospirit2"

# ====== MPI ======
if env["mpi"]:
    if not conf.CheckCXXHeader("mpi.h"):
        print("mpi.h not found. Maybe try 'compiler=mpicxx' or 'compiler=mpic++' as scons argument?")
        Exit(1)
        
    # Skip (deprecated) MPI C++ bindings.
    env.Append(CPPDEFINES = ['MPICH_SKIP_MPICXX'])
        
elif not env["mpi"]:
    env.Append(CPPDEFINES = ['PRECICE_NO_MPI'])
    buildpath += "-nompi"

# ====== Python ======
if env["python"]:
    installation_scheme = sysconfig._get_default_scheme()
    if installation_scheme is 'posix_local':  # for ubuntu with python 2.7 posix_local scheme points to an empty include path, fix this by using posix_prefix. See https://stackoverflow.com/questions/48826123/why-do-include-paths-in-python2-and-python3-differ
        installation_scheme = 'posix_prefix'

    pythonLibDefault = 'python'+str(sys.version_info.major)+'.'+str(sys.version_info.minor)
    pythonLibPathDefault = sysconfig.get_config_var('LIBDIR')
    pythonIncPathDefault = sysconfig.get_path('include', scheme=installation_scheme)
    numpyIncPathDefault = np.get_include()

    pythonLib = checkset_var('PRECICE_PYTHON_LIB', pythonLibDefault)
    pythonLibPath = checkset_var('PRECICE_PYTHON_LIB_PATH', pythonLibPathDefault)
    pythonIncPath = checkset_var('PRECICE_PYTHON_INC_PATH', pythonIncPathDefault)
    numpyIncPath = checkset_var('PRECICE_NUMPY_INC_PATH', numpyIncPathDefault)

    # FIXME: Supresses NumPy deprecation warnings. Needs to converted to the newer API.
    env.Append(CPPDEFINES = ['NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION'])
    env.AppendUnique(CPPPATH = [pythonIncPath, numpyIncPath])
    env.AppendUnique(LIBPATH = [pythonLib])
    checkAdd(lib = pythonLib, header = "Python.h")
    # Check for numpy header needs python header first to compile
    checkAdd( header = ['Python.h', 'numpy/arrayobject.h'], usage = "NumPy")
else:
    buildpath += "-nopython"
    env.Append(CPPDEFINES = ['PRECICE_NO_PYTHON'])


# ====== GProf ======
if env["gprof"]:
    env.Append(CCFLAGS = ['-p', '-pg'])
    env.Append(LINKFLAGS = ['-p', '-pg'])
    buildpath += "-gprof"

# ====== Special Platforms ======
if env["platform"] == "supermuc":
    env.Append(CPPDEFINES = ['SuperMUC_WORK'])
elif env["platform"] == "hazelhen":
    env.Append(LINKFLAGS = ['-dynamic']) # Needed for correct linking against boost.log

# ====== LibXML2 ======
if 'CONDA_PREFIX' in os.environ:  # building takes place in conda environment
    env.Append(CPPPATH = os.path.join( os.environ['CONDA_PREFIX'], 'include/libxml2'))
else:
    env.Append(CPPPATH = ['/usr/include/libxml2'])
checkAdd("xml2")

print
env = conf.Finish() # Used to check libraries

#--------------------------------------------- Define sources and build targets

(sourcesAllNoMain, sourcesMain, sourcesTests) = SConscript (
    'src/SConscript-linux',
    variant_dir = buildpath,
    duplicate = 0
)

staticlib = env.StaticLibrary (
    target = buildpath + '/libprecice',
    source = [sourcesAllNoMain]
)
env.Alias("staticlib", staticlib)

solib = env.SharedLibrary (
    target = buildpath + '/libprecice',
    source = [sourcesAllNoMain]
)
env.Alias("solib", solib)

bin = env.Program (
    target = buildpath + '/binprecice',
    source = [sourcesAllNoMain,
              sourcesMain]
)
env.Alias("bin", bin)

tests = env.Program (
    target = buildpath + '/testprecice',
    source = [sourcesAllNoMain,
              sourcesTests]
)
env.Alias("tests", tests)

# Creates a symlink that always points to the latest build
symlink = env.Command(
    target = "symlink",
    source = None,
    action = "ln -fns {0} {1}".format(os.path.split(buildpath)[-1], os.path.join(os.path.split(buildpath)[0], "last"))
)

Default(staticlib, bin, solib, tests, symlink)

AlwaysBuild(symlink)

print("Targets:   " + ", ".join([str(i) for i in BUILD_TARGETS]))
print("Buildpath: " + buildpath)
print()
