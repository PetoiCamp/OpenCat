# New skill library for OpenCat robots

This file is a collection of new skills that can be performed by OpenCat robots, such as Nybble and Bittle. There are several ways to enable the new skills:

1. Import them through the Petoi Desktop app -> [Skill Composer](https://docs.petoi.com/desktop-app/skill-composer). 
2. Insert it in [OpenCat/serialMaster/example.py](https://docs.petoi.com/api/serialmaster-user-guide#method-2-run-the-custom-scheduler-example.py).

3. Integrate the skill directly into the OpenCat main code. But it requires a deeper understanding of the [data structure](https://bittle.petoi.com/11-tutorial-on-creating-new-skills). 

You can create new skills with the Skill Composer and you are welcome to share your skills by sending merge requests to this file. 

```
# Creator: XXX
# Country: XXX
# Date: Sep 9, 2022
# Note: Make Nybble do push-ups with one hand. More find movements added.
{
.....
}
```

\# The data array must be surrounded by {}. The skill composer will use the first pair of {} to parse the array, so you don't need to delete the commenting information. 

