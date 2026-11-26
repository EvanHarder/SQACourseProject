# conftest.py
#
# Shared pytest setup for every Back End test file. pytest loads this file
# automatically before running any test in backend/tests/.
#
# The Back End modules import each other by plain name (for example
# "from error_log import ErrorLog"), so the backend/ folder must be on the
# import path. This works whether the tests are run from PyCharm, from the
# repository root (pytest backend/tests) or from inside backend/.

import os
import sys

BACKEND_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

if BACKEND_DIR not in sys.path:
    sys.path.insert(0, BACKEND_DIR)
