from setuptools import setup, Extension

module = Extension(
    'symnmfmodule',
    sources=['symnmf.c', 'symnmfmodule.c'],
)

setup(
    name='symnmfmodule',
    version='1.0',
    description='Symmetric NMF Module',
    ext_modules=[module],
)
