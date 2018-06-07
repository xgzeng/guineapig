#!/usr/local/bin/python3
from ScriptingBridge import *

photos_app = SBApplication.applicationWithBundleIdentifier_("com.apple.Photos")

if not photos_app.isRunning():
    print("Photos is not running")

folders = photos_app.folders()
print(len(folders))

#dir(photos_app)