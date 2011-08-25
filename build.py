#!/usr/bin/env python
#-*- coding: utf-8 -*-
from distutils.core import setup, Extension

setup(name="nbio", version="1.0", ext_modules=[Extension(
        "nbio",
        ["nbio.c"],
        extra_link_args = ['libNBioBSP.so'],
    )
])
