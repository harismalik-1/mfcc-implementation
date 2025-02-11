from setuptools import setup, Extension

mfcc_module = Extension('mfcc',
                       sources=['src/mfcc.c', 'src/mfcc_wrapper.c'],
                       include_dirs=['include'],
                       extra_compile_args=['-std=c11'],
                       libraries=['m'])  # Link math library

setup(name='mfcc',
      version='1.0',
      description='MFCC Feature Extraction',
      ext_modules=[mfcc_module]) 