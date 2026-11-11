#
# Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

#
# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
# from sphinx.builders.html import StandaloneHTMLBuilder

import re as _re
from importlib import metadata as _metadata
import importlib.util as _importlib_util
from pathlib import Path as _Path
from packaging import version as _version

try:
    _rocm_version = _metadata.version('rocm-docs-core')
except _metadata.PackageNotFoundError:
    _rocm_version = None

_breathe_available = _importlib_util.find_spec('breathe') is not None

if not _breathe_available:
    from docutils import nodes as _nodes
    from docutils.parsers.rst import Directive as _Directive
    from docutils.parsers.rst import directives as _directives

    class _UnavailableDoxygenDirective(_Directive):
        required_arguments = 1
        optional_arguments = 0
        has_content = False
        option_spec = {
            'project': _directives.unchanged,
            'members-only': _directives.flag,
        }

        def run(self):
            return [
                _nodes.paragraph(
                    text='Generated API details require the optional Breathe '
                    'extension.'
                )
            ]

    def setup(app):
        for _name in ('doxygenclass', 'doxygenenum', 'doxygenfile'):
            app.add_directive(_name, _UnavailableDoxygenDirective)
        return {'version': '1.0', 'parallel_read_safe': True}

_rocm_version_expected = "1.0.0"

# Doxygen
#subprocess.call('doxygen Doxyfile.in', shell=True)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'AOCL-UTILS'
copyright = '2024, AMD'
author = 'AMD'
# Derived from the repo-root version.txt so the docs cannot drift from the
# shipped version -- this line sat at 5.0.1 through the 5.1, 5.2, 5.3 and 5.3.1
# releases because it was hardcoded.
#
# The pattern deliberately mirrors CMakeLists.txt's file(STRINGS ... REGEX
# "^[0-9]+\.[0-9]+\.[0-9]+-") rather than approximating it: like CMake it
# selects the first well-formed line and ignores the rest, so a suffixed
# version (5.0.0-dev1, 1.0.1-rc1 -- both shipped) yields its MAJOR.MINOR.PATCH
# instead of being skipped, and a stray line such as a build date cannot be
# mistaken for the version.
_VERSION_RE = _re.compile(r"^([0-9]+\.[0-9]+\.[0-9]+)-")
_version_file = _Path(__file__).resolve().parent.parent / "version.txt"
release = next(
    (
        _m.group(1)
        for _m in (
            _VERSION_RE.match(line.strip())
            for line in _version_file.read_text().splitlines()
        )
        if _m
    ),
    "unknown",
)
version = release

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.autosectionlabel',
    'sphinx.ext.todo',
    'sphinx.ext.coverage',
    'sphinx.ext.mathjax',
    'sphinx.ext.ifconfig',
    'sphinx.ext.viewcode',
    'sphinx.ext.inheritance_diagram',
    'myst_parser',
    ]
if _breathe_available:
    extensions.append('breathe')

# Enable numbering of figures, tables, code-blocks and sections
numfig = True
numfig_format = {
    'section': '%s',
    'figure': 'Figure %s',
    'table': 'Table %s',
    'code-block': 'Listing %s',
}
# Depth of section numbering
numfig_secnum_depth = 3

source_suffix = {
    '.rst': 'restructuredtext',
    '.txt': 'markdown',
    '.md': 'markdown',
}

if _rocm_version is None or _version.parse(_rocm_version) < _version.parse(_rocm_version_expected):
    templates_path = [] if _rocm_version is None else ['_template_fallback']
    html_theme = 'alabaster'
    html_theme_options = {}
else:
    templates_path = ['_template']
    html_theme = 'rocm_docs_theme'
    html_theme_options = {
        "link_main_doc": False,
        "flavor": "local",
        "repository_provider": None,
    }

if _breathe_available:
    breathe_show_define_initializer = True
    breathe_default_project = "aoclutils"
    breathe_default_members = ('members', 'undoc-members')

exclude_patterns = [
    '_build',
    'sphinx/_build',
    'Thumbs.db',
    '.DS_Store',
    'CMakeLists.txt',
]

_toc_file = _Path(__file__).resolve().parent / ".sphinx" / "_toc.yml.in"
_toc_file.parent.mkdir(parents=True, exist_ok=True)
_toc_content = "root: index\n"
if not _toc_file.exists() or _toc_file.read_text() != _toc_content:
    _toc_file.write_text(_toc_content)

# -- Breathe configuration -------------------------------------------------
html_title = 'Home'
myst_title_to_header = True
myst_heading_anchors = 3
suppress_warnings = ["myst.header", "myst.xref_missing", "autosectionlabel.*", "toc.not_included"]
