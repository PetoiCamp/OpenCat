#!/usr/bin/python3
#  -*- coding: UTF-8 -*-

# MindPlus
# Python
from PetoiRobot import *

autoConnect()
# The list format is [joint index, angle, joint index, angle...]
sendSkillStr('ksit',3)
sendSkillStr('kup',3)
rotateJoints('M', absValList(0, 60), 1)
# The list format is [note, duration, note, duration...]
play('B', [14,4,14,4,21,4,21,4,23,4,23,4,21,2], 1)
closePort()
