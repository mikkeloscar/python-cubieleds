from setuptools import setup, Extension, find_packages

setup(name          = 'Cubie.leds',
      version       = '0.1',
      author        = 'Mikkel Oscar Lyderik',
      author_email  = 'mikkeloscar@gmail.com',
      description   = 'A module to control Cubieboard leds',
      license       = 'GPL',
      keywords      = 'Cubie Board leds',
      packages      = find_packages(),
      ext_modules   = [Extension('Cubie.leds', ['src/py_leds.c',
                                                'src/leds.c'])]
      )
