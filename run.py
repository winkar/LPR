# from predict import lpr
import subprocess
import os
import os.path

if not os.path.isdir("build"):
	os.mkdir("build")

os.chdir("build")


if not os.path.isdir("chars"):
	os.mkdir("chars")

subprocess.check_call("cmake ..".split())
subprocess.check_call("make".split())

subprocess.check_call("./LPR".split())

import pprint
# pprint.pprint(lpr("build/chars"))
