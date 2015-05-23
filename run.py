# coding=utf-8

from predict import lpr
import subprocess
import os
import os.path

if not os.path.isdir("build"):
	os.mkdir("build")

os.chdir("build")


if not os.path.isdir("chars"):
	os.mkdir("chars")

if not os.path.isdir("midImages"):
	os.mkdir("midImages")

subprocess.check_call("cmake ..".split())
subprocess.check_call("make".split())

subprocess.check_call("./LPR".split())

# import pprint
# pprint.pprint(lpr("chars"))

results = lpr("chars")

char_images = os.listdir("chars")

plate = "苏"
for char in char_images:
	plate += results[char]

print "最终结果:%s" % plate
