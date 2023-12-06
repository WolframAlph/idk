from distutils.core import setup, Extension

module = Extension(
	'idk',
    sources=['src/idk.c', 'src/patch.c'],
	include_dirs=['src']
)

setup(
	name='idk',
	version='1.0',
	description='Hook into process of object deallocation.',
	author='Yan Yanchii',
	url='https://github.com/WolframAlph/idk',
	ext_modules=[module]
)
