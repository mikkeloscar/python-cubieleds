from setuptools import setup, Extension, find_packages

classifiers = ['Development Status :: 4 - Beta',
               'Operating System :: POSIX :: Linux',
               'License :: OSI Approved :: GNU General Public License (GPL)',
               'Intended Audience :: Developers',
               'Programming Language :: Python :: 2.7',
               'Programming Language :: Python :: 3',
               'Topic :: Software Development',
               'Topic :: System :: Hardware']


setup(name          = 'cubieleds',
      version       = '0.1',
      author        = 'Mikkel Oscar Lyderik',
      author_email  = 'mikkeloscar@gmail.com',
      description   = 'A module to control Cubieboard LEDs',
      license       = 'GPL',
      keywords      = 'Cubie Board leds',
      url           = 'https://github.com/mikkeloscar/python-cubieleds',
      classifiers   = classifiers,
      packages      = find_packages(),
      ext_modules   = [Extension('Cubie.leds', ['src/py_leds.c',
                                                'src/leds.c'])]
      )
