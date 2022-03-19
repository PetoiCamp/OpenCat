#!/usr/bin/python3
# -*- coding: UTF-8 -*-
# autopep8 -i

# A complex skill to make Bittle climb up a steel ceiling with meganets on its feet.
# Demo: https://youtu.be/ravFaiJzrpo
import sys
sys.path.append("..")
from ardSerial import *
import copy


balance = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30]
buttUp = [
    1, 0, 15, 1,
    20,  40,   0,   0,   5,   5,   3,   3,  90,  90,  45,  45, -60, -60,   5,   5]
calib = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0]
dropped = [
    1, 0, -75, 1,
    0,  30,   0,   0,  -5,  -5,  15,  15, -75, -75,  45,  45,  60,  60, -30, -30]
lifted = [
    1, 0, 75, 1,
    0, -20,   0,   0,   0,   0,   0,   0,  60,  60,  75,  75,  45,  45,  75,  75]
rest = [
    1, 0, 0, 1,
    -30, -80, -45,   0,  -3,  -3,   3,   3,  70,  70,  70,  70, -55, -55, -55, -55]
sit = [
    1, 0, -30, 1,
    0,   0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45]
str = [
    1, 0, 20, 1,
    0,  30,   0,   0,  -5,  -5,   0,   0, -75, -75,  30,  30,  60,  60,   0,   0]
zero = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0]

skill = {
    "balance": balance,
    "buttUp": buttUp,
    "calib": calib,
    "dropped": dropped,
    "lifted": lifted,
    "rest": rest,
    "sit": sit,
    "str": str,
    "zero": zero
}


def schedulerToSkill(testSchedule):
    compactSkillData = []
    newSkill = []
    outputStr = ""

    def deepCopy(dest, source):
        for i in range(len(source)):
            dest[i] = source[i]
        return dest

    for task in testSchedule:  # execute the tasks in the testSchedule
        #            print(task)
        token = task[0][0]
        if token == 'k':
            currentRow = skill[task[0][1:]][-16:]
            skillRow = copy.deepcopy(currentRow)
            compactSkillData.append(skillRow + [8, int(task[1]*1000/500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[1]*1000/500), 0, 0]

        elif token == 'i' or token == 'I':
            currentRow = copy.deepcopy(skillRow)
            for e in range(0, len(task[1]), 2):
                #                    print(task[1][e],task[1][e+1])
                currentRow[task[1][e]] = task[1][e+1]
            skillRow = copy.deepcopy(currentRow)
            compactSkillData.append(skillRow + [8, int(task[2]*1000/500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[2]*1000/500), 0, 0]

        elif token == 'm':
            currentRow = copy.deepcopy(skillRow)
            for e in range(0, len(task[1]), 2):
                currentRow[task[1][e]] = task[1][e+1]
                skillRow = copy.deepcopy(currentRow)
                compactSkillData.append(skillRow + [8, int(task[2]*1000/500), 0, 0])
                newSkill = newSkill + skillRow + [8, int(task[2]*1000/500), 0, 0]

    print('{')
    print(*[-len(compactSkillData), 0, 0, 1], sep=',\t', end=',\n')
    print(*[0, 0, 0], sep=',\t', end=',\n')
    for row in compactSkillData:
        print(*row, sep=',\t', end=',\n')
    print('};')

    newSkill = [-len(compactSkillData), 0, 0, 1, 0, 0, 0] + newSkill
    print(newSkill)
    wrapper(['K', newSkill, 1])


if __name__ == '__main__':
    try:
        flushSerialOutput(300)
        time.sleep(3)

        '''
        testSchedule is used to test various serial port commands
        '''
        testSchedule = [
            # - 'kbalance' indicates the command to control Bittle to stand normally
            # - 2 indicates the postponed time after finishing the command, in seconds
            ['g', 0.5],
            ['kbalance', 1],
            ['ksit', 2],
            ['I', [11, 75, 15, -30, 8, -90, 12, 45, 9, 20, 13, 90], 0.5],
            ['I', [8, -100, 12, 30], 0.5],
            ['m', [8, -128, 12, 85], 1],
            ['m', [9, -100, 13, 45], 0.5],
            ['I', [9, -128, 13, 85, 10, 70, 14, -20, 15, -20], 1],
            ['I', [8, -80, 9, -80, 12, 10, 13, 10, 10, 127, 11, 127, 14, -80, 15, -80], 0.5],
            ['I', [10, -15, 11, -15, 14, 70, 15, 70], 1.5],
            ['I', [8, -70, 9, -70, 12, -10, 13, -10, 10, 30, 11, 30], 0.5],
            ['I', [8, 50, 9, 50, 12, -50, 13, -50, 10, 127, 11, 127, 14, 90, 15, 90], 1],
            ['m', [11, -20, 15, 40], 1],
            ['m', [15, 90], 1],
            ['m', [14, 0, 10, -20], 1],
            ['I', [14, 90, 10, -20], 1],
            ['kbalance', 2],
            ['i', [0, -80, 1, 20], 2],
            ['d', 1],
        ]

#        for task in testSchedule:  # execute the tasks in the testSchedule
#            print(task)
#            token = task[0][0]
#
#            wrapper(task)
#            printSerialMessage(token)
#            time.sleep(task[-1])

        schedulerToSkill(testSchedule)
        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
