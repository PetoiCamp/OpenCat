# New skill library for OpenCat robots

This folder is a collection of new skills that can be performed by OpenCat robots, such as Nybble and Bittle. 

There are several ways to perform the new skills:

1. Import a skill file through the Petoi Desktop app -> [Skill Composer](https://docs.petoi.com/desktop-app/skill-composer). It will be saved to the robot's memory and overwrite the last sent skill. Use token 'T' to call the skill. You can send 'T' through the serial command or assign it to one of the infrared remote key. 

2. Insert it in [OpenCat/serialMaster/example.py](https://docs.petoi.com/api/serialmaster-user-guide#method-2-run-the-custom-scheduler-example.py).

3. Integrate the skill directly into the OpenCat main code. But it requires a deeper understanding of the [data structure](https://bittle.petoi.com/11-tutorial-on-creating-new-skills). 

You can check the [Video Tutorial](https://youtu.be/kyN5vFY9fAI) and create new skills with the [Skill Composer](https://docs.petoi.com/desktop-app/skill-composer).

You are welcome to share your skills by sending merge requests to this folder. 


```
# Name: Model-ShortSkillName
# Creator: XXX
# Country: XXX
# Date: Sep 9, 2022
# Note: Make Nybble do push-ups with one hand. More fine movements are added.
# Demo Video link
{
  skill data array
  .....
}
```

The data array must be surrounded by {}. The skill composer will use the first pair of {} to parse the array, so you don't need to worry about the commenting information. 
